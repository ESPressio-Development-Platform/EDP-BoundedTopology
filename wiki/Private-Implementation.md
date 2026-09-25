# Private Implementation

## BoundedIndex

`StorageType` selects the minimum unsigned scalar width from compile-time capacity. `_value` is the **only retained member**. The private scalar constructor is used by `Invalid()` and `FromUnchecked()` after the relevant caller/path establishes its intended precondition.

## BoundedIndexSet positive capacity

`IndexType` is the private strong index alias. `StorageByteCount` is the compile-time `ceil(N/8)` byte count. `_bytes` is the **only retained state**.

Private helpers:

- `ByteIndex(IndexType)` maps one valid index to a backing-byte ordinal;
- `BitMask(IndexType)` maps one valid index to its bit;
- `ValidMask(byteIndex)` prevents unused tail bits from becoming set in the final partial byte.

The zero-capacity specialization has no private member state.

## IntrusiveQueue positive capacity

`IndexType` is the private strong index alias. `_head` and `_tail` are the only retained members. Invalid head means empty; tail is retained to preserve O(1) append.

`Remove` bounds traversal by `TCapacity` so a corrupt/cyclic external link cannot produce an unbounded library loop.

The zero-capacity specialization has no private member state.

## No hidden state

There are no globals, registries, static runtime caches, heap allocations, provider references, locks, atomics, telemetry counters, or background workers.
