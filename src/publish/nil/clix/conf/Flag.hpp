#pragma once

#include <optional>
#include <string>

namespace nil::clix::conf
{
    struct Flag final
    {
        /**
         * @brief Short Key - Alias
         */
        std::optional<char> skey = std::nullopt;

        /**
         * @brief Message to be used during help
         */
        std::optional<std::string> msg = std::nullopt;
    };
} // namespace nil::clix::conf
