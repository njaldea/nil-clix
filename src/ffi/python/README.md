# nil-clix Python Binding

Python binding for **nil/clix** – an Opinionated Argument Parser

## Links

- **Repository:** https://github.com/njaldea/nil-clix
- **C API Documentation:** https://github.com/njaldea/nil-clix/blob/master/docs/c-api.md

## Installation

```bash
pip install nil-clix
```

## Usage

```python
from nil_clix import create_node

node = create_node()

# Add flags
node.flag("verbose", "v", "Enable verbose output")

# Add parameters
node.param("config", "c", "Path to config file", fallback="config.yml")

# Add numbers
node.number("threads", "t", "Number of threads", fallback=4)

# Add handler
def handler(options):
    if options.flag("verbose"):
        print("Verbose mode enabled")
    config = options.param("config")
    threads = options.number("threads")
    print(f"Config: {config}, Threads: {threads}")
    return 0

node.use(handler)

# Run
exit_code = node.run(["-v", "-c", "my.yml", "-t", "8"])
```

## Documentation

For detailed API documentation and more examples, visit:
- [C++ Documentation](https://github.com/njaldea/nil-clix)
- [C API Guide](https://github.com/njaldea/nil-clix/blob/master/docs/c-api.md)

## License

CC BY-NC-ND 4.0

## Support

For issues, questions, or contributions, visit the [GitHub repository](https://github.com/njaldea/nil-clix).
