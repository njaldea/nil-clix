#include "Option.hpp"

namespace nil::clix
{
    FlagOption::FlagOption(std::string init_lkey, conf::Flag init_options)
        : lkey(std::move(init_lkey))
        , options(std::move(init_options))
    {
    }

    void FlagOption::apply(const IOption::Impl& impl) const
    {
        const auto opt = options.skey ? (lkey + ',' + *options.skey) : lkey;
        auto* value = boost::program_options::value<bool>();
        value->zero_tokens();
        value->default_value(false);
        (*impl.ex)(opt.c_str(), value, options.msg.value_or("").c_str());
    }

    NumberOption::NumberOption(std::string init_lkey, conf::Number init_options)
        : lkey(std::move(init_lkey))
        , options(std::move(init_options))
    {
    }

    void NumberOption::apply(const IOption::Impl& impl) const
    {
        const auto opt = options.skey ? (lkey + ',' + *options.skey) : lkey;
        auto* value = boost::program_options::value<std::int64_t>();
        value->value_name("value");
        value->implicit_value(options.implicit, std::to_string(options.implicit));
        value->default_value(options.fallback, std::to_string(options.fallback));
        (*impl.ex)(opt.c_str(), value, options.msg.value_or("").c_str());
    }

    ParamOption::ParamOption(std::string init_lkey, conf::Param init_options)
        : lkey(std::move(init_lkey))
        , options(std::move(init_options))
    {
    }

    void ParamOption::apply(const IOption::Impl& impl) const
    {
        const auto opt = options.skey ? (lkey + ',' + *options.skey) : lkey;
        auto* value = boost::program_options::value<std::string>();
        value->value_name("text");
        if (options.fallback.has_value())
        {
            value->default_value(options.fallback.value(), "\"" + options.fallback.value() + "\"");
        }
        else
        {
            value->required();
        }
        (*impl.ex)(opt.c_str(), value, options.msg.value_or("").c_str());
    }

    ParamsOption::ParamsOption(std::string init_lkey, conf::Params init_options)
        : lkey(std::move(init_lkey))
        , options(std::move(init_options))
    {
    }

    void ParamsOption::apply(const IOption::Impl& impl) const
    {
        const auto opt = options.skey ? (lkey + ',' + *options.skey) : lkey;
        auto* value = boost::program_options::value<std::vector<std::string>>();
        value->value_name("text");
        value->multitoken();
        value->default_value({}, "");
        (*impl.ex)(opt.c_str(), value, options.msg.value_or("").c_str());
    }
} // namespace nil::clix
