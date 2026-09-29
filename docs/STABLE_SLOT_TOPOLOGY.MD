# Stable Bounded Slot Topology

## Purpose

`BoundedSlotTopology<TIndexSpace, N>` provides reusable stable slot identity and occupancy over a finite compile-time index space without owning the payload represented by any occupied slot.

The component exists for domains that need all of the following together:

- a compact strong slot index;
- one occupancy fact per slot;
- deterministic acquisition of a free slot;
- slot identity that remains stable until explicit release;
- bounded iteration over occupied slots;
- no state-owning Pool semantics.

The first identified consumer is `EDP-RemoteSystem::DeviceRegistry`, but the contract is deliberately domain-neutral.

## Ownership boundary

The topology owns exactly one semantic fact per representable slot: **occupied or free**.

It does not own:

- payload storage or payload objects;
- construction, destruction, movement or copying of payloads;
- payload lifetime or cleanup ordering;
- generation counters or stale-reference protection;
- leases, owner tokens or claim authorization;
- synchronization, waits, blocking or ISR handoff;
- callbacks or provider semantics;
- domain identity such as a Remote Device identifier;
- dynamic allocation.

A consuming domain must couple slot occupancy to its external payload publication/retirement under whatever synchronization and transaction boundary that domain requires.

## Public Types

`BoundedSlotAcquisitionStatus` has two outcomes:

- `Succeeded` — a slot was acquired;
- `Full` — no free slot exists.

`BoundedSlotAcquisitionResult<TIndexSpace, N>` carries the status and, on success, the acquired strong `BoundedIndex`. On failure, `AcquiredIndex()` returns the canonical Invalid index rather than a fabricated slot.

`BoundedSlotReleaseResult` distinguishes:

- `Succeeded`;
- `InvalidIndex`;
- `NotOccupied`.

This distinction allows a consuming domain to separate malformed/out-of-space identity from double release or stale lifecycle use.

## Acquisition

`Acquire()` performs a bounded scan for the numerically lowest free slot. When found, the corresponding occupancy bit is set before the successful result is returned.

The deterministic lowest-free rule is intentional. The topology does not retain a free list, first-free cache, rotating cursor or allocation history merely to make acquisition cheaper.

When every slot is occupied, acquisition returns `Full` and the result carries Invalid.

## Stability and release

An acquired slot remains occupied until `Release(Index)` succeeds. Other acquire/release operations do not renumber or relocate it.

`Release()` changes only the occupancy bit. It never destroys or clears an external payload because payload lifetime is outside this component.

The caller must therefore retire or otherwise make external payload state safe according to its own contract before allowing the slot to be reused when such ordering matters.

## Inspection and traversal

The topology exposes:

- `Capacity()`;
- `IsOccupied(Index)`;
- `IsEmpty()`;
- `IsFull()`;
- `Count()`;
- `FindFirstOccupied()`;
- `FindNextOccupied(Index)`.

Traversal is stateless. `FindFirstOccupied()` returns the lowest occupied slot. `FindNextOccupied(index)` returns the next occupied slot whose numeric value is greater than the supplied valid index. The supplied index need not itself be occupied. Invalid input returns Invalid.

No iterator object, traversal cursor or callback is retained by the topology.

## Memory contract

For positive capacity the complete persistent semantic state is the composed `BoundedIndexSet<TIndexSpace, N>` occupancy bitmap:

```text
StorageBytes = ceil(N / 8)
sizeof(BoundedSlotTopology<TSpace, N>) == StorageBytes
```

There is no cached population count, first-free slot, generation value, capacity member, lock, pointer or payload field.

For zero capacity, `StorageBytes == 0`. The topology has no semantic member state; when embedded using `[[no_unique_address]]`, it need not increase the owner footprint.

## Complexity

| Operation | Complexity |
| --- | ---: |
| `IsOccupied` | O(1) |
| `IsEmpty` | O(ceil(N/8)) |
| `Acquire` | O(N) bounded scan |
| `Release` | O(1) |
| `IsFull` | O(N) bounded scan |
| `Count` | O(N) bounded scan |
| `FindFirstOccupied` | O(N) bounded scan |
| `FindNextOccupied` | O(N) bounded scan from supplied position |

These bounded scans are deliberate RAM-saving tradeoffs.

## Zero-capacity semantics

For `BoundedSlotTopology<TIndexSpace, 0U>`:

- `Capacity()` is zero;
- `StorageBytes` is zero;
- `IsEmpty()` and `IsFull()` are both true;
- `Count()` is zero;
- `Acquire()` returns `Full` with Invalid;
- `Release(Invalid)` returns `InvalidIndex`;
- occupied-slot traversal returns Invalid.

A zero-capacity topology therefore remains a valid compile-time configuration rather than requiring conditional removal from a surrounding generic owner.

## Concurrency and ISR boundary

The topology contains no synchronization. A consuming domain must serialize concurrent read/write access according to its own ownership model.

No general ISR-safety guarantee is made. The implementation is local, bounded and allocation-free, but ISR suitability remains a property of the consuming domain's complete call path and synchronization policy.

## Reuse rule

Higher domains needing stable bounded acquire/release occupancy semantics should consume this component rather than composing a private `BoundedIndex` + bitmap allocator with overlapping behavior.

The component should not be expanded into a generic payload-owning Pool merely because a particular consumer stores an object beside each slot; payload ownership remains with the correct higher domain or `EDP-Memory` abstraction.
