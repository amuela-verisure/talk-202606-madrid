# The price of runtime: Modern C++ Design with C++20

Companion repository for the talk *"The price of runtime: Modern C++ Design
for Systems That Can't Afford Surprises"*.

Nine side-by-side demo pairs compare **classic** C++ design (virtual
polymorphism, heap allocation, exceptions as control flow, raw loops) against
**modern** C++20 design (concepts, `std::variant` + `std::visit`,
`tl::expected`, ranges, compile-time computation, stack/fixed-capacity
storage). Equivalence tests prove both versions of each demo produce identical
observable results. Every number in the talk is reproducible from this repo.

**Polyfill libraries:** `tl::expected` (`std::expected`, C++23), `range-v3`
(`std::ranges`, C++20/23)

**Modern-target flags:** `-fno-exceptions -fno-rtti` (the classic targets keep
the language defaults; `demo04_classic` explicitly requires `-fexceptions`)

**Firmware flags:** `-Os -flto -DNDEBUG`, ARM Cortex-M4 (QEMU mps2-an386)

---

## The Demos

| #  | Directory             | Classic design                          | Modern design                                  |
|----|-----------------------|------------------------------------------|------------------------------------------------|
| 01 | `demo01_shapes`       | virtual hierarchy, `vector<unique_ptr>`  | `std::variant` + `std::visit`, `std::array`    |
| 02 | `demo02_concepts`     | abstract interface class, vtable calls   | `concept` contract, static dispatch, `constexpr` |
| 03 | `demo03_soa`          | evolution: `vector<unique_ptr<Base>>` → AoS | Struct-of-Arrays, fixed compile-time budget (the further step, where it makes sense) |
| 04 | `demo04_expected`     | exceptions as control flow               | `tl::expected`, monadic chain, errors as values |
| 05 | `demo05_compile_time` | runtime CRC table init in `.bss` + flag  | `consteval` table in flash, `constinit` state  |
| 06 | `demo06_bitcast`      | `memcpy` + runtime `assert`              | `std::bit_cast` + `static_assert` layout contract |
| 07 | `demo07_ranges`       | index loops, intermediate heap vectors   | range-v3 views pipeline + ranges algorithms    |
| 08 | `demo08_stack`        | `std::vector` event log (heap, realloc; `reserve` variant included) | `FixedVector` on `std::array` ([C++26] `inplace_vector`) |
| 09 | `demo09_observers`    | `std::function` observer list            | variadic pack + fold expression, zero erasure  |

Each demo directory follows the same layout:

```
demoNN_topic/
  classic/include/demoNN/classic/*.hpp   classic implementation
  classic/src/*.cpp
  modern/include/demoNN/modern/*.hpp     modern implementation
  modern/src/*.cpp
  test/test_demoNN_equivalence.cpp       doctest equivalence test
```

---

## Measured Metrics

All numbers below were measured with GCC 13.3 (host x86-64) and
arm-none-eabi-gcc 13.2.1 (ARM). Reproduce with the commands in each section.

### Full firmware (ARM, `-Os -flto -DNDEBUG`)

`scripts/compare_size.sh build/arm`:

| Section            | classic | modern | delta     |
|--------------------|--------:|-------:|-----------|
| `.text` (flash)    | 19248 B | 6140 B | **−68%**  |
| `.data`            |   120 B |  104 B | −16 B     |
| `.bss` (RAM)       |  1440 B | 5240 B | +3800 B (see note) |

Note on `.bss`: the modern firmware *declares* its memory statically: the SoA
particle pool (4356 B) and the fixed event log (520 B) live in `.bss` and are
visible in the linker map. The classic firmware allocates the equivalent data
on the **heap at runtime** (`malloc` is linked into the classic image only):
same memory, hidden from the linker.

Exception machinery (`readelf -S`, `nm`):

| Check                          | classic           | modern |
|--------------------------------|-------------------|--------|
| `.ARM.extab` (unwind data)     | 128 B             | none   |
| `.ARM.exidx` (unwind index)    | 232 B             | 8 B (sentinel) |
| `_Unwind* / __cxa*` symbols    | 46                | **0**  |
| `malloc` linked                | yes               | **no** |

