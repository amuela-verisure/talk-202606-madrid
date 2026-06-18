// Micro-benchmark: exceptions-as-control-flow (classic) vs tl::expected
// (modern) on a telemetry stream with a configurable corruption rate.
// Build host-release and run: ./build/host-release/bench_expected
#include <array>
#include <chrono>
#include <cstdio>
#include <vector>

#include "demo04/classic/telemetry.hpp"
#include "demo04/modern/telemetry.hpp"

namespace {

constexpr int frames = 10000;
constexpr int iterations = 100;

volatile std::uint32_t sink;

std::uint8_t checksum_of(std::span<const std::uint8_t> bytes) {
    std::uint8_t sum = 0;
    for (std::uint8_t b : bytes) {
        sum = static_cast<std::uint8_t>(sum + b);
    }
    return sum;
}

// corrupt_percent of the frames fail the checksum -> error path taken.
std::vector<std::uint8_t> make_stream(int corrupt_percent) {
    std::vector<std::uint8_t> stream;
    stream.reserve(static_cast<std::size_t>(frames) * 5u);
    for (int i = 0; i < frames; ++i) {
        std::array<std::uint8_t, 5> f{};
        f[0] = 0xA5u;
        f[1] = static_cast<std::uint8_t>(i & 0xFF);
        const auto reading = static_cast<std::int16_t>(i % 900);
        f[2] = static_cast<std::uint8_t>(static_cast<std::uint16_t>(reading) &
                                         0xFFu);
        f[3] = static_cast<std::uint8_t>(static_cast<std::uint16_t>(reading) >>
                                         8u);
        f[4] = checksum_of(std::span<const std::uint8_t>(f.data(), 4));
        if ((i * 100) % frames < corrupt_percent * 100) {
            f[4] = static_cast<std::uint8_t>(f[4] ^ 0xFFu);
        }
        stream.insert(stream.end(), f.begin(), f.end());
    }
    return stream;
}

template <typename F>
double time_ns_per_frame(F&& body) {
    const auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < iterations; ++i) {
        body();
    }
    const auto stop = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::nano>(stop - start).count() /
           (static_cast<double>(iterations) * frames);
}

} // namespace

int main() {
    std::printf("%d frames/stream, %d iterations\n", frames, iterations);
    std::printf("%-10s %16s %16s %9s\n", "corrupt", "exceptions ns/f",
                "expected ns/f", "ratio");

    for (int corrupt : {0, 1, 10, 25, 50}) {
        const auto stream = make_stream(corrupt);

        const double classic_ns = time_ns_per_frame([&] {
            sink = demo04::classic::process_stream(stream).accepted;
        });
        const double modern_ns = time_ns_per_frame([&] {
            sink = demo04::modern::process_stream(stream).accepted;
        });

        std::printf("%9d%% %16.1f %16.1f %8.1fx\n", corrupt, classic_ns,
                    modern_ns, classic_ns / modern_ns);
    }
    return 0;
}
