from __future__ import annotations

import ctypes
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Callable, Dict, List, Optional

ExecFn = Callable[["Options"], int]
SubFn = Callable[["Node"], None]


class nil_clixNode(ctypes.Structure):
    _fields_ = [("handle", ctypes.c_void_p)]


class nil_clixOptions(ctypes.Structure):
    _fields_ = [("handle", ctypes.c_void_p)]


class nil_clixFlagInfo(ctypes.Structure):
    _fields_ = [
        ("skey", ctypes.c_char_p),
        ("msg", ctypes.c_char_p),
    ]


class nil_clixNumberInfo(ctypes.Structure):
    _fields_ = [
        ("skey", ctypes.c_char_p),
        ("msg", ctypes.c_char_p),
        ("fallback", ctypes.POINTER(ctypes.c_int64)),
        ("implicit", ctypes.POINTER(ctypes.c_int64)),
    ]


class nil_clixParamInfo(ctypes.Structure):
    _fields_ = [
        ("skey", ctypes.c_char_p),
        ("msg", ctypes.c_char_p),
        ("fallback", ctypes.c_char_p),
    ]


class nil_clixParamsInfo(ctypes.Structure):
    _fields_ = [
        ("skey", ctypes.c_char_p),
        ("msg", ctypes.c_char_p),
    ]


NIL_CLIX_EXEC = ctypes.CFUNCTYPE(ctypes.c_int, ctypes.POINTER(nil_clixOptions), ctypes.c_void_p)
NIL_CLIX_SUB = ctypes.CFUNCTYPE(None, ctypes.POINTER(nil_clixNode), ctypes.c_void_p)
NIL_CLIX_CLEANUP = ctypes.CFUNCTYPE(None, ctypes.c_void_p)
NIL_CLIX_WRITE = ctypes.CFUNCTYPE(None, ctypes.c_char_p, ctypes.c_uint64, ctypes.c_void_p)


class nil_clixExecInfo(ctypes.Structure):
    _fields_ = [
        ("exec", NIL_CLIX_EXEC),
        ("context", ctypes.c_void_p),
        ("cleanup", NIL_CLIX_CLEANUP),
    ]


class nil_clixSubInfo(ctypes.Structure):
    _fields_ = [
        ("exec", NIL_CLIX_SUB),
        ("context", ctypes.c_void_p),
        ("cleanup", NIL_CLIX_CLEANUP),
    ]


class nil_clixWriteInfo(ctypes.Structure):
    _fields_ = [
        ("exec", NIL_CLIX_WRITE),
        ("context", ctypes.c_void_p),
    ]


class nil_clixStrings(ctypes.Structure):
    _fields_ = [
        ("size", ctypes.c_uint64),
        ("data", ctypes.POINTER(ctypes.c_char_p)),
    ]


def _to_ref_id(ptr: Any) -> int:
    return int(ctypes.cast(ptr, ctypes.c_void_p).value)


@dataclass
class _ExecState:
    __slots__ = ("fn",)
    fn: ExecFn


@dataclass
class _SubState:
    __slots__ = ("fn",)
    fn: SubFn


@dataclass
class _WriteState:
    __slots__ = ("output",)
    output: List[str]


RefState = _ExecState | _SubState | _WriteState


@dataclass
class _FnsState:
    __slots__ = ("exec_exec", "sub_exec", "write_exec", "cleanup")
    exec_exec: Any
    sub_exec: Any
    write_exec: Any
    cleanup: Any


