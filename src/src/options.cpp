#include "nil/clix/conf/Flag.hpp"
#include "structs.hpp"

#include <iomanip>

namespace nil::clix
{
    void apply(
        const std::string& lkey,
        const conf::Flag& conf,
        boost::program_options::options_description_easy_init& i
    )
    {
        const auto opt = conf.skey ? (lkey + ',' + *conf.skey) : lkey;
        auto* value = boost::program_options::value<bool>();
        value->zero_tokens();
        value->default_value(false);
        i(opt.c_str(), value, conf.msg.value_or("").c_str());
    }

    void apply(
        const std::string& lkey,
        const conf::Number& conf,
        boost::program_options::options_description_easy_init& i
    )
    {
        const auto opt = conf.skey ? (lkey + ',' + *conf.skey) : lkey;
        auto* value = boost::program_options::value<std::int64_t>();
        value->value_name("value");
        value->implicit_value(conf.implicit, std::to_string(conf.implicit));
        value->default_value(conf.fallback, std::to_string(conf.fallback));
        i(opt.c_str(), value, conf.msg.value_or("").c_str());
    }

    void apply(
        const std::string& lkey,
        const conf::Param& conf,
        boost::program_options::options_description_easy_init& i
    )
    {
        const auto opt = conf.skey ? (lkey + ',' + *conf.skey) : lkey;
        auto* value = boost::program_options::value<std::string>();
        value->value_name("text");
        if (conf.fallback.has_value())
        {
            value->default_value(conf.fallback.value(), "\"" + conf.fallback.value() + "\"");
        }
        else
        {
            value->required();
        }
        i(opt.c_str(), value, conf.msg.value_or("").c_str());
    }

    void apply(
        const std::string& lkey,
        const conf::Params& conf,
        boost::program_options::options_description_easy_init& i
    )
    {
        const auto opt = conf.skey ? (lkey + ',' + *conf.skey) : lkey;
        auto* value = boost::program_options::value<std::vector<std::string>>();
        value->value_name("text");
        value->multitoken();
        value->default_value({}, "");
        i(opt.c_str(), value, conf.msg.value_or("").c_str());
    }

    Options parse(
        const std::vector<Node::Opt>& opts,
        const std::vector<SubNode>& subs,
        int argc,
        const char* const* argv
    )
    {
        auto retval = Options{
            boost::program_options::options_description{"OPTIONS"},
            boost::program_options::variables_map{},
            std::vector<std::tuple<std::string, std::string>>{}
        };
        auto options = retval.desc.add_options();
        for (const auto& o : opts)
        {
            std::visit([&o, &options](const auto& v) { apply(o.lkey, v, options); }, o.option);
        }

        try
        {
            boost::program_options::positional_options_description positional;
            positional.add("__nil_cli_pos_args", 0);

            boost::program_options::command_line_parser parser(argc, argv);
            parser                    //
                .options(retval.desc) //
                .positional(positional);

            boost::program_options::store(parser.run(), retval.vm);
        }
        catch (const std::exception& ex)
        {
            throw std::invalid_argument(std::string("[nil][cli] ") + ex.what());
        }

        for (const auto& node : subs)
        {
            retval.sub.emplace_back(node.key, node.description);
        }

        return retval;
    }

    void help(const Options& options, std::ostream& os)
    {
        if (options.desc.options().empty() && options.sub.empty())
        {
            os << "No option available\n";
            return;
        }

        if (!options.desc.options().empty())
        {
            options.desc.print(os);
            os << '\n';
        }

        if (!options.sub.empty())
        {
            os << "SUBCOMMANDS:\n";
            const auto width = options.desc.get_option_column_width();
            for (const auto& [key, subdesc] : options.sub)
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

    namespace impl
    {
        template <typename T>
        auto access(const Options& options, const std::string& k)
        {
            try
            {
                return options.vm[k].as<T>();
            }
            catch (const std::exception&)
            {
                throw std::out_of_range("[nil][cli]: option \"" + k + "\" is unknown");
            }
        }
    }

    bool flag(const Options& options, const std::string& lkey)
    {
        return impl::access<bool>(options, lkey);
    }

    std::int64_t number(const Options& options, const std::string& lkey)
    {
        return impl::access<std::int64_t>(options, lkey);
    }

    std::string param(const Options& options, const std::string& lkey)
    {
        return impl::access<std::string>(options, lkey);
    }

    std::vector<std::string> params(const Options& options, const std::string& lkey)
    {
        return impl::access<std::vector<std::string>>(options, lkey);
    }
}
