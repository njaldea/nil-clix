# Repository rules

Use the configure wrapper for local development:

```sh
./configure/gcc -tsc
```

Flag meanings:

- `-t`: enable tests
- `-s`: enable sandbox (mainly for local testing)
- `-c`: enable C API (only when C API is available for the repository)
