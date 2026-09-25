# EDP-BoundedTopology

`EDP-BoundedTopology` provides the dependency-minimal bounded topology primitives used to describe finite runtime identity, compact membership state, and non-owning FIFO ordering across ESPressio domains.

The library is deliberately smaller than a container library. It does not own payload storage, object lifetime, allocation, synchronization, providers, resource routing, or domain semantics.

## Public facilities

- `BoundedIndex<TIndexSpace, N>` — strong capacity-derived identity for one position in a finite index space.
- `BoundedIndexSet<TIndexSpace, N>` — compact membership state using exactly one bit per represented index for positive capacity.
- `IntrusiveQueue<TIndexSpace, N>` — non-owning FIFO topology retaining only compact head/tail indices while next linkage remains in externally owned records.

There is intentionally no generic state-owning `Pool` Type. Domains compose only the index-set state they actually require.

## Documentation

- [Architecture](./docs/ARCHITECTURE.MD) — ownership boundaries, primitive relationships, queue-record contract, and explicit non-goals.
- [Memory and complexity](./docs/MEMORY_AND_COMPLEXITY.MD) — exact retained-state rules, capacity-derived widths, zero-capacity behaviour, and bounded operation costs.
- [Building and validation](./docs/BUILDING.MD) — C++20 baseline, host tests, PIOArduino guidance, and demo build surfaces.
- [Demos](./demos/README.MD) — buildable demonstration matrix.

## Memory model

The library follows a strict memory-first rule:

> Persistent derived or cached state is not retained merely to make a bounded query cheaper.

For positive capacity:

```text
BoundedIndex
    smallest scalar able to represent 0..N-1 plus Invalid

BoundedIndexSet
    ceil(N / 8) bytes

IntrusiveQueue
    2 × sizeof(BoundedIndex)
```

`BoundedIndexSet` retains no population count. `IntrusiveQueue` retains no queue size or owned link table. Zero-capacity set/queue specializations contain no state and can disappear inside an owning object when composed with `[[no_unique_address]]`.

## Dependencies

```text
EDP-BoundedTopology
└── no ESPressio-library dependencies
```

The production headers use only C++20 standard-language/library facilities.

See [DEPENDENCY_GRAPH.MD](./DEPENDENCY_GRAPH.MD) for the maintained dependency graph.

## Build baseline

EDP-BoundedTopology requires C++20.

For Arduino-ESP32 development, ESPressio recommends PIOArduino because it tracks a current Arduino-ESP32 toolchain with C++20 support. Conventional PlatformIO remains usable when the selected platform/toolchain is C++20-capable and the project explicitly builds with `-std=gnu++20`.

## Tests

Run the host suite from the repository root:

```bash
python3 tests/run_tests.py
```

The suite compiles with warnings as errors and validates capacity-derived index widths, semantic index-space separation, checked construction, exact set/queue footprints, membership/FIFO behaviour, and zero-capacity composition.
