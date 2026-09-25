# EDP-BoundedTopology

`EDP-BoundedTopology` owns the smallest shared runtime topology vocabulary needed for fixed finite index spaces across ESPressio.

It owns three public primitives:

- `BoundedIndex<TIndexSpace, N>` — compact strong identity;
- `BoundedIndexSet<TIndexSpace, N>` — one-bit-per-index membership;
- `IntrusiveQueue<TIndexSpace, N>` — non-owning FIFO ordering with linkage stored in external records.

It deliberately does **not** own payload storage/lifetime, allocation, synchronization, waiting, providers, Composition, resource routing, semantic Pool behaviour, serialization, or domain-specific state.

## Public entry point

`src/ESPressio_BoundedTopology.hpp` is the public aggregation header.

## Dependencies

The production library has no ESPressio-library dependency and no optional dependency.

## Wiki navigation

- [Architecture](Architecture)
- [Dependency Contracts](Dependency-Contracts)
- [Composition and Providers](Composition-and-Providers)
- [Public API](Public-API)
- [Internal API](Internal-API)
- [Private Implementation](Private-Implementation)
- [Resources, Lifecycle and Concurrency](Resources-Lifecycle-Concurrency)
- [Build, Test and Source Navigation](Build-Test-Source-Navigation)
- [Compiler Definitions](Compiler-Definitions)
- [Tooling Reference](Tooling-Reference)
- [Reference Index](Reference-Index)
