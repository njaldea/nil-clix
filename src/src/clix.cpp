#include <nil/clix.h>

#include <nil/clix.hpp>

#include <nil/xalt/raii.hpp>

#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace
{
    struct ParamsValue
    {
        std::vector<std::string> strings;
        std::vector<const char*> ptrs;
    };

    using OptionValue = std::variant<bool, std::string, std::int64_t, ParamsValue>;

    struct OptionsHandle
    {
        const nil::clix::Options* options;
        std::unordered_map<std::string, OptionValue> cache;
    };

    nil::clix::Node* to_node(nil_clix_node node)
    {
        return static_cast<nil::clix::Node*>(node.handle);
    }

    OptionsHandle* to_options(nil_clix_options options)
    {
        return static_cast<OptionsHandle*>(options.handle);
    }

    std::optional<std::string> to_string(const char* value)
    {
        if (value == nullptr)
        {
            return std::nullopt;
        }

        return std::string(value);
    }

    nil::clix::conf::Flag to_cpp(nil_clix_flag_info info)
    {
        return {
            .skey = info.skey != nullptr ? std::optional<char>(*info.skey) : std::nullopt,
            .msg = to_string(info.msg)
        };
    }

    nil::clix::conf::Number to_cpp(nil_clix_number_info info)
    {
        return {
            .skey = info.skey != nullptr ? std::optional<char>(*info.skey) : std::nullopt,
            .msg = to_string(info.msg),
            .fallback
            = info.fallback != nullptr ? std::optional<std::int64_t>(*info.fallback) : std::nullopt,
            .implicit
            = info.implicit != nullptr ? std::optional<std::int64_t>(*info.implicit) : std::nullopt
        };
    }

    nil::clix::conf::Param to_cpp(nil_clix_param_info info)
    {
        return {
            .skey = info.skey != nullptr ? std::optional<char>(*info.skey) : std::nullopt,
            .msg = to_string(info.msg),
            .fallback = to_string(info.fallback)
        };
    }

    nil::clix::conf::Params to_cpp(nil_clix_params_info info)
    {
        return {
            .skey = info.skey != nullptr ? std::optional<char>(*info.skey) : std::nullopt,
            .msg = to_string(info.msg)
        };
    }
}

extern "C"
{
    nil_clix_node nil_clix_node_create(void)
    {
        return {.handle = nil::clix::create_node()};
    }

    void nil_clix_node_destroy(nil_clix_node node)
    {
        nil::clix::destroy_node(to_node(node));
    }

    int nil_clix_node_run(nil_clix_node node, int argc, const char* const* argv)
    {
        return nil::clix::run(*to_node(node), argc, argv);
    }

    void nil_clix_node_flag(nil_clix_node node, const char* lkey, nil_clix_flag_info info)
    {
        nil::clix::flag(*to_node(node), lkey, to_cpp(info));
    }

    void nil_clix_node_number(nil_clix_node node, const char* lkey, nil_clix_number_info info)
    {
        nil::clix::number(*to_node(node), lkey, to_cpp(info));
    }

    void nil_clix_node_param(nil_clix_node node, const char* lkey, nil_clix_param_info info)
    {
        nil::clix::param(*to_node(node), lkey, to_cpp(info));
    }

    void nil_clix_node_params(nil_clix_node node, const char* lkey, nil_clix_params_info info)
    {
        nil::clix::params(*to_node(node), lkey, to_cpp(info));
    }

    void nil_clix_node_use(nil_clix_node node, nil_clix_exec_info callback)
    {
        auto holder = std::make_shared<nil::xalt::raii<void>>(callback.context, callback.cleanup);
        nil::clix::use(
            *to_node(node),
            [exec = callback.exec, holder](const nil::clix::Options& options)
            {
                auto c_options = OptionsHandle{.options = &options, .cache = {}};
                return exec({.handle = &c_options}, holder->object);
            }
        );
    }

    void nil_clix_node_sub(
        nil_clix_node node,
        const char* key,
        const char* description,
        nil_clix_sub_info callback
    )
    {
        auto holder = std::make_shared<nil::xalt::raii<void>>(callback.context, callback.cleanup);
        nil::clix::sub(
            *to_node(node),
            key,
            description,
            [exec = callback.exec, holder](nil::clix::Node& child)
            { exec({.handle = &child}, holder->object); }
        );
    }

    void nil_clix_options_help(nil_clix_options options, nil_clix_write_info writer)
    {
        auto* handle = to_options(options);
        std::ostringstream oss;
        nil::clix::help(*handle->options, oss);
        std::string help_text = oss.str();
        writer.exec(help_text.c_str(), help_text.size(), writer.context);
    }

    int nil_clix_options_has_value(nil_clix_options options, const char* lkey)
    {
        return nil::clix::has_value(*to_options(options)->options, lkey) ? 1 : 0;
    }

    int nil_clix_options_flag(nil_clix_options options, const char* lkey)
    {
        return nil::clix::flag(*to_options(options)->options, lkey) ? 1 : 0;
    }

    int64_t nil_clix_options_number(nil_clix_options options, const char* lkey)
    {
        return nil::clix::number(*to_options(options)->options, lkey);
    }

    const char* nil_clix_options_param(nil_clix_options options, const char* lkey)
    {
        auto* handle = to_options(options);
        auto it = handle->cache.find(lkey);
        if (it == handle->cache.end())
        {
            std::string value = nil::clix::param(*handle->options, lkey);
            it = handle->cache.emplace(lkey, value).first;
        }
        return std::get<std::string>(it->second).c_str();
    }

    nil_clix_strings nil_clix_options_params(nil_clix_options options, const char* lkey)
    {
        auto* handle = to_options(options);
        auto it = handle->cache.find(lkey);
        if (it == handle->cache.end())
        {
            ParamsValue pv;
            pv.strings = nil::clix::params(*handle->options, lkey);
            pv.ptrs.reserve(pv.strings.size());
            for (const auto& value : pv.strings)
            {
                pv.ptrs.push_back(value.c_str());
            }
            it = handle->cache.emplace(lkey, std::move(pv)).first;
        }

        const auto& params_value = std::get<ParamsValue>(it->second);
        return {
            .size = static_cast<uint64_t>(params_value.ptrs.size()),
            .data = params_value.ptrs.data()
        };
    }
}
