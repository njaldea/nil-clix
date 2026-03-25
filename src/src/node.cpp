#include "structs.hpp"

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <utility>

namespace nil::clix
{
    namespace
    {
        std::optional<char> short_key(const conf::Flag& options)
        {
            return options.skey;
        }

        std::optional<char> short_key(const conf::Number& options)
        {
            return options.skey;
        }

        std::optional<char> short_key(const conf::Param& options)
        {
            return options.skey;
        }

        std::optional<char> short_key(const conf::Params& options)
        {
            return options.skey;
        }

        bool has_long_key(const Node& node, std::string_view key)
        {
            return std::any_of(
                std::begin(node.opts),
                std::end(node.opts),
                [&](const Node::Opt& option) { return option.lkey == key; }
            );
        }

        bool has_short_key(const Node& node, char key)
        {
            return std::any_of(
                std::begin(node.opts),
                std::end(node.opts),
                [&](const Node::Opt& option)
                {
                    return std::visit(
                        [&](const auto& value)
                        {
                            const auto skey = short_key(value);
                            return skey.has_value() && skey.value() == key;
                        },
                        option.option
                    );
                }
            );
        }

        template <typename T>
        void validate_option(const Node& node, std::string_view lkey, const T& options)
        {
            if (has_long_key(node, lkey))
            {
                throw std::invalid_argument(
                    "[nil][cli]: option \"" + std::string(lkey) + "\" already exists"
                );
            }

            if (const auto skey = short_key(options);
                skey.has_value() && has_short_key(node, skey.value()))
            {
                throw std::invalid_argument(
                    "[nil][cli]: short option \"-" + std::string(1, skey.value())
                    + "\" already exists"
                );
            }
        }
    }

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
        validate_option(node, lkey, options);
        node.opts.emplace_back(Node::Opt(std::move(lkey), std::move(options)));
    }

    void number(Node& node, std::string lkey, conf::Number options)
    {
        validate_option(node, lkey, options);
        node.opts.emplace_back(Node::Opt(std::move(lkey), std::move(options)));
    }

    void param(Node& node, std::string lkey, conf::Param options)
    {
        validate_option(node, lkey, options);
        node.opts.emplace_back(Node::Opt(std::move(lkey), std::move(options)));
    }

    void params(Node& node, std::string lkey, conf::Params options)
    {
        validate_option(node, lkey, options);
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
            throw std::invalid_argument("[nil][cli]: subcommand \"" + key + "\" already exists");
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
