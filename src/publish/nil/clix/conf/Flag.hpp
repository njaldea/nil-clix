// Copyright (c) 2026, Neil Aldea <njaldea@gmail.com>
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <optional>
#include <string>

namespace nil::clix::conf
{
    struct Flag final
    {
        /**
         * @brief Short Key - Alias
         */
        std::optional<char> skey = std::nullopt;

        /**
         * @brief Message to be used during help
         */
        std::optional<std::string> msg = std::nullopt;
    };
} // namespace nil::clix::conf
