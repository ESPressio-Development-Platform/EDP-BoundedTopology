# EDP-BoundedTopology Demos

The `BoundedTopology` logical demo exercises all three public facilities together:

- strong bounded index identity;
- one-bit-per-index membership state;
- non-owning intrusive FIFO ordering over externally owned records.

The demo claims the lowest available slot, removes that slot from the availability set, enqueues its externally owned record, then pops the record and validates the final state.

## Target matrix

```text
demos/BoundedTopology/
├── Arduino_IDE
├── PlatformIO_Arduino
└── PlatformIO_ESP-IDF
```

All targets require C++20. The PlatformIO-compatible Arduino target uses PIOArduino as recommended by ESPressio.

The demos introduce no additional ESPressio-library dependency.
