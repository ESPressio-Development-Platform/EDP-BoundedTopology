# Architecture

## Architectural role

EDP-BoundedTopology is a dependency-minimal foundation below higher runtime domains such as Memory, Threading, Command, Event, Radio/Mesh or future endpoint/resource managers when they need the same finite-index mechanics.

The architectural split is **value ownership versus non-owning topology**. `EDP-BoundedTypes` owns value/container storage. `EDP-Memory` owns object-pool allocation/lifetime semantics. EDP-BoundedTopology owns only finite identity, membership, stable occupancy-slot lifecycle and FIFO relationship state.

## Primitive relationship

```text
BoundedIndex
    ├── BoundedIndexSet
    │   └── BoundedSlotTopology
    └── IntrusiveQueue
```

`BoundedIndexSet`, `BoundedSlotTopology` and `IntrusiveQueue` use exactly the same strong index Type for a given semantic space/capacity. `BoundedSlotTopology` composes `BoundedIndexSet` and adds no persistent state beyond its occupancy bits.

## Pool decision

There is intentionally no state-owning generic `Pool`. A fixed Worker population may require only one availability set, whereas a runtime endpoint population may require several independent state sets. The owning domain pays only for the state it actually needs.

## Stable slot ownership

`BoundedSlotTopology` owns occupancy only. It deterministically acquires the numerically lowest free slot and keeps that slot stable until explicit release. Payload storage/lifetime, generation/stale-reference protection and transaction ordering remain external.

This is deliberately not a generic state-owning Pool. The first identified higher-domain consumer is `EDP-RemoteSystem::DeviceRegistry`, which can use a stable slot as a compact registration-lifetime foreign key while retaining its own Device and facet semantics.

## Queue ownership

`IntrusiveQueue` stores only head/tail. The external record stores `QueueNext()`. This permits a domain to reuse a record field with another mutually exclusive role, as EDP-Threading currently does with queue-link versus execution-context scratch.

## Synchronization boundary

All primitives are synchronization-free. The owning domain supplies the serialization boundary required for shared mutation.

## Evolution rule

New primitives belong here only after cross-domain evidence demonstrates a common representation/contract. Similar storage techniques alone are insufficient; for example, Security replay bitmaps and System `FlagSet` remain semantically distinct.
