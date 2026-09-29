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

## BoundedSlotTopology

`OccupancySet` is the private `BoundedIndexSet<TIndexSpace,TCapacity>` alias. `_occupied` is the **only retained semantic member** and is marked `[[no_unique_address]]` so zero-capacity empty state can overlap owner storage.

`Acquire()` delegates free-slot discovery to `BoundedIndexSet::FindFirstClear()` and then establishes occupancy through `Set()`. `Release()` validates both strong-index validity and current occupancy before `Clear()`. Traversal performs numeric bounded scans without retaining a cursor.

`BoundedSlotAcquisitionResult` privately retains only its status and result index; its constructor is accessible only to the matching topology specialization so a successful status cannot be paired with a fabricated unrelated index through the supported API.

## IntrusiveQueue positive capacity

`IndexType` is the private strong index alias. `_head` and `_tail` are the only retained members. Invalid head means empty; tail is retained to preserve O(1) append.

`Remove` bounds traversal by `TCapacity` so a corrupt/cyclic external link cannot produce an unbounded library loop.

The zero-capacity specialization has no private member state.

## No hidden state

There are no globals, registries, static runtime caches, heap allocations, provider references, locks, atomics, telemetry counters, or background workers.
