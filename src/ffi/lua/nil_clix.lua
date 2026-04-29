local ffi = require("ffi")

ffi.cdef [[
    void *malloc(size_t size);
    void free(void* ptr);
]]

ffi.cdef [[
    typedef struct nil_clix_node
    {
        void* handle;
    } nil_clix_node;

    typedef struct nil_clix_options
    {
        void* handle;
    } nil_clix_options;

    nil_clix_node nil_clix_node_create(void);
    void nil_clix_node_destroy(nil_clix_node node);
    int nil_clix_node_run(nil_clix_node node, int argc, const char* const* argv);

    typedef struct nil_clix_flag_info
    {
        const char* skey;
        const char* msg;
    } nil_clix_flag_info;

    typedef struct nil_clix_number_info
    {
        const char* skey;
        const char* msg;
        const int64_t* fallback;
        const int64_t* implicit;
    } nil_clix_number_info;

    typedef struct nil_clix_param_info
    {
        const char* skey;
        const char* msg;
        const char* fallback;
    } nil_clix_param_info;

    typedef struct nil_clix_params_info
    {
        const char* skey;
        const char* msg;
    } nil_clix_params_info;

    void nil_clix_node_flag(nil_clix_node node, const char* lkey, nil_clix_flag_info info);
    void nil_clix_node_number(nil_clix_node node, const char* lkey, nil_clix_number_info info);
    void nil_clix_node_param(nil_clix_node node, const char* lkey, nil_clix_param_info info);
    void nil_clix_node_params(nil_clix_node node, const char* lkey, nil_clix_params_info info);

    typedef struct nil_clix_exec_info
    {
        int (*exec)(const nil_clix_options* options, void*);
        void* context;
        void (*cleanup)(void*);
    } nil_clix_exec_info;

    typedef struct nil_clix_sub_info
    {
        void (*exec)(nil_clix_node* node, void*);
        void* context;
        void (*cleanup)(void*);
    } nil_clix_sub_info;

    void nil_clix_node_use(nil_clix_node node, nil_clix_exec_info callback);

    void nil_clix_node_sub(
        nil_clix_node node,
        const char* key,
        const char* description,
        nil_clix_sub_info callback
    );

    typedef struct nil_clix_write_info
    {
        void (*exec)(const char* data, uint64_t size, void*);
        void* context;
    } nil_clix_write_info;

    typedef struct nil_clix_strings
    {
        uint64_t size;
        const char* const* data;
    } nil_clix_strings;

    void nil_clix_options_help(nil_clix_options options, nil_clix_write_info writer);
    int nil_clix_options_has_value(nil_clix_options options, const char* lkey);
    int nil_clix_options_flag(nil_clix_options options, const char* lkey);
    int64_t nil_clix_options_number(nil_clix_options options, const char* lkey);
    const char* nil_clix_options_param(nil_clix_options options, const char* lkey);
    nil_clix_strings nil_clix_options_params(nil_clix_options options, const char* lkey);
]]

local function to_ref_id(id)
    return tonumber(ffi.cast("uintptr_t", id))
end

local function current_file_dir()
    local source = debug.getinfo(1, "S").source
    if source:sub(1, 1) == "@" then
        source = source:sub(2)
    end

    local dir = source:match("(.*/)")
    if dir == nil then
        return "./"
    end

    return dir
end

---@class nil_clix.Options
---@field help fun(self: nil_clix.Options): string
---@field has_value fun(self: nil_clix.Options, lkey: string): boolean
---@field flag fun(self: nil_clix.Options, lkey: string): boolean
---@field number fun(self: nil_clix.Options, lkey: string): number
---@field param fun(self: nil_clix.Options, lkey: string): string|nil
---@field params fun(self: nil_clix.Options, lkey: string): string[]

---@class nil_clix.Node
---@field flag fun(self: nil_clix.Node, lkey: string, skey: string|nil, msg: string)
---@field number fun(self: nil_clix.Node, lkey: string, skey: string|nil, msg: string, fallback: number|nil, implicit: number|nil)
---@field param fun(self: nil_clix.Node, lkey: string, skey: string|nil, msg: string, fallback: string|nil)
---@field params fun(self: nil_clix.Node, lkey: string, skey: string|nil, msg: string)
---@field use fun(self: nil_clix.Node, fn: fun(options: nil_clix.Options): number)
---@field sub fun(self: nil_clix.Node, key: string, description: string, fn: fun(node: nil_clix.Node))
---@field run fun(self: nil_clix.Node, argv: string[]): number
---@field destroy fun(self: nil_clix.Node)

---@class nil_clix
---@field create_node fun(): nil_clix.Node

