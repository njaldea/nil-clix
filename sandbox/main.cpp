
#include <nil/clix.hpp>
#include <nil/clix/prebuilt/Help.hpp>

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
            if (flag(options, "help"))
            {
                help(options, std::cout);
                return 0;
            }
            std::cout                                                      //
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
