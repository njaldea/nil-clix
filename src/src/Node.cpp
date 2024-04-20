#include <nil/clix/IOption.hpp>
#include <nil/clix/Node.hpp>
#include <nil/clix/Options.hpp>

#include "Option.hpp"
#include "OptionsImpl.hpp"

#include <stdexcept>

namespace nil::clix
{
    Node::Node() = default;
    Node::~Node() noexcept = default;
    Node::Node(Node&&) noexcept = default;
    Node& Node::operator=(Node&&) noexcept = default;

    void Node::runner(std::function<int(const nil::clix::Options&)> new_exec)
    {
        exec = std::move(new_exec);
    }

    void Node::flag(std::string lkey, conf::Flag options)
    {
        opts.emplace_back(std::make_unique<FlagOption>(std::move(lkey), std::move(options)));
    }

    void Node::number(std::string lkey, conf::Number options)
    {
        opts.emplace_back(std::make_unique<NumberOption>(std::move(lkey), std::move(options)));
    }

    void Node::param(std::string lkey, conf::Param options)
    {
        opts.emplace_back(std::make_unique<ParamOption>(std::move(lkey), std::move(options)));
    }

    void Node::params(std::string lkey, conf::Params options)
    {
        opts.emplace_back(std::make_unique<ParamsOption>(std::move(lkey), std::move(options)));
    }

    void Node::add(std::string key, std::string description, std::function<void(Node&)> predicate)
    {
        if (find(key) != nullptr)
        {
            throw std::invalid_argument("[nil][cli]: option \"" + key + "\" already exists");
        }

        sub.emplace_back(SubNode(std::move(key), std::move(description), std::move(predicate)));
    }

    int Node::run(int argc, const char* const* argv) const
    {
        if (argc > 1)
        {
            if (const auto* subnode = find(argv[1]); subnode != nullptr)
            {
                Node node;
                if (subnode->exec)
                {
                    subnode->exec(node);
                }
                return node.run(argc - 1, std::next(argv));
            }
        }

        if (exec)
        {
            const Options options(std::make_unique<Options::Impl>(opts, sub, argc, argv));
            return exec(options);
        }
        return 0;
    }

    const SubNode* Node::find(std::string_view name) const
    {
        auto result = std::find_if(
            std::begin(sub),
            std::end(sub),
            [&](const auto& subnode) { return subnode.key == name; }
        );

        if (result != std::end(sub))
        {
            return std::addressof(*result);
        }
        return nullptr;
    }
} // namespace nil::clix
