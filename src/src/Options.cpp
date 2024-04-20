#include "OptionsImpl.hpp"

#include <ostream>

namespace nil::clix
{
    Options::Options(std::unique_ptr<Options::Impl> init_impl)
        : impl(std::move(init_impl))
    {
    }

    Options::~Options() noexcept = default;

    void Options::help(std::ostream& os) const
    {
        impl->help(os);
    }

    bool Options::flag(const std::string& lkey) const
    {
        return impl->access<bool>(lkey);
    }

    std::int64_t Options::number(const std::string& lkey) const
    {
        return impl->access<std::int64_t>(lkey);
    }

    std::string Options::param(const std::string& lkey) const
    {
        return impl->access<std::string>(lkey);
    }

    std::vector<std::string> Options::params(const std::string& lkey) const
    {
        return impl->access<std::vector<std::string>>(lkey);
    }
} // namespace nil::clix
