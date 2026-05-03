package.path = package.path .. ";../src/ffi/lua/?.lua"

local clix = require("nil_clix")

local root = clix.create_node()
root:flag("help", { skey = "h", msg = "show this help" })

root:sub("flags", "command for flags", function(node)
    node:flag("help", { skey = "h", msg = "show this help" })
    node:flag("spawn", { skey = "s", msg = "spawn" })
    node:use(function(options)
        if options:flag("help") then
            print(options:help())
            return 0
        end
        print("Flag Node is executing")
        if options:has_value("spawn") then
            print("spawn: " .. tostring(options:flag("spawn")))
        end
        print("\n")
        return 0
    end)
end)

root:sub("numbers", "command for numbers", function(node)
    node:flag("help", { skey = "h", msg = "show this help" })
    node:number("thread", { skey = "t", msg = "number of threads" })
    node:number("job", { skey = "j", msg = "number of jobs", fallback = 1, implicit = 0 })
    node:use(function(options)
        if options:flag("help") then
            print(options:help())
            return 0
        end
        print("Numbers Node is executing")
        if options:has_value("thread") then
            print("thread: " .. options:number("thread"))
        end
        if options:has_value("job") then
            print("job: " .. options:number("job"))
        end
        print("\n")
        return 0
    end)
end)

root:sub("params", "command for params", function(node)
    node:flag("help", { skey = "h", msg = "show this help" })
    node:param("param", { skey = "p", msg = "default param" })
    node:params("mparam", { skey = "m", msg = "multiple params" })
    node:use(function(options)
        if options:flag("help") then
            print(options:help())
            return 0
        end
        print("Params Node is executing")
        if options:has_value("param") then
            print("param: " .. options:param("param"))
        end
        if options:has_value("mparam") then
            for _, m in ipairs(options:params("mparam")) do
                print("mparam: " .. m)
            end
        end
        print("\n")
        return 0
    end)
end)

root:use(function(options)
    if options:flag("help") then
        print(options:help())
        return 0
    end
    print("Root Node is executing")
    return 0
end)

return root:run(arg)
