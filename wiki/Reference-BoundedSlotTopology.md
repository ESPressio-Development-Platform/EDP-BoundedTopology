# Reference — BoundedSlotTopology.hpp

**Source:** `src/bounded_topology/BoundedSlotTopology.hpp`  
**Owning namespace:** `ESPressio::BoundedTopology`

## BoundedSlotAcquisitionStatus

**Classification:** PUBLIC API result-status Type.

- `Succeeded = 0` — one free stable slot was acquired and marked occupied.
- `Full = 1` — no free slot exists; no topology state changes.

## BoundedSlotReleaseResult

**Classification:** PUBLIC API result Type.

- `Succeeded = 0` — one occupied slot was released.
- `InvalidIndex = 1` — supplied index is Invalid/outside the topology index space; no state changes.
- `NotOccupied = 2` — supplied index is valid but currently free; no state changes.

## BoundedSlotTopology<TIndexSpace, TCapacity> forward declaration

**Classification:** PUBLIC API declaration required so the acquisition result can grant controlled construction access to the matching topology specialization.

Template parameters have the same meaning as the complete topology Type described below.

## BoundedSlotAcquisitionResult<TIndexSpace, TCapacity>

**Classification:** PUBLIC API structured result with private invariant-preserving construction.

### Template parameters

- `TIndexSpace` — semantic slot-space tag shared with the owning topology.
- `TCapacity` — compile-time slot count shared with the owning topology.

### Private metadata/state

- `IndexType` — `BoundedIndex<TIndexSpace, TCapacity>`.
- `_status` — authoritative `BoundedSlotAcquisitionStatus`.
- `_index` — acquired slot on success; canonical Invalid on `Full`.

The private constructor accepts exactly the status/index pair chosen by the matching `BoundedSlotTopology`. The friend declaration exists solely to prevent arbitrary external construction of inconsistent status/index combinations.

### Public metadata and inspection

- `Index` — public strong slot index Type.
- `Status()` — returns the operation outcome.
- `IsSucceeded()` — true only for `Succeeded`.
- `AcquiredIndex()` — returns the acquired slot on success or Invalid when no payload is present.

The result owns no external resource and requires no release action.

## BoundedSlotTopology<TIndexSpace, TCapacity>

**Classification:** PUBLIC API with private compact occupancy representation.

### Template parameters

- `TIndexSpace` — semantic Type tag preventing accidental exchange with unrelated equal-capacity slot spaces.
- `TCapacity` — compile-time number of stable slots.

### Private metadata/state

- `OccupancySet` — PRIVATE IMPLEMENTATION alias for `BoundedIndexSet<TIndexSpace, TCapacity>`.
- `_occupied` — PRIVATE IMPLEMENTATION **only retained semantic state**. It is marked `[[no_unique_address]]` so zero-capacity empty state can overlap owner storage.

No free list, count, generation, first-free cursor, payload pointer, lock or capacity member exists.

### Public metadata

- `Index` — `BoundedIndex<TIndexSpace, TCapacity>`.
- `AcquisitionResult` — matching `BoundedSlotAcquisitionResult<TIndexSpace, TCapacity>`.
- `StorageBytes` — exact occupancy bytes; `ceil(N/8)` for positive capacity and zero for zero capacity.

### Construction

Default construction establishes the fully free topology.

### Slot lifecycle

- `Acquire()` — bounded scan for the numerically lowest free slot. On success sets the occupancy bit and returns `Succeeded` plus that Index. When full returns `Full` plus Invalid. Existing occupied slots never move.
- `Release(Index)` — returns `InvalidIndex` for Invalid input, `NotOccupied` for a valid free slot, or clears the occupied bit and returns `Succeeded`.

Release changes topology only. It never destroys, clears or otherwise mutates externally owned payload state.

### Occupancy inspection

- `Capacity()` — compile-time capacity.
- `IsOccupied(Index)` — O(1) occupancy predicate; Invalid returns false through the underlying set contract.
- `IsEmpty()` — true when no slot is occupied.
- `IsFull()` — true when no clear occupancy bit exists.
- `Count()` — calculates occupied count by bounded scan; no cached count exists.

### Stateless traversal

- `FindFirstOccupied()` — lowest occupied slot or Invalid.
- `FindNextOccupied(Index)` — next occupied numeric slot greater than the supplied valid index; the supplied slot itself need not be occupied. Invalid input or no later occupied slot returns Invalid.

Traversal retains no cursor or iterator state and invokes no caller callback.

## Memory invariant

For positive N:

`sizeof(BoundedSlotTopology<TSpace,N>) == ceil(N/8)`.

This is the complete persistent semantic storage cost. Representative 1, 8, 9 and 255-slot boundaries are compile-time tested.

For zero capacity the topology has no semantic state. A complete C++ object still has non-zero `sizeof`, but host tests require `[[no_unique_address]]` composition beside a one-byte marker to add no owner bytes.

## Ownership and lifecycle

The topology owns only occupancy. Payload objects, payload lifetime, retirement ordering, generation/stale-reference semantics and domain identity remain external. Construction establishes free state; no initialization or teardown phase exists.

## Concurrency / ISR

No synchronization is included. The consuming owner must serialize concurrent mutation or read/write access. No general ISR-safety guarantee is made.