---@return nil_clix.Options
local function create_options(refs, lua_fns, clix, options)
    return {
        _options = options,
        help = function(self)
            local output = {}
            local writer_id = ffi.C.malloc(1)
            refs[to_ref_id(writer_id)] = { output = output }

            local write_info = ffi.new("nil_clix_write_info")
            write_info.exec = lua_fns.write_exec
            write_info.context = writer_id

            clix.nil_clix_options_help(self._options, write_info)
            return table.concat(output)
        end,
        has_value = function(self, lkey)
            return clix.nil_clix_options_has_value(self._options, lkey) ~= 0
        end,
        flag = function(self, lkey)
            return clix.nil_clix_options_flag(self._options, lkey) ~= 0
        end,
        number = function(self, lkey)
            return tonumber(clix.nil_clix_options_number(self._options, lkey))
        end,
        param = function(self, lkey)
            local result = clix.nil_clix_options_param(self._options, lkey)
            if result == nil then
                return nil
            end
            return ffi.string(result)
        end,
        params = function(self, lkey)
            local strings = clix.nil_clix_options_params(self._options, lkey)
            local result = {}
            for i = 0, strings.size - 1, 1 do
                result[i + 1] = ffi.string(strings.data[i])
            end
            return result
        end
    }
end

---@return nil_clix.Node
local function create_node(refs, lua_fns, clix, node)
    return {
        _node = node,
        flag = function(self, lkey, skey, msg)
            local flag_info = ffi.new("nil_clix_flag_info")
            flag_info.skey = skey
            flag_info.msg = msg
            clix.nil_clix_node_flag(self._node, lkey, flag_info)
        end,
        number = function(self, lkey, skey, msg, fallback, implicit)
            local number_info = ffi.new("nil_clix_number_info")
            number_info.skey = skey
            number_info.msg = msg

            local fallback_ptr = nil
            local implicit_ptr = nil

            if fallback ~= nil then
                fallback_ptr = ffi.new("int64_t")
                fallback_ptr[0] = fallback
            end

            if implicit ~= nil then
                implicit_ptr = ffi.new("int64_t")
                implicit_ptr[0] = implicit
            end

            number_info.fallback = fallback_ptr
            number_info.implicit = implicit_ptr

            clix.nil_clix_node_number(self._node, lkey, number_info)
        end,
        param = function(self, lkey, skey, msg, fallback)
            local param_info = ffi.new("nil_clix_param_info")
            param_info.skey = skey
            param_info.msg = msg
            param_info.fallback = fallback
            clix.nil_clix_node_param(self._node, lkey, param_info)
        end,
        params = function(self, lkey, skey, msg)
            local params_info = ffi.new("nil_clix_params_info")
            params_info.skey = skey
            params_info.msg = msg
            clix.nil_clix_node_params(self._node, lkey, params_info)
        end,
        use = function(self, fn)
            local id = ffi.C.malloc(1)
            local exec_info = ffi.new("nil_clix_exec_info")

            exec_info.exec = lua_fns.exec_exec
            exec_info.cleanup = lua_fns.cleanup
            exec_info.context = id

            refs[to_ref_id(id)] = { fn = fn }
            clix.nil_clix_node_use(self._node, exec_info)
        end,
        sub = function(self, key, description, fn)
            local id = ffi.C.malloc(1)
            local sub_info = ffi.new("nil_clix_sub_info")

            sub_info.exec = lua_fns.sub_exec
            sub_info.cleanup = lua_fns.cleanup
            sub_info.context = id

            refs[to_ref_id(id)] = { fn = fn }
            clix.nil_clix_node_sub(self._node, key, description, sub_info)
        end,
        run = function(self, argv)
            local argc = #argv
            local c_argv = ffi.new("const char*[?]", argc)

            for i, arg in ipairs(argv) do
                c_argv[i - 1] = arg
            end

            return tonumber(clix.nil_clix_node_run(self._node, argc, c_argv))
        end,
        destroy = function(self)
            clix.nil_clix_node_destroy(self._node)
        end
    }
end

local clix = ffi.load(current_file_dir() .. "libnil-clix-c-api.so")

---@return nil_clix
local function create_clix()
    local refs = {}
    local lua_fns = {}

    local exec_exec = ffi.cast(
        "int (*)(const nil_clix_options*, void*)",
        function(options_ptr, id)
            local options_obj = create_options(refs, lua_fns, clix, options_ptr[0])
            return refs[to_ref_id(id)].fn(options_obj)
        end
    )

    local sub_exec = ffi.cast(
        "void (*)(nil_clix_node*, void*)",
        function(node_ptr, id)
            local node_obj = create_node(refs, lua_fns, clix, node_ptr[0])
            refs[to_ref_id(id)].fn(node_obj)
        end
    )

    local write_exec = ffi.cast(
        "void (*)(const char*, uint64_t, void*)",
        function(data, size, id)
            local ref = refs[to_ref_id(id)]
            if ref and ref.output then
                table.insert(ref.output, ffi.string(data, size))
            end
        end
    )

    local cleanup = ffi.cast(
        "void (*)(void*)",
        function(id)
            if id ~= nil then
                refs[to_ref_id(id)] = nil
                ffi.C.free(id)
            end
        end
    )

    lua_fns = {
        exec_exec = exec_exec,
        sub_exec = sub_exec,
        write_exec = write_exec,
        cleanup = cleanup
    }

    return {
        create_node = function()
            local node = clix.nil_clix_node_create()
            return create_node(refs, lua_fns, clix, node)
        end
    }
end

return create_clix()