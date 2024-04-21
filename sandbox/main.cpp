
#include <nil/clix.hpp>
#include <nil/clix/prebuilt/Help.hpp>

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
    using nil::clix::Node;
    using nil::clix::prebuilt::Help;

    Node root;
    command(root);
    root.add(
        "hello",
        "command for 1:hello",
        [](Node& node)
        {
            command(node);
            node.add("world", "command for 2:world", Help(&std::cout));
        }
    );
    root.add(
        "another",
        "command for 3:another",
        [](Node& node)
        {
            command(node);
            node.add("dimension", "command for 4:vector", Help(&std::cout));
        }
    );
    return root.run(argc, argv);
}
