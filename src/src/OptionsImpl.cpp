#include <nil/clix/Node.hpp>

#include "Option.hpp"
#include "OptionsImpl.hpp"

#include <iomanip>

namespace nil::clix
{
    Options::Impl::Impl(
        const std::vector<std::unique_ptr<IOption>>& info,
        const std::vector<SubNode>& subnodes,
        int argc,
        const char* const* argv
    )
        : desc("OPTIONS")
    {
        auto options = desc.add_options();
        for (const auto& option : info)
        {
            option->apply(IOption::Impl{&options});
        }

        try
        {
            boost::program_options::positional_options_description positional;
            positional.add("__nil_cli_pos_args", 0);

            boost::program_options::command_line_parser parser(argc, argv);
            parser             //
                .options(desc) //
                .positional(positional);

            boost::program_options::store(parser.run(), vm);
        }
        catch (const std::exception& ex)
        {
            throw std::invalid_argument(std::string("[nil][cli] ") + ex.what());
        }

        for (const auto& node : subnodes)
        {
            sub.emplace_back(node.key, node.description);
        }
    }

    void Options::Impl::help(std::ostream& os) const
    {
        if (desc.options().empty() && sub.empty())
        {
            os << "No option available\n";
            return;
        }

        if (!desc.options().empty())
        {
            desc.print(os);
            os << '\n';
        }

        if (!sub.empty())
        {
            os << "SUBCOMMANDS:\n";
            const auto width = desc.get_option_column_width();
            for (const auto& [key, subdesc] : sub)
            {
                os << "  "                      //
                   << std::left                 //
                   << std::setw(int(width - 2)) //
                   << key;
                // iterate per character.
                // add padding when newline is found.
                // expensive but idc since this is not a hot path.
                for (const auto& c : subdesc)
                {
                    if (c == '\n')
                    {
                        os << std::setw(int(width));
                    }
                    os << c;
                }
                os << '\n';
            }
            os << '\n';
        }
    }
}