Both firmwares run to completion on QEMU (`mps2-an386`) and print
`[classic|modern firmware] start/done` via semihosting.

### Per-demo translation units (ARM, firmware profile)

`scripts/measure_demos.sh` (flags: `-Os -DNDEBUG
-fno-asynchronous-unwind-tables`, modern adds `-fno-exceptions -fno-rtti`,
demo04 classic adds `-fexceptions`):

| Demo | `.text` classic | `.text` modern | delta | `.bss` classic | `.bss` modern |
|------|----------------:|---------------:|-------|---------------:|--------------:|
| 01 shapes      | 1320 | 232  | **−82%** | 0    | 0 |
| 02 concepts    | 400  | 128  | **−68%** | 0    | 0 |
| 03 SoA         | 365  | 276  | **−24%** | 0    | 0 |
| 04 expected    | 993  | 252  | **−75%** | 0    | 0 |
| 05 consteval   | 136  | 1088 | table → flash | 1025 | **8** |
| 06 bit_cast    | 90   | 92   | parity   | 0    | 0 |
| 07 ranges      | 1078 | 558  | **−48%** | 0    | 0 |
| 08 fixed vec   | 40   | 38   | parity   | 0    | 0 |

Demo 03's classic column is its AoS stage; the stage-1
`vector<unique_ptr<IParticle>>` TU compiles to 945 B (see the storage
evolution table below).

Demo 05 trades 1 KB of RAM (`.bss` 1025 → 8) for 1 KB of flash (`.rodata`
table) and deletes the boot-time init entirely; on RAM-constrained parts
that is the winning direction.

Symbol-level checks (demo 01, `nm` on the ARM object files):

- classic: `_Znwj` (operator new), `_ZdlPvj` (operator delete), 3 shape
  vtables + 2 `__cxxabiv1` type_info vtables
- modern: **zero** heap, vtable, or RTTI symbols

### Exceptions vs expected, hot path (host, `-Os -flto`)

`./build/host-release/bench_expected`: 10000-frame telemetry stream,
ns/frame:

| corrupted frames | exceptions | `tl::expected` | ratio    |
|------------------|-----------:|---------------:|----------|
| 0%               | 2.2        | 1.7            | 1.3×     |
| 1%               | 9.4        | 1.4            | 6.6×     |
| 10%              | 57.4       | 1.2            | **47×**  |
| 25%              | 126.6      | 1.2            | **103×** |
| 50%              | 429.4      | 2.3            | **185×** |

The `expected` error path costs the same as its happy path; the exception
error path is a stack unwind with unbounded latency.

### Storage evolution, particle update (host)

`./build/host-release/bench_soa`: 4096 particles × 50000 iterations. The
demo measures the *evolution*, not a contest: start from the classic
`vector<unique_ptr<Base>>`, move to values (AoS), then to SoA where the loop
justifies it. Speedups vs stage 1, across repeated runs:

| stage                                | `-Os -flto` | `-O3 -march=native` | ARM `.text` |
|--------------------------------------|-------------|----------------------|------------:|
| 1: `vector<unique_ptr<IParticle>>`   | 1×          | 1×                   | 945 B |
| 2: `vector<Particle>` (AoS)          | **~2×**     | **~2×**              | 365 B |
| 3: Struct-of-Arrays (SoA)            | ~2× (≈ AoS, noise-bound) | **4.7–4.9×** | 276 B |

Step 1→2 (values instead of heap pointers) pays unconditionally. Step 2→3
pays where the vectorizer or the cache is in play: the SoA layout is what
enables auto-vectorization at `-O3`; at `-Os` (no vectorizer) it is within
noise of AoS. Reproduce the `-O3` column with:

```sh
g++ -std=c++20 -O3 -DNDEBUG -march=native bench/bench_soa.cpp \
  demo03_soa/classic/src/particles.cpp demo03_soa/classic/src/particles_ptr.cpp \
  demo03_soa/modern/src/particles.cpp \
  -Idemo03_soa/classic/include -Idemo03_soa/modern/include -o /tmp/bench && /tmp/bench
```

