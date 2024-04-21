#include <nil/clix/Node.hpp>
#include <nil/clix/Options.hpp>
#include <nil/clix/prebuilt/Help.hpp>

namespace nil::clix::prebuilt
{
    Help::Help(std::ostream* init_os)
        : os(init_os)
    {
    }

    Help::operator std::function<void(Node&)>() const
    {
        return [ostream = this->os](Node& node) { node.runner(Help(ostream)); };
    }

    Help::operator std::function<int(const Options&)>() const
    {
        return [ostream = this->os](const Options& options)
        {
            if (ostream != nullptr)
            {
                options.help(*ostream);
            }
            return 0;
        };
    }
}
