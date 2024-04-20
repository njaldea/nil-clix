#pragma once

#include "conf/Flag.hpp"
#include "conf/Number.hpp"
#include "conf/Param.hpp"
#include "conf/Params.hpp"

#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace nil::clix
{
    struct Options;
    struct IOption;
    class Node;

    struct SubNode
    {
        std::string key;
        std::string description;
        void (*exec)(Node&);
    };

    class Node final
    {
    public:
        Node();
        ~Node() noexcept;

        Node(Node&&) noexcept = delete;
        Node& operator=(Node&&) noexcept = delete;
        Node(const Node&) = delete;
        Node& operator=(const Node&) = delete;

        void runner(int (*exec)(const nil::clix::Options&));
        void flag(std::string lkey, conf::Flag options = {});
        void number(std::string lkey, conf::Number options = {});
        void param(std::string lkey, conf::Param options = {});
        void params(std::string lkey, conf::Params options = {});
        void add(std::string key, std::string description, void (*sub_noder)(Node&));
        int run(int argc, const char* const* argv) const;

    private:
        int (*exec)(const nil::clix::Options&) = nullptr;

        std::vector<SubNode> sub;
        std::vector<std::unique_ptr<IOption>> opts;

        const SubNode* find(std::string_view name) const;
    };
} // namespace nil::clix
