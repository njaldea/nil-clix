#include "structs.hpp"

#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <variant>

namespace nil::clix
{
    template <typename T>
    std::string get_name(const std::string& lkey, const T& conf)
    {
        return conf.skey.has_value() ? ("-" + (conf.skey.value() + (",--" + lkey))) : ("--" + lkey);
    }

    std::string get_name(const std::string& lkey)
    {
        return "--" + lkey;
    }

    void apply(const std::string& lkey, const conf::Flag& conf, Options& options)
    {
        auto& value = options.options.emplace(lkey, false).first->second;
        options.app->add_flag(get_name(lkey, conf), get<bool>(value), conf.msg.value_or(""));
    }

    void apply(const std::string& lkey, const conf::Number& conf, Options& options)
    {
        using type = std::optional<std::int64_t>;
        auto& value = conf.fallback.has_value()
            ? options.options.emplace(lkey, conf.fallback.value()).first->second
            : options.options.emplace(lkey, std::monostate{}).first->second;

        std::ostringstream oss;
        if (conf.implicit.has_value())
        {
            oss << "[=value(=" << conf.implicit.value() << ")]";
        }
        if (conf.fallback.has_value())
        {
            if (conf.implicit.has_value())
            {
                oss << ' ';
            }
            oss << "(=" << conf.fallback.value() << ')';
        }

        if (conf.implicit.has_value())
        {
            options.app
                ->add_option_function<type>(
                    get_name(lkey, conf),
                    [&value, ival = conf.implicit.value()](const type& new_value)
                    { value = new_value.has_value() ? new_value.value() : ival; },
                    conf.msg.value_or("")
                )
                ->expected(0, 1)
                ->type_name(oss.str());
        }
        else
        {
            options.app
                ->add_option_function<type::value_type>(
                    get_name(lkey, conf),
                    [&value](const type::value_type& new_value) { value = new_value; },
                    conf.msg.value_or("")
                )
                ->expected(1)
                ->type_name(oss.str());
        }
    }

    void apply(const std::string& lkey, const conf::Param& conf, Options& options)
    {
        using type = std::string;
        auto& value = conf.fallback.has_value()
            ? options.options.emplace(lkey, conf.fallback.value()).first->second
            : options.options.emplace(lkey, std::monostate{}).first->second;

        std::ostringstream oss;
        oss << "text";
        if (conf.fallback.has_value())
        {
            oss << "(=" << conf.fallback.value() << ')';
        }
        options.app->add_option(get_name(lkey, conf), get<type>(value), conf.msg.value_or(""))
            ->type_size(1)
            ->type_name(oss.str());
    }

    void apply(const std::string& lkey, const conf::Params& conf, Options& options)
    {
        using type = std::vector<std::string>;
        auto& value = options.options.emplace(lkey, std::monostate{}).first->second;
        options.app
            ->add_option_function<type>(
                get_name(lkey, conf),
                [&value](const type& new_value) { value = new_value; },
                conf.msg.value_or("")
            )
            ->type_name("text");
    }

    Options parse(
        const std::vector<Node::Opt>& opts,
        const std::vector<SubNode>& subs,
        int argc,
        const char* const* argv
    )
    {
        auto retval = Options{
            std::make_unique<CLI::App>(),
            std::unordered_map<
                std::string,
                std::variant<
                    std::monostate,
                    bool,
                    std::int64_t,
                    std::string,
                    std::vector<std::string>>>{},
            std::vector<std::tuple<std::string, std::string>>{}
        };
        retval.app->set_help_flag();     // removes -h/--help
        retval.app->set_help_all_flag(); // removes --help-all too (optional)
        retval.app->get_formatter()->column_width(50);

        for (const auto& o : opts)
        {
            std::visit([&o, &retval](const auto& v) { apply(o.lkey, v, retval); }, o.option);
        }

        for (const auto& node : subs)
        {
            retval.app->add_subcommand(node.key, node.description);
            retval.sub.emplace_back(node.key, node.description);
        }

        retval.app->parse(argc, argv);

        for (const auto& node : subs)
        {
            retval.sub.emplace_back(node.key, node.description);
        }

        return retval;
    }

    void help(const Options& options, std::ostream& os)
    {
        os << options.app->help();
    }

    bool has_value(const Options& options, const std::string& k) noexcept
    {
        if (const auto it = options.options.find(k); it != options.options.end())
        {
            return std::visit(
                [&]<typename variant_t>(const variant_t& /* value */) -> bool
                { return !std::is_same_v<variant_t, std::monostate>; },
                it->second
            );
        }
        return false;
    }

    namespace impl
    {
        template <typename T>
        auto access(const Options& options, const std::string& k)
        {
            if (const auto it = options.options.find(k); it != options.options.end())
            {
                return std::get<T>(std::visit(
                    [&, k]<typename variant_t>(const variant_t& value) -> std::variant<
                                                                           std::monostate,
                                                                           bool,
                                                                           std::int64_t,
                                                                           std::string,
                                                                           std::vector<std::string>>
                    {
                        if constexpr (std::is_same_v<variant_t, T>)
                        {
                            return value;
                        }
                        else
                        {
                            throw std::out_of_range(k); // NOLINT
                        }
                    },
                    it->second
                ));
            }
            throw std::out_of_range("[nil][cli]: option \"" + k + "\" is unknown");
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
