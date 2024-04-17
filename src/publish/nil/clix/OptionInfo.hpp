#pragma once

#include "IOption.hpp"

#include <memory>
#include <vector>

namespace nil::clix {
using OptionInfo = std::vector<std::unique_ptr<IOption>>;
}
