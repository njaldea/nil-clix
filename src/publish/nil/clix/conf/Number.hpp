#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace nil::clix::conf
{
    struct Number final
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
        std::optional<std::int64_t> fallback = std::nullopt;

        /**
         * @brief Value used when there is no value provided to the option
         */
        std::optional<std::int64_t> implicit = std::nullopt;
    };
} // namespace nil::clix::conf
