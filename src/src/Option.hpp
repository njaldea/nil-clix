#pragma once

#include <nil/clix/IOption.hpp>
#include <nil/clix/conf/Flag.hpp>
#include <nil/clix/conf/Number.hpp>
#include <nil/clix/conf/Param.hpp>
#include <nil/clix/conf/Params.hpp>

#include <boost/program_options/options_description.hpp>

namespace boost::program_options
{
    class options_description_easy_init;
}

namespace nil::clix
{
    struct IOption::Impl final
    {
        boost::program_options::options_description_easy_init* ex;
    };

    class FlagOption final: public IOption
    {
    public:
        FlagOption(std::string init_lkey, conf::Flag init_options);
        void apply(const IOption::Impl& impl) const override;

    private:
        std::string lkey;
        conf::Flag options;
    };

    class NumberOption final: public IOption
    {
    public:
        NumberOption(std::string init_lkey, conf::Number init_options);
        void apply(const IOption::Impl& impl) const override;

    private:
        std::string lkey;
        conf::Number options;
    };

    class ParamOption final: public IOption
    {
    public:
        ParamOption(std::string init_lkey, conf::Param init_options);
        void apply(const IOption::Impl& impl) const override;

    private:
        std::string lkey;
        conf::Param options;
    };

    class ParamsOption final: public IOption
    {
    public:
        ParamsOption(std::string init_lkey, conf::Params init_options);
        void apply(const IOption::Impl& impl) const override;

    private:
        std::string lkey;
        conf::Params options;
    };

} // namespace nil::clix
