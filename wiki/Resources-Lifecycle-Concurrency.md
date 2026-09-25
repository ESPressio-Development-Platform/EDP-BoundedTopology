# Resources, Lifecycle and Concurrency

## Retained memory

`BoundedIndex` stores one capacity-derived scalar. `BoundedIndexSet` positive capacity stores exactly `ceil(N/8)` bytes. `IntrusiveQueue` positive capacity stores exactly two bounded indices.

No population/queue count is cached.

## Zero capacity

Zero-capacity set/queue specializations contain no members. A complete empty C++ object still has non-zero `sizeof`, but an owner can use `[[no_unique_address]]` so the empty topology contributes no additional retained bytes. Host tests enforce this composition property.

## Lifecycle

The primitives require no initialization/teardown phase. Construction establishes Invalid/empty state. They own no external resource requiring release.

## Allocation

No dynamic allocation occurs.

## Concurrency

No internal synchronization exists. The owning domain must serialize concurrent mutation or read/write access to one mutable topology object.

## ISR

No general ISR-safety guarantee is made. Local operations do not allocate or block, but ISR suitability depends on the owning domain's synchronization and the applicable ESPressio ISR policy.

## Complexity

`Set/Clear/IsSet`, queue Push/Pop and basic index operations are O(1). Set scans/count and specific queue removal are bounded O(N). The bounded scans are intentional RAM-saving tradeoffs.
