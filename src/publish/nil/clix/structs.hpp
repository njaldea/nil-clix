// Copyright (c) 2026, Neil Aldea <njaldea@gmail.com>
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <memory>

namespace nil::clix
{
    struct Node;

    struct N
    {
    public:
        explicit N(Node* node, void (*cleanup)(Node*))
            : ptr(node, cleanup)
        {
        }

        ~N() noexcept = default;

        N(const N&) = delete;
        N& operator=(const N&) = delete;

        N(N&&) = default;
        N& operator=(N&&) = default;

        operator Node&() const // NOLINT
        {
            return *ptr;
        }

    private:
        std::unique_ptr<Node, void (*)(Node*)> ptr;
    };

    N make_node();
}
