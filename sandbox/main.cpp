
#include <nil/clix.hpp>
#include <nil/clix/runners/Help.hpp>

#include <iostream>

template <int I>
void apply_runner(nil::clix::Node& node)
{
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

void apply_options(nil::clix::Node& node)
{
    // clang-format off
    node.flag  ("help",   { .skey ='h', .msg = "show this help"                                        });
    node.flag  ("spawn",  { .skey ='s', .msg = "spawn"                                                 });
    node.number("thread", { .skey ='t', .msg = "number of threads"                                     });
    node.number("job",    { .skey ='j', .msg = "number of jobs"    , .fallback = 1     , .implicit = 0 });
    node.param ("param",  { .skey ='p', .msg = "default param"     , .fallback = "123"                 });
    node.params("mparam", { .skey ='m', .msg = "multiple params"                                       });
    // clang-format on
}

nil::clix::Node compose()
{
    nil::clix::Node root;
    apply_runner<0>(root);
    apply_options(root);
    root.add(
        "hello",
        "command for 1:hello",
        [](auto& node)
        {
            apply_runner<1>(node);
            apply_options(node);
            node.add(
                "world",
                "command for 2:world",
                [](auto& n) { n.runner(nil::clix::runners::Help(std::cout)); }
            );
        }
    );
    root.add(
        "another",
        "command for 3:another",
        [](nil::clix::Node& node)
        {
            apply_runner<3>(node);
            apply_options(node);
            node.add(
                "dimension",
                "command for 4:vector",
                [](auto& n) { n.runner(nil::clix::runners::Help(std::cout)); }
            );
        }
    );
    return root;
}

int main(int argc, const char** argv)
{
    return compose().run(argc, argv);
}
