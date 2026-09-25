# EDP-BoundedTopology Dependency Graph

> Established: 2026-09-25 — initial 0.1.0 development baseline.

## Library Dependency Tree

```text
EDP-BoundedTopology
└── no ESPressio-library dependencies
```

## Mandatory Dependencies

EDP-BoundedTopology has no mandatory ESPressio-library dependencies.

The production headers use only C++20 standard-language/library facilities required for fixed-width integers, size values, numeric limits, and compile-time Type selection.

## Optional Dependencies

EDP-BoundedTopology has no optional ESPressio-library dependencies.

## Dependency direction

The library is intentionally dependency-minimal so higher facilities such as EDP-Memory, EDP-BoundedTypes, EDP-Threading, EDP-Command, and other future runtime domains can consume the same finite-index topology without introducing dependency cycles or importing unrelated resource semantics.

Consumers are not dependencies of EDP-BoundedTopology.
