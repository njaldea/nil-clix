#pragma once

#include "../node.hpp"
#include "../options.hpp"

#include <functional>
#include <ostream>

namespace nil::clix::prebuilt
{
    class Help
    {
    public:
        explicit Help(std::ostream* init_os);

        Help(const Help&) noexcept = default;
        Help& operator=(const Help&) noexcept = default;
        ~Help() noexcept = default;

        Help(Help&&) noexcept = delete;
        Help& operator=(Help&&) noexcept = delete;

        operator std::function<void(Node&)>() const;         // NOLINT
        operator std::function<int(const Options&)>() const; // NOLINT

    private:
        std::ostream* os;
    };
}