def _configure_signatures(clix: Any) -> None:
    clix.nil_clix_node_create.argtypes = []
    clix.nil_clix_node_create.restype = nil_clixNode

    clix.nil_clix_node_destroy.argtypes = [nil_clixNode]
    clix.nil_clix_node_destroy.restype = None

    clix.nil_clix_node_run.argtypes = [nil_clixNode, ctypes.c_int, ctypes.POINTER(ctypes.c_char_p)]
    clix.nil_clix_node_run.restype = ctypes.c_int

    clix.nil_clix_node_flag.argtypes = [nil_clixNode, ctypes.c_char_p, nil_clixFlagInfo]
    clix.nil_clix_node_flag.restype = None

    clix.nil_clix_node_number.argtypes = [nil_clixNode, ctypes.c_char_p, nil_clixNumberInfo]
    clix.nil_clix_node_number.restype = None

    clix.nil_clix_node_param.argtypes = [nil_clixNode, ctypes.c_char_p, nil_clixParamInfo]
    clix.nil_clix_node_param.restype = None

    clix.nil_clix_node_params.argtypes = [nil_clixNode, ctypes.c_char_p, nil_clixParamsInfo]
    clix.nil_clix_node_params.restype = None

    clix.nil_clix_node_use.argtypes = [nil_clixNode, nil_clixExecInfo]
    clix.nil_clix_node_use.restype = None

    clix.nil_clix_node_sub.argtypes = [nil_clixNode, ctypes.c_char_p, ctypes.c_char_p, nil_clixSubInfo]
    clix.nil_clix_node_sub.restype = None

    clix.nil_clix_options_help.argtypes = [nil_clixOptions, nil_clixWriteInfo]
    clix.nil_clix_options_help.restype = None

    clix.nil_clix_options_has_value.argtypes = [nil_clixOptions, ctypes.c_char_p]
    clix.nil_clix_options_has_value.restype = ctypes.c_int

    clix.nil_clix_options_flag.argtypes = [nil_clixOptions, ctypes.c_char_p]
    clix.nil_clix_options_flag.restype = ctypes.c_int

    clix.nil_clix_options_number.argtypes = [nil_clixOptions, ctypes.c_char_p]
    clix.nil_clix_options_number.restype = ctypes.c_int64

    clix.nil_clix_options_param.argtypes = [nil_clixOptions, ctypes.c_char_p]
    clix.nil_clix_options_param.restype = ctypes.c_char_p

    clix.nil_clix_options_params.argtypes = [nil_clixOptions, ctypes.c_char_p]
    clix.nil_clix_options_params.restype = nil_clixStrings


class Options:
    __slots__ = ("_refs", "_fns", "_clix", "_libc", "_options")

    def __init__(self, refs: Dict[int, RefState], fns: _FnsState, clix: Any, libc: Any, options: nil_clixOptions) -> None:
        self._refs = refs
        self._fns = fns
        self._clix = clix
        self._libc = libc
        self._options = options

    def help(self) -> str:
        output: List[str] = []
        writer_id = self._libc.malloc(1)
        if not writer_id:
            raise MemoryError("malloc failed")

        self._refs[_to_ref_id(writer_id)] = _WriteState(output=output)

        write_info = nil_clixWriteInfo(
            exec=self._fns.write_exec,
            context=ctypes.c_void_p(writer_id),
        )

        self._clix.nil_clix_options_help(self._options, write_info)
        return "".join(output)

    def has_value(self, lkey: str) -> bool:
        return self._clix.nil_clix_options_has_value(self._options, lkey.encode()) != 0

    def flag(self, lkey: str) -> bool:
        return self._clix.nil_clix_options_flag(self._options, lkey.encode()) != 0

    def number(self, lkey: str) -> int:
        return int(self._clix.nil_clix_options_number(self._options, lkey.encode()))

    def param(self, lkey: str) -> Optional[str]:
        result = self._clix.nil_clix_options_param(self._options, lkey.encode())
        if not result:
            return None
        return result.decode()

    def params(self, lkey: str) -> List[str]:
        strings = self._clix.nil_clix_options_params(self._options, lkey.encode())
        result: List[str] = []
        for i in range(int(strings.size)):
            result.append(strings.data[i].decode())
        return result


