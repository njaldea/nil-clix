#pragma once

#include <ostream>

namespace nil::clix
{
    struct Node;
    struct Options;
}

namespace nil::clix::prebuilt
{
    struct Help
    {
    public:
        explicit Help(std::ostream* init_os);

        Help(const Help&) noexcept = default;
        Help& operator=(const Help&) noexcept = default;
        ~Help() noexcept = default;

        Help(Help&&) noexcept = default;
        Help& operator=(Help&&) noexcept = default;

        void operator()(Node&) const;
        int operator()(const Options&) const;

    private:
        std::ostream* os;
    };
}
