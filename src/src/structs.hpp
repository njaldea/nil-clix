// Copyright (c) 2026, Neil Aldea <njaldea@gmail.com>
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <nil/clix/node.hpp>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/variables_map.hpp>

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
        boost::program_options::options_description desc;
        boost::program_options::variables_map vm;
        std::vector<std::tuple<std::string, std::string>> sub;
    };

    Options parse(
        const std::vector<Node::Opt>& opts,
        const std::vector<SubNode>& subs,
        int argc,
        const char* const* argv
    );
}