class Node:
    __slots__ = ("_refs", "_fns", "_clix", "_libc", "_node")

    def __init__(self, refs: Dict[int, RefState], fns: _FnsState, clix: Any, libc: Any, node: nil_clixNode) -> None:
        self._refs = refs
        self._fns = fns
        self._clix = clix
        self._libc = libc
        self._node = node

    def flag(self, lkey: str, skey: Optional[str] = None, msg: str = "") -> None:
        flag_info = nil_clixFlagInfo(
            skey=skey.encode() if skey else None,
            msg=msg.encode(),
        )
        self._clix.nil_clix_node_flag(self._node, lkey.encode(), flag_info)

    def number(
        self, lkey: str, skey: Optional[str] = None, msg: str = "", fallback: Optional[int] = None, implicit: Optional[int] = None
    ) -> None:
        fallback_ptr = None
        implicit_ptr = None

        if fallback is not None:
            fallback_ptr = ctypes.pointer(ctypes.c_int64(fallback))

        if implicit is not None:
            implicit_ptr = ctypes.pointer(ctypes.c_int64(implicit))

        number_info = nil_clixNumberInfo(
            skey=skey.encode() if skey else None,
            msg=msg.encode(),
            fallback=fallback_ptr,
            implicit=implicit_ptr,
        )
        self._clix.nil_clix_node_number(self._node, lkey.encode(), number_info)

    def param(self, lkey: str, skey: Optional[str] = None, msg: str = "", fallback: Optional[str] = None) -> None:
        param_info = nil_clixParamInfo(
            skey=skey.encode() if skey else None,
            msg=msg.encode(),
            fallback=fallback.encode() if fallback else None,
        )
        self._clix.nil_clix_node_param(self._node, lkey.encode(), param_info)

    def params(self, lkey: str, skey: Optional[str] = None, msg: str = "") -> None:
        params_info = nil_clixParamsInfo(
            skey=skey.encode() if skey else None,
            msg=msg.encode(),
        )
        self._clix.nil_clix_node_params(self._node, lkey.encode(), params_info)

    def use(self, fn: ExecFn) -> None:
        ctx_id = self._libc.malloc(1)
        if not ctx_id:
            raise MemoryError("malloc failed")

        exec_info = nil_clixExecInfo(
            exec=self._fns.exec_exec,
            context=ctypes.c_void_p(ctx_id),
            cleanup=self._fns.cleanup,
        )

        self._refs[_to_ref_id(ctx_id)] = _ExecState(fn=fn)
        self._clix.nil_clix_node_use(self._node, exec_info)

    def sub(self, key: str, description: str, fn: SubFn) -> None:
        ctx_id = self._libc.malloc(1)
        if not ctx_id:
            raise MemoryError("malloc failed")

        sub_info = nil_clixSubInfo(
            exec=self._fns.sub_exec,
            context=ctypes.c_void_p(ctx_id),
            cleanup=self._fns.cleanup,
        )

        self._refs[_to_ref_id(ctx_id)] = _SubState(fn=fn)
        self._clix.nil_clix_node_sub(self._node, key.encode(), description.encode(), sub_info)

    def run(self, argv: List[str]) -> int:
        argc = len(argv)
        c_argv = (ctypes.c_char_p * (argc))()

        for i, arg in enumerate(argv):
            c_argv[i] = arg.encode()

        return int(self._clix.nil_clix_node_run(self._node, argc, c_argv))

    def destroy(self) -> None:
        self._clix.nil_clix_node_destroy(self._node)


class Clix:
    __slots__ = ("_clix", "_refs", "_fns", "_libc")

    def __init__(self, clix: Any) -> None:
        self._clix = clix
        self._refs: Dict[int, RefState] = {}
        self._fns: _FnsState | None = None

        self._libc = ctypes.CDLL(None)
        self._libc.malloc.argtypes = [ctypes.c_size_t]
        self._libc.malloc.restype = ctypes.c_void_p
        self._libc.free.argtypes = [ctypes.c_void_p]
        self._libc.free.restype = None

        @NIL_CLIX_EXEC
        def exec_exec(options_ptr: Any, ptr: Any) -> int:
            options_obj = Options(self._refs, self._fns, self._clix, self._libc, options_ptr.contents)
            return self._refs[_to_ref_id(ptr)].fn(options_obj)

        @NIL_CLIX_SUB
        def sub_exec(node_ptr: Any, ptr: Any) -> None:
            node_obj = Node(self._refs, self._fns, self._clix, self._libc, node_ptr.contents)
            self._refs[_to_ref_id(ptr)].fn(node_obj)

        @NIL_CLIX_WRITE
        def write_exec(data: Any, size: int, ptr: Any) -> None:
            ref = self._refs.get(_to_ref_id(ptr))
            if ref and isinstance(ref, _WriteState):
                ref.output.append(ctypes.string_at(data, size).decode())

        @NIL_CLIX_CLEANUP
        def cleanup(ptr: Any) -> None:
            if ptr:
                self._refs.pop(_to_ref_id(ptr), None)
                self._libc.free(ctypes.c_void_p(ptr))

        self._fns = _FnsState(
            exec_exec=exec_exec,
            sub_exec=sub_exec,
            write_exec=write_exec,
            cleanup=cleanup,
        )

    def create_node(self) -> Node:
        node = self._clix.nil_clix_node_create()
        return Node(self._refs, self._fns, self._clix, self._libc, node)


def _load_clix_from_module_dir() -> Clix:
    lib_path = Path(__file__).resolve().parent / "libnil-clix-c-api.so"
    clix = ctypes.CDLL(str(lib_path))
    _configure_signatures(clix)
    return Clix(clix)


_CLIX = _load_clix_from_module_dir()


def create_node() -> Node:
    return _CLIX.create_node()


__all__ = ["create_node", "Clix", "Node", "Options"]
