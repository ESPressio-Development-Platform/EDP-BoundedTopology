# Internal API

EDP-BoundedTopology currently defines **no supported internal cross-repository API distinct from its public API**.

There is no `Detail` contract intended for direct use by another repository. All non-public aliases, members and helper functions are private implementation and may change provided the public contract, exact memory guarantees, and policy requirements remain satisfied.

If a future consumer requires a currently private helper as a contractual cross-repository surface, that relationship must be made explicit and documented here before relying on it.
