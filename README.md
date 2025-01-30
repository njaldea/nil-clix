# nil/clix

This library is only intended to simplify arg parsing and allow nesting of sub commands. Internally uses `boost::program_options`.

Simplification is done by limiting the touch points to the internal library and providing more concrete option types with very opinionated defaults.

## Objects

### `nil::clix::Node`

Boost program options does not inherently support nesting of commands.
This Node is simply a way to chain the commands.

| method                                        | decription                                                                |
| --------------------------------------------- | ------------------------------------------------------------------------- |
| `void flag(node, lkey, conf::Flag)`           | register a flag option (bool)                                             |
| `void number(node, lkey, conf::Number)`       | register a flag option (int64_t)                                          |
| `void param(node, lkey, conf::Param)`         | register a flag option (string)                                           |
| `void params(node, lkey, conf::Params)`       | register a flag option (list of strings)                                  |
| `void use(node, runner)`                      | register a runner which will be called when node is executed              |
| `void sub(node, key, description, sub_impl)`  | register a sub node with sub_impl defining its setup                      |
| `void run(node, argc, argv)`                  | start running the node                                                    |
| `N create_node()`                             | create a node. returns a proxy to allow compatibility with other methods  |

It is not allowed to create a Node object is directly. <br/>
`nil::clix::create_node()` method returns a proxy object that will own the Node instance. <br/>
This is compatible to all methods that receives the Node object as described above

### `nil::clix::Options`

This class provides a way to access the parsed options.
Content will reflect the options added to the `Node`.

| method                                                |
| ----------------------------------------------------- |
| `bool has_value(option, lkey) const`                  |
| `bool flag(option, lkey) const`                       |
| `int number(option, lkey) const`                      |
| `std::string param(option, lkey) const`               |
| `std::vector<std::string> params(option, lkey) const` |

## Example

```cpp
#include <nil/clix.hpp>

#include <iostream>

int main(int argc, const char** argv)
{
    auto root = nil::clix::create_node();
    flag(root, "help", {.skey = 'h', .msg = "show this help"});
    sub(root,
        "flags",
        "command for flags",
        [](nil::clix::Node& node)
        {
            flag(node, "help", {.skey = 'h', .msg = "show this help"});
            flag(node, "spawn", {.skey = 's', .msg = "spawn"});
            use(node,
                [](const nil::clix::Options& options)
                {
                    if (flag(options, "help"))
                    {
                        help(options, std::cout);
                        return 0;
                    }
                    std::cout << "Flag Node is executing\n" << std::endl;
                    return 0;
                });
        });
    sub(root,
        "numbers",
        "command for numbers",
        [](nil::clix::Node& node)
        {
            flag(node, "help", {.skey = 'h', .msg = "show this help"});
            number(node, "thread", {.skey = 't', .msg = "number of threads"});
            number(
                node,
                "job",
                {.skey = 'j', .msg = "number of jobs", .fallback = 1, .implicit = 0}
            );
            use(node,
                [](const nil::clix::Options& options)
                {
                    if (flag(options, "help"))
                    {
                        help(options, std::cout);
                        return 0;
                    }
                    std::cout << "Numbers Node is executing\n" << std::endl;
                    return 0;
                });
        });
    sub(root,
        "params",
        "command for params",
        [](nil::clix::Node& node)
        {
            flag(node, "help", {.skey = 'h', .msg = "show this help"});
            param(node, "param", {.skey = 'p', .msg = "default param", .fallback = "123"});
            params(node, "mparam", {.skey = 'm', .msg = "multiple params"});
            use(node,
                [](const nil::clix::Options& options)
                {
                    if (flag(options, "help"))
                    {
                        help(options, std::cout);
                        return 0;
                    }
                    std::cout << "Params Node is executing\n" << std::endl;
                    return 0;
                });
        });
    use(root,
        [](const nil::clix::Options& options)
        {
            if (flag(options, "help"))
            {
                help(options, std::cout);
                return 0;
            }
            std::cout << "Root Node is executing\n" << std::endl;
            return 0;
        });
    return run(root, argc, argv);
}
```

### Preview (Root)
```
terminal$ executable -h
Root Node is executing

terminal$ executable -h
OPTIONS:
  -h [ --help ]         show this help

SUBCOMMANDS:
  flags                 command for flags
  numbers               command for numbers
  params                command for params

```

### Preview (flags subcommand)

```
terminal$ executable flags -h
Flag Node is executing

terminal$ executable flags -h
OPTIONS:
  -h [ --help ]         show this help
  -s [ --spawn ]        spawn

```

### Preview (numbers subcommand)

```
terminal$ executable numbers
Numbers Node is executing

terminal$ executable numbers -h
OPTIONS:
  -h [ --help ]                  show this help
  -t [ --thread ] value          number of threads
  -j [ --job ] [=value(=0)] (=1) number of jobs

```

### Preview (params subcommand)

```
terminal$ executable params
Params Node is executing

terminal$ executable params -h
OPTIONS:
  -h [ --help ]                show this help
  -p [ --param ] text (="123") default param
  -m [ --mparam ] text         multiple params

```