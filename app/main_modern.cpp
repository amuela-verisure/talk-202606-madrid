// Firmware entry point — modern build.
// Exercises every modern demo module: -fno-exceptions, -fno-rtti, zero heap.
// On QEMU/semihosting the output goes to the host terminal.
#include <array>
#include <cstdint>
#include <span>

#include "demo01/modern/shapes.hpp"
#include "demo02/modern/filters.hpp"
#include "demo03/modern/particles.hpp"
#include "demo04/modern/telemetry.hpp"
#include "demo05/modern/crc32.hpp"
#include "demo06/modern/packet.hpp"
#include "demo07/modern/pipeline.hpp"
#include "demo08/modern/event_log.hpp"
#include "demo09/modern/subject.hpp"

extern "C" int puts(const char*);

static volatile std::uint32_t sink;

// Static storage (.bss), initialized by code — no heap anywhere.
static demo03::modern::Particles particles;
static demo08::modern::EventLog event_log;

int main() {
    puts("[modern firmware] start");

    // Demo 1 — shapes: variant + visit, scene on the stack
    {
        auto scene = demo01::modern::make_scene();
        demo01::modern::scale_all(scene, 1.5f);
        sink = static_cast<std::uint32_t>(demo01::modern::total_area(scene));
    }

    // Demo 2 — concepts: static dispatch, fully inlined
    {
        constexpr std::array<float, 6> samples = {1.0f, 4.0f, 2.0f,
                                                  8.0f, 5.0f, 3.0f};
        demo02::modern::LowPass lp{.alpha = 0.3f};
        sink = static_cast<std::uint32_t>(
            demo02::modern::run_lowpass(lp, samples));
    }

    // Demo 3 — SoA particles, fixed compile-time budget
    {
        demo03::modern::make_particles(particles, 128);
        demo03::modern::update(particles, 0.016f);
        sink = static_cast<std::uint32_t>(demo03::modern::count_alive(particles));
    }

    // Demo 4 — expected: errors as values, no unwinder on board
    {
        constexpr std::array<std::uint8_t, 5> frame = {0xA5u, 0x07u, 0xFAu,
                                                       0x00u, 0xA6u};
        const auto stats = demo04::modern::process_stream(frame);
        sink = stats.accepted;
    }

    // Demo 5 — consteval CRC table in flash, constinit stats
    {
        constexpr std::array<std::uint8_t, 4> data = {0xDEu, 0xADu, 0xBEu, 0xEFu};
        sink = demo05::modern::crc32_checked(data);
    }

    // Demo 6 — bit_cast serialization, layout checked at compile time
    {
        const demo06::modern::SensorPacket pkt{1, 2, 300, 4000u};
        const auto bytes = demo06::modern::serialize(pkt);
        sink = bytes[0];
    }

    // Demo 7 — ranges pipeline into caller-provided storage
    {
        constexpr std::array<float, 8> raw = {10.0f, 20.0f,  150.0f, 30.0f,
                                              40.0f, -10.0f, 50.0f,  60.0f};
        std::array<float, 8> out{};
        sink = static_cast<std::uint32_t>(
            demo07::modern::process_readings(raw, out));
    }

    // Demo 8 — fixed-capacity event log in .bss
    {
        sink = static_cast<std::uint32_t>(event_log.record({1u, 2u, 3u}) ? 1u : 0u);
        sink = event_log.checksum();
    }

    // Demo 9 — variadic observers, folded at compile time
    {
        std::uint32_t alarms = 0;
        demo09::modern::Subject subject{
            [&alarms](const demo09::modern::Event& e) {
                if (e.code == 0xFFu) {
                    ++alarms;
                }
            },
        };
        subject.notify({0xFFu, 42});
        sink = alarms;
    }

    puts("[modern firmware] done");
    return 0;
}
