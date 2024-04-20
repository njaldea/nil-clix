#pragma once

#include <memory>
#include <string>
#include <vector>

namespace nil::clix
{
    class Node;
    struct SubNode;

    class Options final
    {
    public:
        Options(const Options&) = delete;
        Options& operator=(const Options&) = delete;
        Options(Options&&) = delete;
        Options& operator=(Options&&) = delete;

        void help(std::ostream& os) const;
        bool flag(const std::string& lkey) const;
        std::int64_t number(const std::string& lkey) const;
        std::string param(const std::string& lkey) const;
        std::vector<std::string> params(const std::string& lkey) const;

    private:
        friend Node;
        struct Impl;
        std::unique_ptr<Impl> impl;

        explicit Options(std::unique_ptr<Impl> init_impl);
        ~Options() noexcept;
    };
} // namespace nil::clix
