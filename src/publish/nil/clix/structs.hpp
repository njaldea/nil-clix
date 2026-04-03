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
