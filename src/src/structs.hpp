#pragma once

#include <nil/clix/node.hpp>

#include <CLI/CLI.hpp>

#include <functional>
#include <string>
#include <variant>

namespace nil::clix
{
    struct SubNode
    {
        std::string key;
        std::string description;
        std::function<void(Node&)> exec;
    };

    struct Node
    {
        struct Opt
        {
            std::string lkey;
            std::variant<conf::Flag, conf::Number, conf::Param, conf::Params> option;
        };

        std::function<int(const nil::clix::Options&)> exec;
        std::vector<SubNode> sub;

        std::vector<Opt> opts;
    };

    struct Options
    {
        std::unique_ptr<CLI::App> app;
        std::unordered_map<
            std::string,
            std::variant<std::monostate, bool, std::int64_t, std::string, std::vector<std::string>>>
            options;
        std::vector<std::tuple<std::string, std::string>> sub;
    };

    Options parse(
        const std::vector<Node::Opt>& opts,
        const std::vector<SubNode>& subs,
        int argc,
        const char* const* argv
    );
}
