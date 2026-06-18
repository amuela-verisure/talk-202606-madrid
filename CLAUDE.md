# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Side-by-side classic vs modern C++20 demo code for the talk "The Price of Run
Time: Modern C++ Design for Systems That Can't Afford Surprises". Each demo
pairs a *classic* design (virtual polymorphism, heap, exceptions as control
flow, raw loops) with a *modern* C++20 design (concepts, variant+visit,
tl::expected, ranges, consteval, fixed-capacity stack storage) and proves
equivalence with doctest. This is presentation/talk material, not a library.

## Build Commands

```bash
# Configure + build (host, for tests and development)
cmake --preset host-debug
cmake --build build/host-debug -j$(nproc)

# Run all tests
ctest --test-dir build/host-debug --output-on-failure

# Run a single test
ctest --test-dir build/host-debug -R demo04_equivalence

# Host release build (benchmarks)
cmake --preset host-release
cmake --build build/host-release -j$(nproc)
./build/host-release/bench_expected
./build/host-release/bench_soa

# ARM cross-compile (requires arm-none-eabi-gcc)
cmake --preset arm-cortexm4
cmake --build build/arm -j$(nproc)

# ARM firmware size comparison + per-demo TU sizes
scripts/compare_size.sh build/arm
scripts/measure_demos.sh

# Run firmware on QEMU (prints via semihosting, exits cleanly)
qemu-system-arm -machine mps2-an386 -nographic -semihosting \
    -kernel build/arm/firmware_modern

# Docker (full build + tests + cross-compile + QEMU run)
scripts/docker-build.sh
scripts/docker-run.sh
```

## Architecture

Each demo (`demo01_shapes/` through `demo09_observers/`) has identical structure:
- `classic/include/demoNN/classic/*.hpp` + `classic/src/*.cpp` -- classic design
- `modern/include/demoNN/modern/*.hpp` + `modern/src/*.cpp` -- modern C++20 design
- `test/test_demoNN_equivalence.cpp` -- doctest equivalence test proving both
  produce identical results

Tests link both `demoNN_classic` and `demoNN_modern` static libraries
(demo09 targets are INTERFACE, header-only). Test targets are `test_demoNN`
with CTest name `demoNN_equivalence`. Namespaces: `demoNN::classic`,
`demoNN::modern`.

`bench/` -- host-only micro-benchmarks (`bench_soa`, `bench_expected`).
`app/` -- bare-metal `main_classic.cpp` / `main_modern.cpp` entry points (ARM
cross-compile only). `startup/` + `linker/` -- Cortex-M4 startup (FPU enable,
data/bss init, `__libc_init_array`, semihosting `_write`/`_exit`) and linker
script for QEMU mps2-an386.

## Standards and Constraints

- C++20 minimum (`CMAKE_CXX_STANDARD 20`)
- Every **modern** target builds with `-fno-exceptions -fno-rtti`
  (`MODERN_FLAGS` in CMakeLists.txt); classic targets keep language defaults
- `demo04_classic` requires `-fexceptions` (set explicitly — its design point)
- No heap in any modern demo (verified: zero `malloc`/`operator new` symbols)
- Warnings as errors in all presets (`WARNINGS_AS_ERRORS=ON`)
- Polyfills via FetchContent: `tl::expected` (C++23 `std::expected`),
  `range-v3` (C++23 `std::ranges`); doctest for host tests
- Formatting: `.clang-format` (LLVM-based, 4-space indent, 100 col limit)

## Adding a New Demo

1. Create `demoNN_topic/{classic,modern,test}/` following the existing layout
2. Add both libraries in `CMakeLists.txt` following the existing pattern
   (modern target gets `${MODERN_FLAGS}`)
3. Add the demo to the test `foreach` list at the bottom of CMakeLists.txt
4. Classic and modern implementations must produce identical observable
   results — the equivalence test must prove it
5. Wire the demo into both `app/main_classic.cpp` and `app/main_modern.cpp`

## Slides

`slides/talk.org` is the org-re-reveal source of truth; `slides/talk.html` is
its export (emacs batch: `(org-re-reveal-export-to-html)`). reveal.js is a git
submodule pinned to 6.0.1. The theme is `slides/verisure.css` (+ scss source),
loaded via `#+REVEAL_EXTRA_CSS` on top of the white theme. Keep measured
numbers in talk.org in sync with README.md — both must come from actual runs.
