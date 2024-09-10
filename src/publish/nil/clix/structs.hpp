#pragma once

#include <memory>

namespace nil::clix
{
    struct Node;
    struct Options;

    struct N
    {
        std::unique_ptr<Node, void (*)(Node*)> ptr;

        operator Node&() const // NOLINT
        {
            return *ptr;
        }
    };

    using node_ptr_t = std::unique_ptr<Node, void (*)(Node*)>;
}
