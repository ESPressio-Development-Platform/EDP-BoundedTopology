#!/usr/bin/env python3

from __future__ import annotations

import os
import pathlib
import shutil
import subprocess
import sys


def main() -> int:
    root = pathlib.Path(__file__).resolve().parents[1]
    build = root / "tests" / ".test-build"
    build.mkdir(parents=True, exist_ok=True)

    compiler = os.environ.get("CXX")
    if not compiler:
        compiler = shutil.which("c++") or shutil.which("g++") or shutil.which("clang++")

    if not compiler:
        print("ERROR: no C++ compiler found; set CXX or install c++/g++/clang++.", file=sys.stderr)
        return 2

    executable = build / "bounded-topology-tests"

    command = [
        compiler,
        "-std=c++20",
        "-Wall",
        "-Wextra",
        "-Wpedantic",
        "-Werror",
        "-I",
        str(root / "src"),
        str(root / "tests" / "BoundedTopologyTests.cpp"),
        "-o",
        str(executable),
    ]

    print("Compiler:", compiler)
    print("[1/2] Building host tests...")
    subprocess.run(command, check=True)

    print("[2/2] Running host tests...")
    subprocess.run([str(executable)], check=True)

    print("EDP-BoundedTopology host tests: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
