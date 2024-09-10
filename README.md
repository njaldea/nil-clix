# nil/clix

This library is only intended to simplify arg parsing and allow nesting of sub commands. Internally uses `boost::program_options`.

Simplification is done by limiting the touch points to the internal library and providing more concrete option types with very opinionated defaults.

## Objects

### `nil::clix::Node`

Boost program options does not inherently support nesting of commands. This Node is simply a way to chain the commands.

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

### `nil::clix::Options`

This class provides a way to access the parsed options.
Content will reflect the options added to the `Node`.

| method                                                |
| ----------------------------------------------------- |
| `bool flag(option, lkey) const`                       |
| `int number(option, lkey) const`                      |
| `std::string param(option, lkey) const`               |
| `std::vector<std::string> params(option, lkey) const` |

## Example

```cpp
#include <nil/clix.hpp>

#include <iostream>

void command(nil::clix::Node& node)
{
    // clang-format off
    flag  (node, "help",   { .skey ='h', .msg = "show this help"                                        });
    flag  (node, "spawn",  { .skey ='s', .msg = "spawn"                                                 });
    number(node, "thread", { .skey ='t', .msg = "number of threads"                                     });
    number(node, "job",    { .skey ='j', .msg = "number of jobs"    , .fallback = 1     , .implicit = 0 });
    param (node, "param",  { .skey ='p', .msg = "default param"     , .fallback = "123"                 });
    params(node, "mparam", { .skey ='m', .msg = "multiple params"                                       });
    // clang-format on
    use(node,
        [](const nil::clix::Options& options)
        {
            if (options.flag("help"))
            {
                options.help(std::cout);
                return 0;
            }
            std::cout                                                     //
                << "flag   -s: " << flag(options, "spawn") << std::endl    //
                << "number -t: " << number(options, "thread") << std::endl //
                << "number -j: " << number(options, "job") << std::endl    //
                << "param  -p: " << param(options, "param") << std::endl   //
                << "params -m: " << std::endl;
            for (const auto& item : params(options, "mparam"))
            {
                std::cout << " -  " << item << std::endl;
            }
            return 0;
        }
    );
}

int main(int argc, const char** argv)
{
    auto root = nil::clix::create_node();
    command(root);
    use(root,
        "hello",
        "command for 1:hello",
        [](nil::clix::Node& node)
        {
            command(node);
            sub(node, "world", "command for 2:world", command);
        }
    );
    add(root,
        "another",
        "command for 3:another",
        [](nil::clix::Node& node)
        {
            command(node);
            sub(node, "dimension", "command for 4:vector", command);
        }
    );
    return run(root, argc, argv);
}
```

```
terminal$ executable hello -h
OPTIONS:
  -h [ --help ]                     show this help
  -s [ --spawn ]                    spawn
  -t [ --thread ] [=value(=1)] (=0) number of threads
  -j [ --job ] [=value(=0)] (=1)    number of jobs
  -p [ --param ] text (="123")      default param
  -m [ --mparam ] text              multiple params

SUBCOMMANDS:
  world                             command for 2:world
```