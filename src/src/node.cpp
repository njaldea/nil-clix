#include "structs.hpp"

#include <stdexcept>
#include <string_view>
#include <algorithm>
#include <memory>
#include <utility>

namespace nil::clix
{
    const SubNode* find(const Node& node, std::string_view name)
    {
        auto result = std::find_if(
            std::begin(node.sub),
            std::end(node.sub),
            [&](const auto& subnode) { return subnode.key == name; }
        );

        if (result != std::end(node.sub))
        {
            return std::addressof(*result);
        }
        return nullptr;
    }

    void use(Node& node, std::function<int(const nil::clix::Options&)> new_exec)
    {
        node.exec = std::move(new_exec);
    }

    void flag(Node& node, std::string lkey, conf::Flag options)
    {
        node.opts.emplace_back(Node::Opt(std::move(lkey), std::move(options)));
    }

    void number(Node& node, std::string lkey, conf::Number options)
    {
        node.opts.emplace_back(Node::Opt(std::move(lkey), std::move(options)));
    }

    void param(Node& node, std::string lkey, conf::Param options)
    {
        node.opts.emplace_back(Node::Opt(std::move(lkey), std::move(options)));
    }

    void params(Node& node, std::string lkey, conf::Params options)
    {
        node.opts.emplace_back(Node::Opt(std::move(lkey), std::move(options)));
    }

    void sub(
        Node& node,
        std::string key,
        std::string description,
        std::function<void(Node&)> predicate
    )
    {
        if (find(node, key) != nullptr)
        {
            throw std::invalid_argument("[nil][cli]: option \"" + key + "\" already exists");
        }

        node.sub.emplace_back(std::move(key), std::move(description), std::move(predicate));
    }

    int run(const Node& node, int argc, const char* const* argv)
    {
        if (argc > 1)
        {
            if (const auto* subnode = find(node, argv[1]); subnode != nullptr)
            {
                Node next_node;
                if (subnode->exec)
                {
                    subnode->exec(next_node);
                }
                return run(next_node, argc - 1, std::next(argv));
            }
        }

        if (node.exec)
        {
            const auto options = parse(node.opts, node.sub, argc, argv);
            return node.exec(options);
        }
        return 0;
    }

    N create_node()
    {
        return N{{new Node(), [](Node* node) { std::default_delete<Node>()(node); }}};
    }
} // namespace nil::clix
