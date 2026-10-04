// Copyright (c) 2026, Neil Aldea <njaldea@gmail.com>
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#include <nil/clix/prebuilt/Help.hpp>

#include <nil/clix/node.hpp>
#include <nil/clix/options.hpp>

namespace nil::clix::prebuilt
{
    Help::Help(std::ostream* init_os)
        : os(init_os)
    {
    }

    void Help::operator()(Node& node) const
    {
        use(node, *this);
    }

    int Help::operator()(const Options& options) const
    {
        if (os != nullptr)
        {
            help(options, *os);
            *os << '\n';
        }
        return 0;
    }
}
