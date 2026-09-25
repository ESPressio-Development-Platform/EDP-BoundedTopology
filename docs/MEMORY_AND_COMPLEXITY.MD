# Memory and Complexity

## Memory-first rule

EDP-BoundedTopology follows this architectural constraint:

> Persistent derived or cached state is not retained merely to make a bounded query cheaper. Every retained byte must be justified by essential semantics or a materially important bounded-time property.

This is stricter than merely avoiding heap allocation.

## BoundedIndex footprint

`BoundedIndex<TIndexSpace, N>` contains one scalar only. The semantic tag does not exist at runtime.

The storage width is the minimum able to represent all N valid values plus the Invalid sentinel:

| Capacity N | Storage |
| ---: | ---: |
| 0..255 | 1 byte |
| 256..65,535 | 2 bytes |
| 65,536..4,294,967,295 | 4 bytes |
| larger representable target capacities | 8 bytes |

For N = 255, valid indices are 0..254 and 255 is the uint8 Invalid sentinel. N = 256 therefore requires uint16 storage.

## BoundedIndexSet footprint

For positive capacity:

```text
StorageBytes = ceil(N / 8)
sizeof(BoundedIndexSet<TSpace, N>) == StorageBytes
```

The host tests explicitly enforce representative size boundaries.

The zero-capacity specialization contains no state. C++ complete objects cannot have `sizeof(T) == 0`, but `[[no_unique_address]]` permits the empty specialization to consume no additional retained bytes when embedded in an owning facility. The host tests validate that composition property.

## IntrusiveQueue footprint

For positive capacity:

```text
sizeof(IntrusiveQueue<TSpace, N>)
    = 2 × sizeof(BoundedIndex<TSpace, N>)
```

The two retained values are head and tail. There is no queue size, empty flag, link array, record pointer, storage pointer, capacity field, or synchronization member.

The zero-capacity specialization contains no state and is validated with `[[no_unique_address]]` composition.

## Why tail is retained

A singly linked intrusive FIFO could theoretically retain only the head. That would save one compact index but would require walking up to N links for every append.

Keeping tail makes append O(1) while costing only 1 byte when N <= 255, 2 bytes when N <= 65,535, or 4 bytes when N <= 4,294,967,295.

This is considered essential topology state rather than a convenience cache.

## Operation complexity

### BoundedIndex

| Operation | Complexity | Persistent extra state |
| --- | ---: | ---: |
| validity | O(1) | none |
| checked creation | O(1) | none |
| equality | O(1) | none |

### BoundedIndexSet

| Operation | Complexity | Persistent extra state |
| --- | ---: | ---: |
| Set/Clear/IsSet | O(1) | none |
| ClearAll/SetAll | O(ceil(N/8)) | none |
| IsEmpty/IsAnySet | O(ceil(N/8)) | none |
| Count | O(N) | no cached count |
| FindFirstSet | O(N) | no cached first index |
| FindFirstClear | O(N) | no cached first index |

### IntrusiveQueue

| Operation | Complexity | Persistent extra state |
| --- | ---: | ---: |
| IsEmpty/Head/Tail | O(1) | none |
| Push | O(1) | tail is authoritative topology state |
| Pop | O(1) | none |
| Remove specific | O(N) bounded | no previous-link table |

## Allocation and ownership

No primitive performs dynamic allocation.

`BoundedIndexSet` owns only its bitmap bytes. `IntrusiveQueue` owns only queue endpoints. Record linkage and record lifetime remain external.

The library never constructs, destroys, relocates, retains, or releases represented payload objects.

## Synchronization cost

There is no internal mutex, atomic, semaphore, signal, spin lock, provider object or provider pointer. A consuming domain pays only for the synchronization model it actually needs.
