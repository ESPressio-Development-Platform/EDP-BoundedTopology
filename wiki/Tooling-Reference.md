# Tooling Reference

EDP-BoundedTopology contains no substantive maintained compiler/generator/decompiler/CLI tooling.

`tests/run_tests.py` is repository validation support rather than a consumer toolchain. It selects `CXX` or an available host C++ compiler, builds `tests/BoundedTopologyTests.cpp` with the repository `src` include path and C++20 warnings-as-errors flags, then executes the binary.

No generated production artefact or schema is owned by this repository.
