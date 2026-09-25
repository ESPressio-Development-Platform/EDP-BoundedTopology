# Compiler Definitions

EDP-BoundedTopology defines **no production compiler definition or conditional-compilation switch**.

The package metadata enforces/requests C++20 by removing older GNU language modes and adding `-std=gnu++20` where PlatformIO-compatible metadata is consumed.

The production headers do not branch on Arduino, ESP-IDF, FreeRTOS, architecture, compiler vendor, or SDK macros.

Test/demo build-system settings are build configuration rather than EDP-owned runtime compiler switches.
