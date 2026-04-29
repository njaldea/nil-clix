#include <nil/clix.h>

#include <stdio.h>

static void write_stdout(const char* data, uint64_t size, void* context)
{
    (void)context;
    fwrite(data, sizeof(char), (size_t)size, stdout);
    fwrite("\n", sizeof(char), 1, stdout);
}

// ===== flags subcommand =====
static int run_flags(const nil_clix_options* options, void* context)
{
    (void)context;

    if (nil_clix_options_flag(*options, "help") != 0)
    {
        nil_clix_write_info writer = {.exec = write_stdout, .context = NULL};
        nil_clix_options_help(*options, writer);
        return 0;
    }

    printf("Flag Node is executing\n");
    if (nil_clix_options_has_value(*options, "spawn") != 0)
    {
        printf("spawn: %d\n", nil_clix_options_flag(*options, "spawn"));
    }
    return 0;
}

static void configure_flags(nil_clix_node* node, void* context)
{
    (void)context;

    nil_clix_flag_info help_info = {.skey = "h", .msg = "show this help"};
    nil_clix_node_flag(*node, "help", help_info);

    nil_clix_flag_info spawn_info = {.skey = "s", .msg = "spawn"};
    nil_clix_node_flag(*node, "spawn", spawn_info);

    nil_clix_exec_info exec_info = {.exec = run_flags, .context = NULL, .cleanup = NULL};
    nil_clix_node_use(*node, exec_info);
}

// ===== numbers subcommand =====
static int run_numbers(const nil_clix_options* options, void* context)
{
    (void)context;

    if (nil_clix_options_flag(*options, "help") != 0)
    {
        nil_clix_write_info writer = {.exec = write_stdout, .context = NULL};
        nil_clix_options_help(*options, writer);
        return 0;
    }

    printf("Numbers Node is executing\n");
    if (nil_clix_options_has_value(*options, "thread") != 0)
    {
        printf("thread: %ld\n", nil_clix_options_number(*options, "thread"));
    }
    if (nil_clix_options_has_value(*options, "job") != 0)
    {
        printf("job: %ld\n", nil_clix_options_number(*options, "job"));
    }
    return 0;
}

static void configure_numbers(nil_clix_node* node, void* context)
{
    (void)context;

    nil_clix_flag_info help_info = {.skey = "h", .msg = "show this help"};
    nil_clix_node_flag(*node, "help", help_info);

    nil_clix_number_info thread_info = {.skey = "t", .msg = "number of threads", .fallback = NULL, .implicit = NULL};
    nil_clix_node_number(*node, "thread", thread_info);

    static const int64_t job_fallback = 1;
    static const int64_t job_implicit = 0;
    nil_clix_number_info job_info = {.skey = "j", .msg = "number of jobs", .fallback = &job_fallback, .implicit = &job_implicit};
    nil_clix_node_number(*node, "job", job_info);

    nil_clix_exec_info exec_info = {.exec = run_numbers, .context = NULL, .cleanup = NULL};
    nil_clix_node_use(*node, exec_info);
}

// ===== params subcommand =====
static int run_params(const nil_clix_options* options, void* context)
{
    (void)context;

    if (nil_clix_options_flag(*options, "help") != 0)
    {
        nil_clix_write_info writer = {.exec = write_stdout, .context = NULL};
        nil_clix_options_help(*options, writer);
        return 0;
    }

    printf("Params Node is executing\n");
    if (nil_clix_options_has_value(*options, "param") != 0)
    {
        printf("param: %s\n", nil_clix_options_param(*options, "param"));
    }
    if (nil_clix_options_has_value(*options, "mparam") != 0)
    {
        nil_clix_strings mparams = nil_clix_options_params(*options, "mparam");
        for (uint64_t i = 0; i < mparams.size; ++i)
        {
            printf("mparam: %s\n", mparams.data[i]);
        }
    }
    return 0;
}

static void configure_params(nil_clix_node* node, void* context)
{
    (void)context;

    nil_clix_flag_info help_info = {.skey = "h", .msg = "show this help"};
    nil_clix_node_flag(*node, "help", help_info);

    nil_clix_param_info param_info = {.skey = "p", .msg = "default param", .fallback = NULL};
    nil_clix_node_param(*node, "param", param_info);

    nil_clix_params_info mparam_info = {.skey = "m", .msg = "multiple params"};
    nil_clix_node_params(*node, "mparam", mparam_info);

    nil_clix_exec_info exec_info = {.exec = run_params, .context = NULL, .cleanup = NULL};
    nil_clix_node_use(*node, exec_info);
}

// ===== root execution =====
static int run_root(const nil_clix_options* options, void* context)
{
    (void)context;

    if (nil_clix_options_flag(*options, "help") != 0)
    {
        nil_clix_write_info writer = {.exec = write_stdout, .context = NULL};
        nil_clix_options_help(*options, writer);
        return 0;
    }

    printf("Root Node is executing\n");
    return 0;
}

// ===== main =====
int main(int argc, const char* const* argv)
{
    nil_clix_node root = nil_clix_node_create();

    // Configure root
    nil_clix_flag_info root_help = {.skey = "h", .msg = "show this help"};
    nil_clix_node_flag(root, "help", root_help);

    // Configure "flags" subcommand
    nil_clix_sub_info flags_sub = {.exec = configure_flags, .context = NULL, .cleanup = NULL};
    nil_clix_node_sub(root, "flags", "command for flags", flags_sub);

    // Configure "numbers" subcommand
    nil_clix_sub_info numbers_sub = {.exec = configure_numbers, .context = NULL, .cleanup = NULL};
    nil_clix_node_sub(root, "numbers", "command for numbers", numbers_sub);

    // Configure "params" subcommand
    nil_clix_sub_info params_sub = {.exec = configure_params, .context = NULL, .cleanup = NULL};
    nil_clix_node_sub(root, "params", "command for params", params_sub);

    // Set root execution
    nil_clix_exec_info root_exec = {.exec = run_root, .context = NULL, .cleanup = NULL};
    nil_clix_node_use(root, root_exec);

    // Run and cleanup
    const int exit_code = nil_clix_node_run(root, argc -1 , argv + 1);
    nil_clix_node_destroy(root);
    return exit_code;
}
