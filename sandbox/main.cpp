
#include <nil/clix.hpp>
#include <nil/clix/prebuilt/Help.hpp>

#include <iostream>

void command(nil::clix::Node& node)
{
    // clang-format off
    flag  (node, "help",            { .skey ='h', .msg = "show this help"                                       });
    flag  (node, "spawn",           { .skey ='b', .msg = "spawn"                                                });
    number(node, "fallback_number", { .skey ='n', .msg = "fallback number"  , .fallback = 1     , .implicit = 0 });
    number(node, "optional_number", { .skey ='m', .msg = "fallback number"                                      });
    param (node, "fallback_param",  { .skey ='s', .msg = "fallback param"   , .fallback = "fff"                 });
    param (node, "optional_param",  { .skey ='t', .msg = "optional param"                                       });
    params(node, "mparam",          { .skey ='z', .msg = "multiple params"                                      });
    // clang-format on

    use(node,
        [](const nil::clix::Options& options)
        {
            if (flag(options, "help"))
            {
                help(options, std::cout);
                return 0;
            }
            std::cout                                                                        //
                << "flag            -b: " << flag(options, "spawn") << std::endl             //
                << "fallback_number -n: " << number(options, "fallback_number") << std::endl //
                << "fallback_param  -s: " << param(options, "fallback_param") << std::endl   //
                << "params          -z: " << std::endl;

            if (has_value(options, "optional_number"))
            {
                std::cout << "optional_number -m: " << param(options, "optional_number")
                          << std::endl;
            }
            else
            {
                std::cout << "optional_number -m: not provided" << std::endl;
            }

            if (has_value(options, "optional_param"))
            {
                std::cout << "optional_param  -t: " << param(options, "optional_param")
                          << std::endl;
            }
            else
            {
                std::cout << "optional_param  -t: not provided" << std::endl;
            }

            if (has_value(options, "mparam"))
            {
                for (const auto& item : params(options, "mparam"))
                {
                    std::cout << " -  : " << item << std::endl;
                }
            }
            else
            {
                std::cout << " -  : no mparam" << std::endl;
            }
            return 0;
        });
}

int main(int argc, const char** argv)
{
    using nil::clix::prebuilt::Help;

    auto root = nil::clix::create_node();
    command(root);
    sub(root,
        "hello",
        "command for 1:hello",
        [](nil::clix::Node& node)
        {
            command(node);
            sub(node, "world", "command for 2:world", Help(&std::cout));
        });
    sub(root,
        "another",
        "command for 3:another",
        [](nil::clix::Node& node)
        {
            command(node);
            sub(node, "dimension", "command for 4:vector", Help(&std::cout));
        });
    return run(root, argc, argv);
}
