#pragma once

#include "../Command.hpp"

#include <ostream>

namespace nil::clix::nodes
{
    class Options;

    class Help final: public nil::clix::Command
    {
    public:
        explicit Help(std::ostream& init_os);
        ~Help() noexcept override = default;

        Help(Help&&) noexcept = delete;
        Help& operator=(Help&&) noexcept = delete;

        Help(const Help&) = delete;
        Help& operator=(const Help&) = delete;

        OptionInfo options() const override;

        int run(const nil::clix::Options& options) const override;

    private:
        std::ostream& os;
    };
} // namespace nil::clix::nodes
