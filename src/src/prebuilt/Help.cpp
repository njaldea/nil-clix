#include <nil/clix/node.hpp>
#include <nil/clix/options.hpp>
#include <nil/clix/prebuilt/Help.hpp>

namespace nil::clix::prebuilt
{
    Help::Help(std::ostream* init_os)
        : os(init_os)
    {
    }

    Help::operator std::function<void(Node&)>() const
    {
        return [ostream = this->os](Node& node) { use(node, Help(ostream)); };
    }

    Help::operator std::function<int(const Options&)>() const
    {
        return [ostream = this->os](const Options& options)
        {
            if (ostream != nullptr)
            {
                help(options, *ostream);
            }
            return 0;
        };
    }
}
