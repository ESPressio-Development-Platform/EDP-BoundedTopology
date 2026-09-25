# Reference — BoundedIndexSet.hpp

**Source:** `src/bounded_topology/BoundedIndexSet.hpp`  
**Owning namespace:** `ESPressio::BoundedTopology`

## BoundedIndexSetMutationResult

**Classification:** PUBLIC API result Type.

- `Succeeded = 0` — requested membership state was established; Set/Clear are idempotent.
- `InvalidIndex = 1` — supplied strong index is Invalid/outside the represented space; no state is changed.

## BoundedIndexSet<TIndexSpace, TCapacity> — positive capacity

**Classification:** PUBLIC API with private compact representation.

### Template parameters

- `TIndexSpace` — semantic tag shared with every `Index` accepted/returned by this set.
- `TCapacity` — positive number of membership positions. A static assertion rejects zero in this primary specialization; zero is handled by the dedicated specialization.

### Private metadata/state

- `IndexType` — PRIVATE IMPLEMENTATION alias for `BoundedIndex<TIndexSpace, TCapacity>`.
- `StorageByteCount` — PRIVATE IMPLEMENTATION compile-time `ceil(TCapacity/8)`.
- `_bytes[StorageByteCount]` — PRIVATE IMPLEMENTATION **only retained state**; one bit per valid index.

### Private helpers

- `ByteIndex(IndexType)` — O(1) mapping from known-valid index to backing byte.
- `BitMask(IndexType)` — O(1) mapping from known-valid index to one bit inside that byte.
- `ValidMask(std::size_t byteIndex)` — returns all valid bits for a backing byte; on the final partial byte it excludes unused tail bits.

These helpers retain no state.

### Public metadata

- `Index` — public strong index Type.
- `StorageBytes` — exact positive-capacity retained bitmap bytes.

### Construction

Default construction clears all membership bits.

### Mutation

- `Set(Index)` — validates index, then sets its bit. Returns `Succeeded` or `InvalidIndex`.
- `Clear(Index)` — validates index, then clears its bit. Returns `Succeeded` or `InvalidIndex`.
- `ClearAll()` — writes zero to every backing byte. O(ceil(N/8)).
- `SetAll()` — fills every backing byte with its valid mask; unused final-byte tail bits remain clear. O(ceil(N/8)).

### Inspection/query

- `IsSet(Index)` — false for Invalid; otherwise O(1) bit test.
- `IsEmpty()` — scans backing bytes and returns true only when all are zero.
- `IsAnySet()` — logical inverse of `IsEmpty()`.
- `Count()` — scans all N logical indices and calculates membership count; **no cached count exists**.
- `FindFirstSet()` — returns lowest set index or Invalid. Bounded O(N).
- `FindFirstClear()` — returns lowest clear index or Invalid when all represented indices are set. Bounded O(N).

## Zero-capacity specialization

`BoundedIndexSet<TIndexSpace, 0U>` is **PUBLIC API** and contains no retained members.

Public declarations:

- `Index = BoundedIndex<TIndexSpace,0U>`;
- `StorageBytes = 0`;
- default constructor;
- `Set` / `Clear` always return `InvalidIndex`;
- `ClearAll` / `SetAll` are no-ops;
- `IsSet` and `IsAnySet` always false;
- `IsEmpty` always true;
- `Count` returns zero;
- `FindFirstSet` / `FindFirstClear` return Invalid.

A standalone empty C++ object still has implementation-defined non-zero `sizeof`, but owners may use `[[no_unique_address]]`; the host suite requires such composition to add zero bytes beside a one-byte marker.

## Memory invariant

For every positive N, the intended ABI/storage contract is:

`sizeof(BoundedIndexSet<TSpace,N>) == ceil(N/8)`.

Representative boundary sizes are compile-time tested. There is no cached count, capacity member, provider reference, lock, pointer, telemetry, or heap storage.

## Ownership/lifecycle

The set owns only its bitmap. It does not own any object corresponding to a set index. No initialization/teardown beyond normal value construction/destruction exists.

## Concurrency / ISR

No synchronization is included. The owner serializes shared mutation. No general ISR guarantee is made.
