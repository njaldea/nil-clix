#pragma once

#include <nil/clix/Builder.hpp>
#include <nil/clix/IOption.hpp>

namespace boost::program_options {
class options_description_easy_init;
}

namespace nil::clix {
struct IOption::Impl final {
  boost::program_options::options_description_easy_init *ex;
};
} // namespace nil::clix
