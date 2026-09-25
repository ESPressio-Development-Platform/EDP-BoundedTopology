# Composition and Providers

EDP-BoundedTopology intentionally uses **no ESPressio Composition Framework facilities**.

There is:

- no Composition Domain;
- no Capability;
- no provider Type;
- no Offer/Property/Attribute;
- no Requirement/cardinality;
- no provider selection policy;
- no Bootstrap-owned runtime provider object.

This absence is architectural. The primitives are ordinary header-only value/topology Types whose storage is completely determined by template parameters. Adding a provider/Composition dependency would impose unnecessary coupling and potentially retained runtime state.
