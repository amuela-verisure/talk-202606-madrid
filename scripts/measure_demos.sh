#!/usr/bin/env bash
# Per-demo ARM size comparison (firmware profile: -Os -DNDEBUG
# -fno-asynchronous-unwind-tables). Modern TUs add -fno-exceptions -fno-rtti;
# demo04 classic adds -fexceptions (its design requires the machinery).
# Needs the host build for the FetchContent dependency headers:
#   cmake --preset host-debug
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
DEPS="${ROOT}/build/host-debug/_deps"

if [[ ! -d "${DEPS}/tl_expected-src" ]]; then
    echo "Run 'cmake --preset host-debug' first (dependency headers needed)."
    exit 1
fi

CPU="-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16"
PROFILE="-std=c++20 -Os -DNDEBUG -fno-asynchronous-unwind-tables"
MODERN="-fno-exceptions -fno-rtti"
INCLUDES="-isystem ${DEPS}/tl_expected-src/include -isystem ${DEPS}/range_v3-src/include"

declare -A SRC_CLASSIC=(
    [demo01]=demo01_shapes/classic/src/shapes.cpp
    [demo02]=demo02_concepts/classic/src/filters.cpp
    [demo03]=demo03_soa/classic/src/particles.cpp
    [demo04]=demo04_expected/classic/src/telemetry.cpp
    [demo05]=demo05_compile_time/classic/src/crc32.cpp
    [demo06]=demo06_bitcast/classic/src/packet.cpp
    [demo07]=demo07_ranges/classic/src/pipeline.cpp
    [demo08]=demo08_stack/classic/src/event_log.cpp
)
declare -A SRC_MODERN=(
    [demo01]=demo01_shapes/modern/src/shapes.cpp
    [demo02]=demo02_concepts/modern/src/filters.cpp
    [demo03]=demo03_soa/modern/src/particles.cpp
    [demo04]=demo04_expected/modern/src/telemetry.cpp
    [demo05]=demo05_compile_time/modern/src/crc32.cpp
    [demo06]=demo06_bitcast/modern/src/packet.cpp
    [demo07]=demo07_ranges/modern/src/pipeline.cpp
    [demo08]=demo08_stack/modern/src/event_log.cpp
)

inc_for() {
    local demo_dir=$1 variant=$2
    echo "-I${ROOT}/${demo_dir}/${variant}/include"
}

text_of() { arm-none-eabi-size "$1" | tail -1 | awk '{print $1}'; }
bss_of()  { arm-none-eabi-size "$1" | tail -1 | awk '{print $3}'; }

printf "%-8s %12s %12s %10s %10s\n" "demo" ".text C" ".text M" ".bss C" ".bss M"
for demo in demo01 demo02 demo03 demo04 demo05 demo06 demo07 demo08; do
    csrc="${ROOT}/${SRC_CLASSIC[$demo]}"
    msrc="${ROOT}/${SRC_MODERN[$demo]}"
    cdir="$(dirname "$(dirname "$(dirname "$csrc")")")"
    mdir="$(dirname "$(dirname "$(dirname "$msrc")")")"

    cflags="${PROFILE}"
    [[ "$demo" == "demo04" ]] && cflags="${PROFILE} -fexceptions"

    # shellcheck disable=SC2086
    arm-none-eabi-g++ ${CPU} ${cflags} -I"${cdir}/classic/include" \
        ${INCLUDES} -c "$csrc" -o /tmp/measure_c.o
    # shellcheck disable=SC2086
    arm-none-eabi-g++ ${CPU} ${PROFILE} ${MODERN} -I"${mdir}/modern/include" \
        ${INCLUDES} -c "$msrc" -o /tmp/measure_m.o

    printf "%-8s %12s %12s %10s %10s\n" "$demo" \
        "$(text_of /tmp/measure_c.o)" "$(text_of /tmp/measure_m.o)" \
        "$(bss_of /tmp/measure_c.o)" "$(bss_of /tmp/measure_m.o)"
done

echo ""
echo "demo03 storage evolution stages (.text per TU):"
# shellcheck disable=SC2086
arm-none-eabi-g++ ${CPU} ${PROFILE} -I"${ROOT}/demo03_soa/classic/include" \
    -c "${ROOT}/demo03_soa/classic/src/particles_ptr.cpp" -o /tmp/measure_s1.o
# shellcheck disable=SC2086
arm-none-eabi-g++ ${CPU} ${PROFILE} -I"${ROOT}/demo03_soa/classic/include" \
    -c "${ROOT}/demo03_soa/classic/src/particles.cpp" -o /tmp/measure_s2.o
# shellcheck disable=SC2086
arm-none-eabi-g++ ${CPU} ${PROFILE} ${MODERN} -I"${ROOT}/demo03_soa/modern/include" \
    -c "${ROOT}/demo03_soa/modern/src/particles.cpp" -o /tmp/measure_s3.o
printf "  stage 1 vector<unique_ptr<IParticle>>: %6s B\n" "$(text_of /tmp/measure_s1.o)"
printf "  stage 2 vector<Particle> (AoS):        %6s B\n" "$(text_of /tmp/measure_s2.o)"
printf "  stage 3 Struct-of-Arrays (SoA):        %6s B\n" "$(text_of /tmp/measure_s3.o)"
