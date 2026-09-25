# Build, Test and Source Navigation

## Production source

```text
src/
├── ESPressio_BoundedTopology.hpp
└── bounded_topology/
    ├── BoundedIndex.hpp
    ├── BoundedIndexSet.hpp
    └── IntrusiveQueue.hpp
```

## Tests

`tests/BoundedTopologyTests.cpp` provides compile-time footprint/type checks plus runtime FIFO/zero-capacity behavior tests.

`tests/run_tests.py` builds with C++20, warnings-as-errors and executes the host binary.

The initial implementation validation reconstructed the committed source exactly because the execution environment could not resolve GitHub directly. `/usr/bin/c++` host build/run completed with `EDP-BoundedTopology host tests: PASS`.

## Demos

`demos/BoundedTopology` provides Arduino IDE, PIOArduino/Arduino-ESP32 and PlatformIO-compatible ESP-IDF using PIOArduino surfaces. Target builds are not claimed as executed by the implementation environment because external platform packages could not be fetched there.

## Package metadata

`library.json` and `library.properties` both establish development version `0.1.0`.
