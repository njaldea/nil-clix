# nil-clix

Lua binding for **nil-clix** – an Opinionated Argument Parser

## Links

- **Repository:** https://github.com/njaldea/nil-clix
- **C API Documentation:** https://github.com/njaldea/nil-clix/blob/master/docs/c-api.md

## Usage

```lua
local clix = require("nil_clix")

local node = clix.create_node()

-- Add flags
node.flag("verbose", { skey = "v", msg = "Enable verbose output" })

-- Add parameters
node.param("config", { skey = "c", msg = "Path to config file", fallback = "config.yml" })

-- Add numbers
node.number("threads", { skey = "t", msg = "Number of threads", fallback = 4 })

-- Add handler
node.use(function(options)
    if options.flag("verbose") then
        print("Verbose mode enabled")
    end
    local config = options.param("config")
    local threads = options.number("threads")
    print(string.format("Config: %s, Threads: %d", config, threads))
    return 0
end)

-- Run
local exit_code = node.run({ "-v", "-c", "my.yml", "-t", "8" })
```

### Subcommands

```lua
local clix = require("nil_clix")

local node = clix.create_node()

node.sub("init", "Initialize the project", function(cmd)
    cmd.flag("force", { skey = "f", msg = "Force initialization" })
    cmd.use(function(options)
        if options.flag("force") then
            print("Init: forced")
        else
            print("Init: normal")
        end
        return 0
    end)
end)

local exit_code = node.run({ "init", "-f" })
```

## Lifetime Notes

Root nodes are owning handles. The Lua binding registers a GC finalizer so
 the underlying node is destroyed if the object is collected. Finalizer timing
 is nondeterministic and may not run at shutdown, so call `destroy()` for
 deterministic teardown. Subcommand nodes passed to callbacks are non-owning
 views and must not be stored or destroyed.

## Documentation

For detailed API documentation and more examples, visit:
- [C++ Documentation](https://github.com/njaldea/nil-clix)
- [C API Guide](https://github.com/njaldea/nil-clix/blob/master/docs/c-api.md)

## License

CC BY-NC-ND 4.0

## Support

For issues, questions, or contributions, visit the [GitHub repository](https://github.com/njaldea/nil-clix).
