// Copyright (c) 2026, Neil Aldea <njaldea@gmail.com>
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "conf/Flag.hpp"
#include "conf/Number.hpp"
#include "conf/Param.hpp"
#include "conf/Params.hpp"
#include "options.hpp"
#include "structs.hpp"

#include <functional>
#include <string>

namespace nil::clix
{
    void flag(Node& node, std::string lkey, conf::Flag options = {});
    void number(Node& node, std::string lkey, conf::Number options = {});
    void param(Node& node, std::string lkey, conf::Param options = {});
    void params(Node& node, std::string lkey, conf::Params options = {});

    void use(Node& node, std::function<int(const nil::clix::Options&)> new_exec);
    void sub(
        Node& node,
        std::string key,
        std::string description,
        std::function<void(Node&)> predicate
    );

    int run(const Node& node, int argc, const char* const* argv);

    Node* create_node();
    void destroy_node(Node* node);
}
