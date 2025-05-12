#include <nil/clix/prebuilt/Help.hpp>

#include <nil/clix/node.hpp>
#include <nil/clix/options.hpp>

namespace nil::clix::prebuilt
{
    Help::Help(std::ostream* init_os)
        : os(init_os)
    {
    }

    void Help::operator()(Node& node) const
    {
        use(node, *this);
    }

    int Help::operator()(const Options& options) const
    {
        if (os != nullptr)
        {
            help(options, *os);
        }
        return 0;
    }
}
