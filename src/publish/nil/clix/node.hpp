#pragma once

#include "conf/Flag.hpp"
#include "conf/Number.hpp"
#include "conf/Param.hpp"
#include "conf/Params.hpp"
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

    N create_node();
} // namespace nil::clix
