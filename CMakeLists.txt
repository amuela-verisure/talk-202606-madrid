cmake_minimum_required(VERSION 3.24)
set(CMAKE_POLICY_DEFAULT_CMP0135 NEW)
project(cpp_design_talk LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Symlink compile_commands.json to project root (host builds only)
if(NOT CMAKE_CROSSCOMPILING AND CMAKE_EXPORT_COMPILE_COMMANDS)
    file(RELATIVE_PATH REL_BUILD_DIR ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            ${REL_BUILD_DIR}/compile_commands.json
            ${CMAKE_SOURCE_DIR}/compile_commands.json
    )
endif()

# ---------- Modules ----------
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(CompilerWarnings)
include(FetchDeps)

# Every *modern* target builds with the embedded discipline flags: no
# exception machinery, no RTTI. Classic targets keep the language defaults
# (and demo04_classic explicitly REQUIRES exceptions — that is its point).
set(MODERN_FLAGS -fno-exceptions -fno-rtti)

# =============================================================================
# Demo 1: Shapes — virtual hierarchy + heap (classic) vs variant + visit (modern)
# =============================================================================

add_library(demo01_classic STATIC demo01_shapes/classic/src/shapes.cpp)
target_include_directories(demo01_classic PUBLIC demo01_shapes/classic/include)
target_compile_features(demo01_classic PUBLIC cxx_std_20)
set_project_warnings(demo01_classic)

add_library(demo01_modern STATIC demo01_shapes/modern/src/shapes.cpp)
target_include_directories(demo01_modern PUBLIC demo01_shapes/modern/include)
target_compile_features(demo01_modern PUBLIC cxx_std_20)
target_compile_options(demo01_modern PRIVATE ${MODERN_FLAGS})
set_project_warnings(demo01_modern)

# =============================================================================
# Demo 2: Filters — abstract interface (classic) vs concepts (modern)
# =============================================================================

add_library(demo02_classic STATIC demo02_concepts/classic/src/filters.cpp)
target_include_directories(demo02_classic PUBLIC demo02_concepts/classic/include)
target_compile_features(demo02_classic PUBLIC cxx_std_20)
set_project_warnings(demo02_classic)

add_library(demo02_modern STATIC demo02_concepts/modern/src/filters.cpp)
target_include_directories(demo02_modern PUBLIC demo02_concepts/modern/include)
target_compile_features(demo02_modern PUBLIC cxx_std_20)
target_compile_options(demo02_modern PRIVATE ${MODERN_FLAGS})
set_project_warnings(demo02_modern)

# =============================================================================
# Demo 3: Particles — storage evolution: vector<unique_ptr<Base>> → AoS → SoA
# =============================================================================

add_library(demo03_classic STATIC
    demo03_soa/classic/src/particles.cpp
    demo03_soa/classic/src/particles_ptr.cpp)
target_include_directories(demo03_classic PUBLIC demo03_soa/classic/include)
target_compile_features(demo03_classic PUBLIC cxx_std_20)
set_project_warnings(demo03_classic)

add_library(demo03_modern STATIC demo03_soa/modern/src/particles.cpp)
target_include_directories(demo03_modern PUBLIC demo03_soa/modern/include)
target_compile_features(demo03_modern PUBLIC cxx_std_20)
target_compile_options(demo03_modern PRIVATE ${MODERN_FLAGS})
set_project_warnings(demo03_modern)

# =============================================================================
# Demo 4: Telemetry — exceptions as control flow (classic) vs tl::expected (modern)
# =============================================================================

add_library(demo04_classic STATIC demo04_expected/classic/src/telemetry.cpp)
target_include_directories(demo04_classic PUBLIC demo04_expected/classic/include)
target_compile_features(demo04_classic PUBLIC cxx_std_20)
# The classic design REQUIRES the exception machinery (relevant when
# cross-compiling: the toolchain default is -fno-exceptions).
target_compile_options(demo04_classic PRIVATE -fexceptions)
set_project_warnings(demo04_classic)

add_library(demo04_modern STATIC demo04_expected/modern/src/telemetry.cpp)
target_include_directories(demo04_modern PUBLIC demo04_expected/modern/include)
target_link_libraries(demo04_modern PUBLIC tl::expected)
target_compile_features(demo04_modern PUBLIC cxx_std_20)
target_compile_options(demo04_modern PRIVATE ${MODERN_FLAGS})
set_project_warnings(demo04_modern)

# =============================================================================
# Demo 5: CRC-32 — runtime table init (classic) vs consteval + constinit (modern)
# =============================================================================

add_library(demo05_classic STATIC demo05_compile_time/classic/src/crc32.cpp)
target_include_directories(demo05_classic PUBLIC demo05_compile_time/classic/include)
target_compile_features(demo05_classic PUBLIC cxx_std_20)
set_project_warnings(demo05_classic)

add_library(demo05_modern STATIC demo05_compile_time/modern/src/crc32.cpp)
target_include_directories(demo05_modern PUBLIC demo05_compile_time/modern/include)
target_compile_features(demo05_modern PUBLIC cxx_std_20)
target_compile_options(demo05_modern PRIVATE ${MODERN_FLAGS})
set_project_warnings(demo05_modern)

# =============================================================================
# Demo 6: Packets — memcpy + runtime assert (classic) vs bit_cast + static_assert (modern)
# =============================================================================

add_library(demo06_classic STATIC demo06_bitcast/classic/src/packet.cpp)
target_include_directories(demo06_classic PUBLIC demo06_bitcast/classic/include)
target_compile_features(demo06_classic PUBLIC cxx_std_20)
set_project_warnings(demo06_classic)

add_library(demo06_modern STATIC demo06_bitcast/modern/src/packet.cpp)
target_include_directories(demo06_modern PUBLIC demo06_bitcast/modern/include)
target_compile_features(demo06_modern PUBLIC cxx_std_20)
target_compile_options(demo06_modern PRIVATE ${MODERN_FLAGS})
set_project_warnings(demo06_modern)

# =============================================================================
# Demo 7: Pipeline — index loops + heap buffers (classic) vs ranges (modern)
# =============================================================================

add_library(demo07_classic STATIC demo07_ranges/classic/src/pipeline.cpp)
target_include_directories(demo07_classic PUBLIC demo07_ranges/classic/include)
target_compile_features(demo07_classic PUBLIC cxx_std_20)
set_project_warnings(demo07_classic)

add_library(demo07_modern STATIC demo07_ranges/modern/src/pipeline.cpp)
target_include_directories(demo07_modern PUBLIC demo07_ranges/modern/include)
target_link_libraries(demo07_modern PUBLIC range-v3)
target_compile_features(demo07_modern PUBLIC cxx_std_20)
target_compile_options(demo07_modern PRIVATE ${MODERN_FLAGS})
set_project_warnings(demo07_modern)

# =============================================================================
# Demo 8: Event log — std::vector heap (classic) vs fixed-capacity stack (modern)
# =============================================================================

add_library(demo08_classic STATIC demo08_stack/classic/src/event_log.cpp)
target_include_directories(demo08_classic PUBLIC demo08_stack/classic/include)
target_compile_features(demo08_classic PUBLIC cxx_std_20)
set_project_warnings(demo08_classic)

add_library(demo08_modern STATIC demo08_stack/modern/src/event_log.cpp)
target_include_directories(demo08_modern PUBLIC demo08_stack/modern/include)
target_compile_features(demo08_modern PUBLIC cxx_std_20)
target_compile_options(demo08_modern PRIVATE ${MODERN_FLAGS})
set_project_warnings(demo08_modern)

# =============================================================================
# Demo 9: Observers — std::function vector (classic) vs variadic pack (modern)
# =============================================================================

add_library(demo09_classic INTERFACE)
target_include_directories(demo09_classic INTERFACE demo09_observers/classic/include)
target_compile_features(demo09_classic INTERFACE cxx_std_20)

add_library(demo09_modern INTERFACE)
target_include_directories(demo09_modern INTERFACE demo09_observers/modern/include)
target_compile_features(demo09_modern INTERFACE cxx_std_20)

# =============================================================================
# Benchmarks (host only)
# =============================================================================

if(NOT CMAKE_CROSSCOMPILING)
    add_executable(bench_soa bench/bench_soa.cpp)
    target_link_libraries(bench_soa PRIVATE demo03_classic demo03_modern)
    target_compile_features(bench_soa PRIVATE cxx_std_20)
    set_project_warnings(bench_soa)

    add_executable(bench_expected bench/bench_expected.cpp)
    target_link_libraries(bench_expected PRIVATE demo04_classic demo04_modern)
    target_compile_features(bench_expected PRIVATE cxx_std_20)
    set_project_warnings(bench_expected)
endif()

# =============================================================================
# ARM firmware targets (cross-compile only)
# =============================================================================

if(CMAKE_CROSSCOMPILING)
    set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/linker/MPS2_AN386.ld")

    # Classic firmware — virtual dispatch, heap, exceptions (demo 4)
    add_executable(firmware_classic
        app/main_classic.cpp
        startup/startup_mps2.cpp
        startup/syscalls.cpp
    )
    target_link_libraries(firmware_classic PRIVATE
        demo01_classic demo02_classic demo03_classic demo04_classic
        demo05_classic demo06_classic demo07_classic demo08_classic
        demo09_classic)
    target_compile_options(firmware_classic PRIVATE -fexceptions)
    target_link_options(firmware_classic PRIVATE
        -fexceptions -T${LINKER_SCRIPT} -Wl,-Map=firmware_classic.map)

    # Modern firmware — static dispatch, stack, expected; no unwinder at all
    add_executable(firmware_modern
        app/main_modern.cpp
        startup/startup_mps2.cpp
        startup/syscalls.cpp
    )
    target_link_libraries(firmware_modern PRIVATE
        demo01_modern demo02_modern demo03_modern demo04_modern
        demo05_modern demo06_modern demo07_modern demo08_modern
        demo09_modern)
    target_link_options(firmware_modern PRIVATE
        -T${LINKER_SCRIPT} -Wl,-Map=firmware_modern.map)

    foreach(target firmware_classic firmware_modern)
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${target}>
                    ${target}.bin
            COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${target}>
            COMMENT "Size report for ${target}")
    endforeach()
endif()

# =============================================================================
# Tests (host only)
# =============================================================================

if(BUILD_TESTING AND NOT CMAKE_CROSSCOMPILING)
    enable_testing()

    foreach(demo IN ITEMS
            demo01:demo01_shapes
            demo02:demo02_concepts
            demo03:demo03_soa
            demo04:demo04_expected
            demo05:demo05_compile_time
            demo06:demo06_bitcast
            demo07:demo07_ranges
            demo08:demo08_stack
            demo09:demo09_observers)
        string(REPLACE ":" ";" demo_parts ${demo})
        list(GET demo_parts 0 demo_name)
        list(GET demo_parts 1 demo_dir)

        add_executable(test_${demo_name}
            ${demo_dir}/test/test_${demo_name}_equivalence.cpp)
        target_link_libraries(test_${demo_name} PRIVATE
            ${demo_name}_classic ${demo_name}_modern doctest::doctest)
        target_compile_features(test_${demo_name} PRIVATE cxx_std_20)
        add_test(NAME ${demo_name}_equivalence COMMAND test_${demo_name})
    endforeach()
endif()