### Cyclomatic complexity (lizard)

| Function                        | classic CCN | modern CCN |
|---------------------------------|------------:|-----------:|
| demo04 `parse_frame`            | 6           | **1** (monadic chain) |
| demo04 file average             | 3.7         | 2.0        |
| demo07 `process_readings`       | 8           | 6          |
| demo07 `compute_stats`          | 7           | **2**      |
| demo07 file average             | 7.5         | 2.8        |

### Compile time (host, `-O2`, warm cache, single TU)

| Demo | classic | modern | note |
|------|--------:|-------:|------|
| 01 shapes   | 276 ms | **82 ms** | `<memory>` + `<vector>` outweigh `<variant>` |
| 04 expected | 148 ms | 167 ms    | `tl::expected` ≈ exceptions       |
| 07 ranges   | 239 ms | 451 ms    | range-v3 headers; the honest tax |

---

## Build Instructions

### Prerequisites

- GCC 12+ (or Clang 16+) with C++20 support
- CMake 3.24+, Ninja
- arm-none-eabi-gcc 13.2+ with newlib + libstdc++ (ARM cross-compilation)
- qemu-system-arm (run the firmware)
- Python 3 + `lizard` (complexity metrics)

Or use the Dockerfile (recommended; it builds everything and runs all tests and
both firmwares as part of the image build):

```sh
docker build -t cpp-design-talk .
docker run --rm -it cpp-design-talk
```

Helper scripts: `scripts/docker-build.sh`, `scripts/docker-run.sh`.

### Host build + tests

```sh
cmake --preset host-debug
cmake --build --preset host-debug
ctest --preset host-debug          # 9/9 equivalence tests
```

### Host release (benchmarks)

```sh
cmake --preset host-release
cmake --build --preset host-release
./build/host-release/bench_expected
./build/host-release/bench_soa
```

### ARM cross-compilation + QEMU

```sh
cmake --preset arm-cortexm4
cmake --build --preset arm-cortexm4
scripts/compare_size.sh build/arm
scripts/measure_demos.sh

qemu-system-arm -machine mps2-an386 -nographic -semihosting \
    -kernel build/arm/firmware_modern    # prints start/done, exits cleanly
qemu-system-arm -machine mps2-an386 -nographic -semihosting \
    -kernel build/arm/firmware_classic
```

---

## Project Structure

```
.
├── CMakeLists.txt               Root build (all demos, tests, firmware)
├── CMakePresets.json            host-debug, host-release, arm-cortexm4
├── Dockerfile                   Complete build + test + QEMU environment
├── demo01_shapes/ … demo09_observers/   The nine demo pairs
├── bench/                       bench_soa, bench_expected (host only)
├── app/                         main_classic.cpp, main_modern.cpp (firmware)
├── startup/                     Cortex-M4 startup, semihosting syscalls
├── linker/                      MPS2-AN386 linker script (256K flash / 64K RAM)
├── cmake/                       Toolchain, warnings, FetchContent deps
├── scripts/                     compare_size.sh, measure_demos.sh, docker-*.sh
└── slides/                      talk.org (org-re-reveal), reveal.js submodule,
                                 verisure theme
```

## Slides

`slides/talk.org` is an [org-re-reveal](https://gitlab.com/oer/org-re-reveal)
deck; `slides/talk.html` is the exported presentation (open in a browser;
reveal.js is vendored as a git submodule):

```sh
git submodule update --init       # fetch reveal.js (pinned to 6.0.1)
```

## References

- N. Josuttis: *Rethink Polymorphism in C++*, C++ on Sea 2025
- V. Romeo: *More Speed and Simplicity*, CppCon 2025
- K. Estell: *C++ Exceptions for Smaller Firmware*, CppCon 2024 keynote
- H. Sutter: P0709 *Zero-overhead deterministic exceptions*
- P0843 `std::inplace_vector` [C++26], `std::expected` [C++23]

## License

GPL-3.0; see [LICENSE](LICENSE).
