#pragma once

#include <nil/clix/IOption.hpp>
#include <nil/clix/Options.hpp>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/variables_map.hpp>

#include <vector>

namespace nil::clix
{
    struct Options::Impl final
    {
        Impl(
            const std::vector<std::unique_ptr<IOption>>& info,
            const std::vector<SubNode>& subnodes,
            int argc,
            const char* const* argv
        );
        ~Impl() noexcept = default;

        Impl(Impl&&) noexcept = delete;
        Impl& operator=(Impl&&) noexcept = delete;
        Impl(const Impl&) = delete;
        Impl& operator=(const Impl&) = delete;

        boost::program_options::options_description desc;
        boost::program_options::variables_map vm;
        std::vector<std::tuple<std::string, std::string>> sub;

        template <typename T>
        auto access(const std::string& k)
        {
            try
            {
                return vm[k].as<T>();
            }
            catch (const std::exception&)
            {
                throw std::out_of_range("[nil][cli]: option \"" + k + "\" is unknown");
            }
        }

        void help(std::ostream& os) const;
    };
}
