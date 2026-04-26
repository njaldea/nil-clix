from __future__ import annotations

import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[1]
PY_FFI_DIR = REPO_ROOT / "src" / "ffi" / "python"

sys.path.insert(0, str(PY_FFI_DIR))

import nil_clix

root = nil_clix.create_node()
root.flag("help", "h", "show this help")

def flags_setup(node: nil_clix.Node) -> None:
    node.flag("help", "h", "show this help")
    node.flag("spawn", "s", "spawn")
    
    def flags_exec(options: nil_clix.Options) -> int:
        if options.flag("help"):
            print(options.help())
            return 0
        print("Flag Node is executing")
        if options.has_value("spawn"):
            print(f"spawn: {options.flag('spawn')}")
        return 0
    
    node.use(flags_exec)

def numbers_setup(node: nil_clix.Node) -> None:
    node.flag("help", "h", "show this help")
    node.number("thread", "t", "number of threads")
    node.number("job", "j", "number of jobs", 1, 0)
    
    def numbers_exec(options: nil_clix.Options) -> int:
        if options.flag("help"):
            print(options.help())
            return 0
        print("Numbers Node is executing")
        if options.has_value("thread"):
            print(f"thread: {options.number('thread')}")
        if options.has_value("job"):
            print(f"job: {options.number('job')}")
        return 0
    
    node.use(numbers_exec)

def params_setup(node: nil_clix.Node) -> None:
    node.flag("help", "h", "show this help")
    node.param("param", "p", "default param")
    node.params("mparam", "m", "multiple params")
    
    def params_exec(options: nil_clix.Options) -> int:
        if options.flag("help"):
            print(options.help())
            return 0
        print("Params Node is executing")
        if options.has_value("param"):
            print(f"param: {options.param('param')}")
        if options.has_value("mparam"):
            for m in options.params("mparam"):
                print(f"mparam: {m}")
        return 0
    
    node.use(params_exec)

root.sub("flags", "command for flags", flags_setup)
root.sub("numbers", "command for numbers", numbers_setup)
root.sub("params", "command for params", params_setup)

def root_exec(options: nil_clix.Options) -> int:
    if options.flag("help"):
        print(options.help())
        return 0
    print("Root Node is executing")
    return 0

root.use(root_exec)

exit_code = root.run(sys.argv[1:])
sys.exit(exit_code)
