# EDP-BoundedTopology Architecture

## Purpose

EDP-BoundedTopology owns a very small family of reusable runtime topology mechanics for fixed finite index spaces.

It exists because several higher ESPressio domains need the same low-level structures without sharing payload semantics:

```text
finite identity
compact membership state
FIFO relationship between externally owned records
```

The library deliberately does not become a generic container, allocator, resource manager, scheduler, provider registry, or semantic Pool.

## Ownership boundary

EDP-BoundedTopology owns:

- capacity-derived strong index identity;
- compact bounded membership over that identity;
- non-owning intrusive FIFO ordering over externally owned records.

It does not own:

- represented payload objects or their lifetime;
- raw backing allocation;
- Memory Object Pool semantics;
- Task/Thread lifecycle;
- Command/Event semantics;
- resource health, ranking, routing, retries, or load balancing;
- synchronization, waiting, timeouts, cancellation, or ISR handoff;
- provider selection or Composition;
- serialization or wire representation.

## Strong bounded index identity

`BoundedIndex<TIndexSpace, N>` identifies one position in one compile-time finite index space.

`TIndexSpace` is a semantic Type tag. It carries no runtime state, but prevents an index from one domain from being passed accidentally into another index space with the same numeric capacity.

The valid values are `0 .. N - 1`. The maximum value of the selected storage scalar is reserved as the canonical Invalid sentinel.

Storage width is capacity-derived:

```text
N <= 255             -> uint8_t
N <= 65,535          -> uint16_t
N <= 4,294,967,295   -> uint32_t
otherwise            -> uint64_t where representable by the target
```

The public checked construction path is `TryCreate`. `FromUnchecked` exists only for paths where the caller already proves `value < N`, such as a bounded internal loop. No separate validity Boolean is stored.

## Bounded index set

`BoundedIndexSet<TIndexSpace, N>` stores one membership bit for every valid index.

For positive capacity its complete persistent representation is `ceil(N / 8)` bytes. There is no cached member count, first-set/first-clear index, capacity field, lock, provider pointer, history or telemetry.

`Count`, `FindFirstSet`, and `FindFirstClear` perform bounded scans. This is intentional: bounded execution work is preferred over permanently retained redundant RAM when the operation remains deterministic.

The set assigns no semantic meaning to a set bit. A consumer may interpret membership as available, occupied, present, active, eligible, or another domain-specific Boolean fact.

The zero-capacity specialization contains no state.

## Intrusive FIFO topology

`IntrusiveQueue<TIndexSpace, N>` owns only head and tail indices. The queue does not own an N-entry link array.

Every externally owned record participating in the queue must expose:

```cpp
Index QueueNext() const noexcept;
void SetQueueNext(Index index) noexcept;
```

The queue uses `operator[]` on the caller-supplied record collection to reach the indexed record.

Keeping the tail costs one compact index but preserves deterministic O(1) append. The queue deliberately does not retain a size counter; empty state is encoded by an Invalid head.

Specific removal traverses at most N records. The explicit traversal bound prevents corrupt or cyclic external linkage from turning the library operation into an unbounded loop.

The caller remains responsible for the higher-level invariant that one record is not pushed into the same queue twice while already linked.

## Why Pool is not a primitive

A universal state-owning Pool would force state dimensions that many consumers do not require.

```text
fixed Worker population
    -> availability set only

Memory dedicated slots
    -> occupancy set only

runtime endpoint population
    -> perhaps presence set + eligibility set
```

The lower foundation therefore supplies the minimum index/set mechanics. Higher domains compose exactly the state they need.

EDP-Memory's `ObjectPool<T,...>` remains a separate Memory-domain abstraction owning object storage, lifetime, stable addresses, leases, overflow, waiting, cancellation, and teardown.

## Relationship to EDP-BoundedTypes

EDP-BoundedTypes owns memory-bounded values and value-owning containers such as Vector, Deque, Queue, Stack, CircularBuffer, Set, and Map.

EDP-BoundedTopology is different: it owns no represented values. It describes identity, membership and ordering over state owned elsewhere. No mirrored non-owning Vector/Deque/Stack/CircularBuffer family is part of this architecture.

## Concurrency

The library contains no synchronization. Concurrent mutation or read/write access to one mutable topology object must be serialized by the owning domain/application.

This keeps synchronization policy out of a low-level representation facility and avoids forcing lock state on consumers that already provide a stronger surrounding serialization boundary.

## ISR boundary

The library does not claim a general ISR-safe contract. Its operations are allocation-free and local, but ESPressio ISR use remains governed by the owning domain and platform ISR policy.

## Version baseline

The initial development baseline is `0.1.0`. There are currently no released EDP-* library versions, and this initial implementation does not perform a release/version transition.
