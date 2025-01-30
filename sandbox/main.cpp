
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
