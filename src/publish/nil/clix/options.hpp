// Copyright (c) 2026, Neil Aldea <njaldea@gmail.com>
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <cstdint>
#include <ostream>
#include <string>
#include <vector>

namespace nil::clix
{
    struct Options;

    void help(const Options& options, std::ostream& os);

    bool has_value(const Options& options, const std::string& lkey) noexcept;

    bool flag(const Options& options, const std::string& lkey);
    std::int64_t number(const Options& options, const std::string& lkey);
    std::string param(const Options& options, const std::string& lkey);
    std::vector<std::string> params(const Options& options, const std::string& lkey);
}
