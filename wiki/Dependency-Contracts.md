# Dependency Contracts

## Production graph

```text
EDP-BoundedTopology
└── no ESPressio-library dependencies
```

## Standard-library dependency

Production headers use only C++20 standard facilities for `std::size_t`, fixed-width unsigned integers, numeric limits, and compile-time Type selection.

## Optional dependencies

None.

## Composition requirements

None. The library declares no Domain, Capability, Offer, Property, Attribute, Requirement, provider trait, or runtime provider reference.

## Consumers

Higher repositories may depend **toward** EDP-BoundedTopology. Such consumer relationships are not reverse dependencies. Candidate migrations must preserve exact semantics and memory footprint before they are adopted.

## Ownership/lifetime contract

No dependency supplies runtime state to this library. `IntrusiveQueue` receives externally owned record collections per operation; it does not retain a pointer/reference to those collections.
