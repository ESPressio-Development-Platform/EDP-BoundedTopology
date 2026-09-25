# Architecture

## Architectural role

EDP-BoundedTopology is a dependency-minimal foundation below higher runtime domains such as Memory, Threading, Command, Event, Radio/Mesh or future endpoint/resource managers when they need the same finite-index mechanics.

The architectural split is **value ownership versus non-owning topology**. `EDP-BoundedTypes` owns value/container storage. `EDP-Memory` owns object-pool allocation/lifetime semantics. EDP-BoundedTopology owns only finite identity, membership and FIFO relationship state.

## Primitive relationship

```text
BoundedIndex
    ├── BoundedIndexSet
    └── IntrusiveQueue
```

`BoundedIndexSet` and `IntrusiveQueue` use exactly the same strong index Type for a given semantic space/capacity.

## Pool decision

There is intentionally no state-owning generic `Pool`. A fixed Worker population may require only one availability set, whereas a runtime endpoint population may require several independent state sets. The owning domain pays only for the state it actually needs.

## Queue ownership

`IntrusiveQueue` stores only head/tail. The external record stores `QueueNext()`. This permits a domain to reuse a record field with another mutually exclusive role, as EDP-Threading currently does with queue-link versus execution-context scratch.

## Synchronization boundary

All primitives are synchronization-free. The owning domain supplies the serialization boundary required for shared mutation.

## Evolution rule

New primitives belong here only after cross-domain evidence demonstrates a common representation/contract. Similar storage techniques alone are insufficient; for example, Security replay bitmaps and System `FlagSet` remain semantically distinct.
