# Reference — BoundedIndex.hpp

**Source:** `src/bounded_topology/BoundedIndex.hpp`  
**Owning namespace:** `ESPressio::BoundedTopology`

## BoundedIndexCreationResult

**Classification:** PUBLIC API result Type.

Dedicated checked-construction outcome:

- `Succeeded = 0` — the requested numeric value was within `0..N-1` and the output was updated.
- `OutOfRange = 1` — the requested numeric value was not representable as a valid member; caller output remains unchanged.

## BoundedIndex<TIndexSpace, TCapacity>

**Classification:** PUBLIC API with private representation helpers.

### Purpose

Strong compact identity for one member of one compile-time finite semantic index space.

### Template parameters

- `TIndexSpace` — semantic tag Type. It changes C++ Type identity but adds no runtime storage.
- `TCapacity` — number of valid indices. It controls valid range and storage width.

### Private declarations

- `StorageType` — PRIVATE IMPLEMENTATION alias choosing `uint8_t`, `uint16_t`, `uint32_t`, or `uint64_t` from capacity.
- `_value` — PRIVATE IMPLEMENTATION authoritative scalar representation. It is the Type's only retained member. Default state is the maximum scalar value.
- private `BoundedIndex(StorageType)` constructor — PRIVATE IMPLEMENTATION construction path used after the caller/helper has established the intended representation.

### Public metadata

- `Storage` — selected scalar representation Type.
- `IndexSpace` — exposes `TIndexSpace` for compile-time relationships.
- `InvalidValue` — maximum value of `Storage`; reserved as the canonical invalid sentinel.

### Construction

- default constructor — creates Invalid.
- `Invalid()` — returns the canonical Invalid value.
- `FromUnchecked(std::size_t value)` — creates an index without validating `value < TCapacity`. **Precondition:** caller has already proven the range. Intended for bounded internal loops/compile-time-proven paths.
- `TryCreate(std::size_t value, BoundedIndex& output)` — checked construction. Returns `Succeeded` and replaces output when valid; returns `OutOfRange` and preserves output otherwise.

### Inspection

- `Capacity()` — compile-time capacity as `std::size_t`.
- `IsValid()` — true only when stored value is less than capacity. This also defensively rejects any non-canonical out-of-range scalar.
- `Value()` — returns the exact scalar representation. Calling on Invalid is permitted and returns the sentinel; consumers should normally check `IsValid()` before treating it as an ordinal.

### Comparison

- `operator==` — exact same-Type representation equality.
- `operator!=` — exact same-Type representation inequality.

Cross-space comparison is prevented by the C++ Type system because different `TIndexSpace` tags produce distinct Types.

### Memory and lifecycle

The complete retained state is one scalar. No separate validity byte, runtime capacity, pointer, provider, lock, heap allocation, or registry exists.

Storage boundaries:

- capacities 0..255: 1 byte;
- 256..65,535: 2 bytes;
- 65,536..4,294,967,295: 4 bytes;
- larger representable target capacities: 8 bytes.

Construction/destruction requires no external lifecycle.

### Concurrency / ISR

Value instances contain ordinary scalar state and no synchronization. Shared mutable access follows normal C++ data-race rules. The library makes no general ISR guarantee.
