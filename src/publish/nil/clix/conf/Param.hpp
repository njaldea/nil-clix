#pragma once

#include <optional>
#include <string>

namespace nil::clix::conf
{
    struct Param final
    {
        /**
         * @brief Short Key - Alias
         */
        std::optional<char> skey = std::nullopt;

        /**
         * @brief Message to be used during help
         */
        std::optional<std::string> msg = std::nullopt;

        /**
         * @brief Value used when option is not provided
         */
        std::optional<std::string> fallback = std::nullopt;
    };
} // namespace nil::clix::conf
