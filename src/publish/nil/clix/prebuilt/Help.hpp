// Copyright (c) 2026, Neil Aldea <njaldea@gmail.com>
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

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
