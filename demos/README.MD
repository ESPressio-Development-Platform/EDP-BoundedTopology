# EDP-BoundedTopology Demos

The `BoundedTopology` logical demo exercises all four public facilities together:

- strong bounded index identity;
- one-bit-per-index membership state;
- stable lowest-free slot acquisition and explicit release;
- non-owning intrusive FIFO ordering over externally owned records.

The demo acquires the lowest free stable slot, marks that slot active in a separate membership set, enqueues its externally owned record, then pops the record, clears the independent membership fact, releases the stable slot, and validates the final state.

## Target matrix

```text
demos/BoundedTopology/
├── Arduino_IDE
├── PlatformIO_Arduino
└── PlatformIO_ESP-IDF
```

All targets require C++20. The PlatformIO-compatible Arduino target uses PIOArduino as recommended by ESPressio.

The demos introduce no additional ESPressio-library dependency.
