# nil/clix C API

This document covers the C API exposed by nil/clix.

Header:
- nil/clix.h

Build:
- Enable `ENABLE_C_API` to build `clix-c-api`.

## Handle Model

Owning handles (must be destroyed):
- nil_clix_node

View handles (non-owning):
- nil_clix_options

Rules:
- `nil_clix_node_sub` setup callbacks receive a non-owning child node view valid only during setup.
- Destroy only nodes created with `nil_clix_node_create`.
- `nil_clix_options` is a non-owning callback view and must not be stored.

Precondition rule:
- Unless otherwise documented, API functions expect valid non-null handles and callback pointers.

## Creation And Execution

Create and destroy the command tree:
- nil_clix_node_create
- nil_clix_node_destroy

Execute the command tree:
- nil_clix_node_run

## Node Configuration

Register options on a node:
- nil_clix_node_flag
- nil_clix_node_number
- nil_clix_node_param
- nil_clix_node_params

Register execution logic:
- nil_clix_node_use

Register subcommands:
- nil_clix_node_sub

Input structure notes (`nil_clix_flag_info`, `nil_clix_number_info`, `nil_clix_param_info`,
`nil_clix_params_info`):
- Pointer fields are non-owning inputs.
- Passing `NULL` means the value is not provided.
- `skey` expects a pointer to a character; only the first character is used.
- `fallback` / `implicit` pointers are read during the API call and copied into internal state.
- Callers do not need to keep these pointers alive after the configuration call returns.

## Options Access

Inside `nil_clix_node_use` callbacks, access parsed values with:
- nil_clix_options_help
- nil_clix_options_has_value
- nil_clix_options_flag
- nil_clix_options_number
- nil_clix_options_param
- nil_clix_options_params

## Lifetime Notes

`nil_clix_options`:
- Valid only during the active exec callback invocation.
- Must not be used after the exec callback returns.

`nil_clix_options_help`:
- The `data` pointer passed to `writer.exec` is valid only for the duration of that callback call.
- Writers that need to retain data must copy it.

`nil_clix_options_param`:
- Returned pointer is callback-scoped.
- Repeated calls for the same key in the same callback return a stable pointer.

`nil_clix_options_params`:
- Returned `nil_clix_strings.data` and element pointers are callback-scoped.
- Repeated calls for the same key in the same callback return stable pointers.
- Different keys return independent cached storage.

`nil_clix_node` child setup handles:
- Valid only during the `nil_clix_node_sub` setup callback.
- Must not be stored after the callback returns.

## Minimal Usage Pattern

1. Create a node handle with `nil_clix_node_create`.
2. Register options, subcommands, and an exec callback.
3. Call `nil_clix_node_run`.
4. Destroy the node with `nil_clix_node_destroy`.
