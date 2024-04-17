#include <nil/clix/nodes/Help.hpp>

#include <nil/clix/Builder.hpp>
#include <nil/clix/Options.hpp>

namespace nil::clix::nodes
{
    Help::Help(std::ostream& init_os)
        : os(init_os)
    {
    }

    OptionInfo Help::options() const
    {
        return nil::clix::Builder() //
            .flag("help", {.skey = 'h', .msg = "this help"})
            .build();
    }

    int Help::run(const nil::clix::Options& options) const
    {
        options.help(os);
        return 0;
    }
} // namespace nil::clix::nodes
