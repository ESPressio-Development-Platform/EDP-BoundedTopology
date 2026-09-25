# Building and Validation

## C++ baseline

EDP-BoundedTopology requires C++20. The library is header-only and has no ESPressio-library dependencies.

## Host validation

From the repository root:

```bash
python3 tests/run_tests.py
```

The runner chooses `CXX` when supplied, otherwise the first available `c++`, `g++`, or `clang++`. It compiles with `-std=c++20 -Wall -Wextra -Wpedantic -Werror` and then executes the behavior/footprint suite.

## Arduino-ESP32

ESPressio recommends PIOArduino for Arduino-ESP32 work because it tracks a current Arduino-ESP32/toolchain baseline capable of C++20.

The supplied PlatformIO-compatible Arduino demo selects PIOArduino and explicitly removes older language-mode flags before enabling `-std=gnu++20`.

If conventional PlatformIO is retained instead, the project must select an Arduino-ESP32 release/toolchain that supports C++20 and apply the same language-mode correction.

## ESP-IDF

The supplied PlatformIO ESP-IDF demo explicitly enables C++20 and consumes the repository itself through `lib_extra_dirs`. A native ESP-IDF component may likewise include the repository `src` directory and compile C++ translation units as C++20.

## Arduino IDE

The repository contains standard `library.properties` metadata and exports:

```cpp
#include <ESPressio_BoundedTopology.hpp>
```

Install or place the repository as an Arduino library, then open the supplied Arduino IDE demo.

## Validation scope

The host tests are the current executable validation committed with the initial 0.1.0 baseline.

The target demo projects provide buildable integration surfaces for Arduino IDE, PlatformIO-compatible Arduino-ESP32 using PIOArduino, and PlatformIO ESP-IDF.

Any target build or physical-hardware execution results must be recorded only after they have actually been performed.
