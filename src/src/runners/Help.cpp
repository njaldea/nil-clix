#include <nil/clix/Options.hpp>
#include <nil/clix/runners/Help.hpp>

namespace nil::clix::runners
{
    Help::Help(std::ostream& init_oss)
        : oss(&init_oss)
    {
    }

    int Help::operator()(const Options& options) const
    {
        options.help(*oss);
        return 0;
    }
}
