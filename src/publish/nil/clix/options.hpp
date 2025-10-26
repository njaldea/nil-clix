#pragma once

#include "structs.hpp"

#include <cstdint>
#include <ostream>
#include <string>
#include <vector>

namespace nil::clix
{
    void help(const Options& options, std::ostream& os);

    bool has_value(const Options& options, const std::string& lkey) noexcept;

    bool flag(const Options& options, const std::string& lkey);
    std::int64_t number(const Options& options, const std::string& lkey);
    std::string param(const Options& options, const std::string& lkey);
    std::vector<std::string> params(const Options& options, const std::string& lkey);
}
