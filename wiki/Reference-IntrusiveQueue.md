# Reference — IntrusiveQueue.hpp

**Source:** `src/bounded_topology/IntrusiveQueue.hpp`  
**Owning namespace:** `ESPressio::BoundedTopology`

## Result enums

### IntrusiveQueuePushResult

**Classification:** PUBLIC API.

- `Succeeded = 0` — record was appended.
- `InvalidIndex = 1` — supplied index was invalid; queue/records are unchanged.

### IntrusiveQueuePopResult

**Classification:** PUBLIC API.

- `Succeeded = 0` — old head was removed and written to caller output.
- `Empty = 1` — no record existed; output remains unchanged.

### IntrusiveQueueRemoveResult

**Classification:** PUBLIC API.

- `Removed = 0` — requested queued record was removed.
- `NotFound = 1` — valid requested record was not found during the bounded traversal.
- `InvalidIndex = 2` — supplied requested index was invalid.

## IntrusiveQueue<TIndexSpace, TCapacity> — positive capacity

**Classification:** PUBLIC API with private endpoint representation.

### Template parameters

- `TIndexSpace` — semantic index-space tag shared with represented records.
- `TCapacity` — positive maximum number of addressable records; it determines index width and the hard upper bound for specific-removal traversal.

### External record contract

For a queue with public `Index`, each record reached through the caller-supplied indexed collection must provide:

```cpp
Index QueueNext() const noexcept;
void SetQueueNext(Index index) noexcept;
```

The collection must support `operator[](std::size_t)` and contain every valid index addressed by the queue. The queue retains no collection pointer/reference.

**Caller invariant:** a record must not be pushed into the same queue while it is already linked there. Duplicate-link detection is deliberately not retained/scanned on Push because the owning domain state machine is responsible for that invariant.

### Private metadata/state

- `IndexType` — PRIVATE IMPLEMENTATION alias for `BoundedIndex<TIndexSpace,TCapacity>`.
- `_head` — PRIVATE IMPLEMENTATION authoritative first queued record; Invalid encodes empty.
- `_tail` — PRIVATE IMPLEMENTATION authoritative last queued record; retained to make append O(1).

No other state exists.

### Public metadata

- `Index` — public strong record index Type.

### Construction

Default construction sets both endpoints Invalid, establishing empty state.

### Inspection

- `IsEmpty()` — true when head is Invalid.
- `Head()` — current head or Invalid.
- `Tail()` — current tail or Invalid.

### Mutation

- `Push(TRecords& records, Index recordIndex)` — validates index, clears the new record's external next-link, connects previous tail when present, then publishes the new tail. O(1).
- `Pop(TRecords& records, Index& output)` — if non-empty, reads old head's external next-link, advances head, clears tail if queue becomes empty, clears removed record's link, then writes output. O(1). Empty failure preserves output.
- `Remove(TRecords& records, Index recordIndex)` — validates requested index then traverses from head for at most `TCapacity` links, reconnecting predecessor/head and tail when found. O(N) bounded.

The explicit visit bound prevents a corrupt/cyclic external linkage chain from causing an unbounded loop. The operation reports `NotFound` if no match is reached inside that bound.

## Zero-capacity specialization

`IntrusiveQueue<TIndexSpace,0U>` is **PUBLIC API** and retains no endpoint members.

- `Index = BoundedIndex<TIndexSpace,0U>`;
- `IsEmpty` always true;
- `Head` / `Tail` return Invalid;
- `Push` returns `InvalidIndex` without accessing records;
- `Pop` returns `Empty` without accessing records/output;
- `Remove` returns `InvalidIndex` without accessing records.

With `[[no_unique_address]]`, an owning facility can compose this empty Type without additional retained bytes; the host tests enforce that property.

## Memory invariant

For positive capacity:

`sizeof(IntrusiveQueue<TSpace,N>) == 2 * sizeof(BoundedIndex<TSpace,N>)`.

The queue does not store size, capacity, empty flag, link array, record pointer, allocator/provider state, synchronization, or history.

## Ownership/lifecycle

The queue owns only endpoint topology. Externally supplied records own linkage and all payload/lifetime semantics. No setup/teardown or allocation exists.

## Concurrency / ISR

The queue is unsynchronized. The owning domain must serialize shared mutation and protect external links consistently. No general ISR guarantee is made.
