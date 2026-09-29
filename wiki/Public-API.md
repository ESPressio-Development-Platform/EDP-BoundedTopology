# Public API

All public declarations reside in namespace `ESPressio::BoundedTopology` and are available through `src/ESPressio_BoundedTopology.hpp`.

## BoundedIndex

- `BoundedIndexCreationResult::{Succeeded, OutOfRange}`
- `BoundedIndex<TIndexSpace, TCapacity>`
- aliases/constants: `Storage`, `IndexSpace`, `InvalidValue`
- construction: default Invalid, `Invalid()`, `FromUnchecked(value)`, `TryCreate(value, output)`
- inspection: `Capacity()`, `IsValid()`, `Value()`
- comparison: `operator==`, `operator!=`

## BoundedIndexSet

- `BoundedIndexSetMutationResult::{Succeeded, InvalidIndex}`
- `BoundedIndexSet<TIndexSpace, TCapacity>` including zero-capacity specialization
- alias/metadata: `Index`, `StorageBytes`
- mutation: `Set`, `Clear`, `ClearAll`, `SetAll`
- predicates/query: `IsSet`, `IsEmpty`, `IsAnySet`, `Count`, `FindFirstSet`, `FindFirstClear`

## BoundedSlotTopology

- `BoundedSlotAcquisitionStatus::{Succeeded, Full}`
- `BoundedSlotReleaseResult::{Succeeded, InvalidIndex, NotOccupied}`
- `BoundedSlotAcquisitionResult<TIndexSpace, TCapacity>`
- `BoundedSlotTopology<TIndexSpace, TCapacity>`
- aliases/metadata: `Index`, `AcquisitionResult`, `StorageBytes`, `Capacity()`
- lifecycle: `Acquire`, `Release`
- predicates/query: `IsOccupied`, `IsEmpty`, `IsFull`, `Count`
- stateless traversal: `FindFirstOccupied`, `FindNextOccupied`

## IntrusiveQueue

- `IntrusiveQueuePushResult::{Succeeded, InvalidIndex}`
- `IntrusiveQueuePopResult::{Succeeded, Empty}`
- `IntrusiveQueueRemoveResult::{Removed, NotFound, InvalidIndex}`
- `IntrusiveQueue<TIndexSpace, TCapacity>` including zero-capacity specialization
- alias: `Index`
- inspection: `IsEmpty`, `Head`, `Tail`
- mutation: `Push(records,index)`, `Pop(records,output)`, `Remove(records,index)`

See the individual reference pages from [Reference Index](Reference-Index) for full contracts and private implementation context.
