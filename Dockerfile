FROM ubuntu:24.04 AS base

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Etc/UTC

RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential \
        cmake \
        ninja-build \
        gcc-arm-none-eabi \
        libnewlib-arm-none-eabi \
        libstdc++-arm-none-eabi-newlib \
        qemu-system-arm \
        python3 \
        python3-pip \
        git \
        ca-certificates \
        fish \
    && rm -rf /var/lib/apt/lists/*

RUN pip3 install --no-cache-dir --break-system-packages lizard

WORKDIR /workspace
COPY . .

# Host debug build + tests
RUN cmake --preset host-debug \
    && cmake --build build/host-debug -j4 \
    && ctest --test-dir build/host-debug --output-on-failure

# Host release build (benchmarks: bench_soa, bench_expected)
RUN cmake --preset host-release \
    && cmake --build build/host-release -j4 \
    && ctest --test-dir build/host-release --output-on-failure

# ARM cross-compilation (classic vs modern firmware)
RUN cmake --preset arm-cortexm4 \
    && cmake --build build/arm -j4

# Print size report and run both firmwares on QEMU
RUN echo "=== ARM firmware sizes ===" \
    && arm-none-eabi-size build/arm/firmware_classic build/arm/firmware_modern \
    && qemu-system-arm -machine mps2-an386 -nographic -semihosting \
        -kernel build/arm/firmware_modern \
    && qemu-system-arm -machine mps2-an386 -nographic -semihosting \
        -kernel build/arm/firmware_classic

CMD ["bash"]
