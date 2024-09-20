#pragma once

#include <optional>
#include <string>
#include <cstdint>

namespace nil::clix::conf
{
    struct Number final
    {
        /**
         * @brief Short Key - Alias
         */
        std::optional<char> skey;

        /**
         * @brief Message to be used during help
         */
        std::optional<std::string> msg;

        /**
         * @brief Value used when option is not provided
         */
        std::int64_t fallback = 0;

        /**
         * @brief Value used when there is no value provided to the option
         */
        std::int64_t implicit = 1;
    };
} // namespace nil::clix::conf
