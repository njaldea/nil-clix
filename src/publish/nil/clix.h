#pragma once

// NOLINTNEXTLINE(hicpp-deprecated-headers,modernize-deprecated-headers)
#include <stddef.h>

// NOLINTNEXTLINE(hicpp-deprecated-headers,modernize-deprecated-headers)
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif
    // Unless explicitly documented otherwise, API functions require valid non-null handles.

    // NOLINTNEXTLINE(modernize-use-using)
    typedef struct nil_clix_node
    {
        void* handle;
    } nil_clix_node;

    // NOLINTNEXTLINE(modernize-use-using)
    typedef struct nil_clix_options
    {
        void* handle;
    } nil_clix_options;

    nil_clix_node nil_clix_node_create(void);
    void nil_clix_node_destroy(nil_clix_node node);
    int nil_clix_node_run(nil_clix_node node, int argc, const char* const* argv);

    // NOLINTNEXTLINE(modernize-use-using)
    typedef struct nil_clix_flag_info
    {
        const char* skey; // NULL if not present
        const char* msg;
    } nil_clix_flag_info;

    // NOLINTNEXTLINE(modernize-use-using)
    typedef struct nil_clix_number_info
    {
        const char* skey; // NULL if not present
        const char* msg;
        const int64_t* fallback; // NULL if not present
        const int64_t* implicit; // NULL if not present
    } nil_clix_number_info;

    // NOLINTNEXTLINE(modernize-use-using)
    typedef struct nil_clix_param_info
    {
        const char* skey; // NULL if not present
        const char* msg;
        const char* fallback; // NULL if not present
    } nil_clix_param_info;

    // NOLINTNEXTLINE(modernize-use-using)
    typedef struct nil_clix_params_info
    {
        const char* skey; // NULL if not present
        const char* msg;
    } nil_clix_params_info;

    void nil_clix_node_flag(nil_clix_node node, const char* lkey, nil_clix_flag_info info);
    void nil_clix_node_number(nil_clix_node node, const char* lkey, nil_clix_number_info info);
    void nil_clix_node_param(nil_clix_node node, const char* lkey, nil_clix_param_info info);
    void nil_clix_node_params(nil_clix_node node, const char* lkey, nil_clix_params_info info);

    // NOLINTNEXTLINE(modernize-use-using)
    typedef struct nil_clix_exec_info
    {
        int (*exec)(const nil_clix_options* options, void*);
        void* context;
        void (*cleanup)(void*);
    } nil_clix_exec_info;

    // NOLINTNEXTLINE(modernize-use-using)
    typedef struct nil_clix_sub_info
    {
        void (*exec)(nil_clix_node* node, void*);
        void* context;
        void (*cleanup)(void*);
    } nil_clix_sub_info;

    void nil_clix_node_use(nil_clix_node node, nil_clix_exec_info callback);

    // `callback` receives a non-owning node view valid only during the setup callback.
    void nil_clix_node_sub(
        nil_clix_node node,
        const char* key,
        const char* description,
        nil_clix_sub_info callback
    );

    // NOLINTNEXTLINE(modernize-use-using)
    typedef struct nil_clix_write_info
    {
        void (*exec)(const char* data, uint64_t size, void*);
        void* context;
    } nil_clix_write_info;

    // NOLINTNEXTLINE(modernize-use-using)
    typedef struct nil_clix_strings
    {
        uint64_t size;
        const char* const* data;
    } nil_clix_strings;

    // Returned views from nil_clix_options_param / nil_clix_options_params are valid only while
    // the current exec callback is active.
    void nil_clix_options_help(nil_clix_options options, nil_clix_write_info writer);
    int nil_clix_options_has_value(nil_clix_options options, const char* lkey);
    int nil_clix_options_flag(nil_clix_options options, const char* lkey);
    int64_t nil_clix_options_number(nil_clix_options options, const char* lkey);
    const char* nil_clix_options_param(nil_clix_options options, const char* lkey);
    nil_clix_strings nil_clix_options_params(nil_clix_options options, const char* lkey);

#ifdef __cplusplus
}

#endif
