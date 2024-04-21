# nil/clix

This library is only intended to simplify arg parsing and allow nesting of sub commands. Internally uses `boost::program_options`.

Simplification is done by limiting the touch points to the internal library and providing more concrete option types with very opinionated defaults.

## Classes

### `nil::clix::Node`

Boost program options does not inherently support nesting of commands. This Node is simply a way to chain the commands.

- construct a `Node`
- add options via `flag`|`number`|`param`|`params`
- add subnodes via `node.add("key", "description", subnode_predicate)`
- trigger run to parse arguments

### `nil::clix::Options`

This class provides a way to access the parsed options.
Content will reflect the options added to the `Node`.

| method                                       |
| -------------------------------------------- |
| `bool flag(lkey) const`                      |
| `int number(lkey) const`                     |
| `std::string param(lkey) const`              |
| `std::vector<std::string params(lkey) const` |

## Example

```cpp
#include <nil/clix.hpp>

#include <iostream>

void command(nil::clix::Node& node)
{
    // clang-format off
    node.flag  ("help",   { .skey ='h', .msg = "show this help"                                        });
    node.flag  ("spawn",  { .skey ='s', .msg = "spawn"                                                 });
    node.number("thread", { .skey ='t', .msg = "number of threads"                                     });
    node.number("job",    { .skey ='j', .msg = "number of jobs"    , .fallback = 1     , .implicit = 0 });
    node.param ("param",  { .skey ='p', .msg = "default param"     , .fallback = "123"                 });
    node.params("mparam", { .skey ='m', .msg = "multiple params"                                       });
    // clang-format on
    node.runner(
        [](const nil::clix::Options& options)
        {
            if (options.flag("help"))
            {
                options.help(std::cout);
                return 0;
            }
            std::cout                                                     //
                << "flag   -s: " << options.flag("spawn") << std::endl    //
                << "number -t: " << options.number("thread") << std::endl //
                << "number -j: " << options.number("job") << std::endl    //
                << "param  -p: " << options.param("param") << std::endl   //
                << "params -m: " << std::endl;
            for (const auto& item : options.params("mparam"))
            {
                std::cout << " -  " << item << std::endl;
            }
            return 0;
        }
    );
}

int main(int argc, const char** argv)
{
    nil::clix::Node root;
    command(root);
    root.add(
        "hello",
        "command for 1:hello",
        [](nil::clix::Node& node)
        {
            command(node);
            node.add("world", "command for 2:world", command);
        }
    );
    root.add(
        "another",
        "command for 3:another",
        [](nil::clix::Node& node)
        {
            command(node);
            node.add("dimension", "command for 4:vector", command);
        }
    );
    return root.run(argc, argv);
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