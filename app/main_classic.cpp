// Firmware entry point — classic build.
// Exercises every classic demo module: virtual dispatch, heap allocation,
// type erasure, and (demo 4) exceptions as control flow. Compiled with
// -fexceptions to pull in the unwinder the design depends on.
#include <array>
#include <cstdint>
#include <span>

#include "demo01/classic/shapes.hpp"
#include "demo02/classic/filters.hpp"
#include "demo03/classic/particles.hpp"
#include "demo03/classic/particles_ptr.hpp"
#include "demo04/classic/telemetry.hpp"
#include "demo05/classic/crc32.hpp"
#include "demo06/classic/packet.hpp"
#include "demo07/classic/pipeline.hpp"
#include "demo08/classic/event_log.hpp"
#include "demo09/classic/subject.hpp"

extern "C" int puts(const char*);

static volatile std::uint32_t sink;

int main() {
    puts("[classic firmware] start");

    // Demo 1 — shapes: virtual dispatch, one heap allocation per shape
    {
        auto scene = demo01::classic::make_scene();
        demo01::classic::scale_all(scene, 1.5f);
        sink = static_cast<std::uint32_t>(demo01::classic::total_area(scene));
    }

    // Demo 2 — abstract interface, vtable call per sample
    {
        constexpr std::array<float, 6> samples = {1.0f, 4.0f, 2.0f,
                                                  8.0f, 5.0f, 3.0f};
        demo02::classic::LowPass lp{0.3f};
        sink = static_cast<std::uint32_t>(
            demo02::classic::run_filter(lp, samples));
    }

    // Demo 3 — storage evolution stages 1 and 2: heap objects, then AoS
    {
        auto ptr_particles = demo03::classic::make_particles_ptr(64);
        demo03::classic::update(ptr_particles, 0.016f);
        sink = static_cast<std::uint32_t>(
            demo03::classic::count_alive(ptr_particles));

        auto particles = demo03::classic::make_particles(128);
        demo03::classic::update(particles, 0.016f);
        sink = static_cast<std::uint32_t>(demo03::classic::count_alive(particles));
    }

    // Demo 4 — exceptions as control flow (unwinder + tables on board)
    {
        constexpr std::array<std::uint8_t, 5> frame = {0xA5u, 0x07u, 0xFAu,
                                                       0x00u, 0xA6u};
        const auto stats = demo04::classic::process_stream(frame);
        sink = stats.accepted;
    }

    // Demo 5 — CRC table built at boot, in RAM
    {
        demo05::classic::crc32_init();
        constexpr std::array<std::uint8_t, 4> data = {0xDEu, 0xADu, 0xBEu, 0xEFu};
        sink = demo05::classic::crc32_compute(data);
    }

    // Demo 6 — memcpy serialization, runtime asserts
    {
        const demo06::classic::SensorPacket pkt{1, 2, 300, 4000u};
        std::array<std::uint8_t, 8> bytes{};
        demo06::classic::serialize(pkt, bytes);
        sink = bytes[0];
    }

    // Demo 7 — loop pipeline with intermediate heap vectors
    {
        constexpr std::array<float, 8> raw = {10.0f, 20.0f,  150.0f, 30.0f,
                                              40.0f, -10.0f, 50.0f,  60.0f};
        const auto out = demo07::classic::process_readings(raw);
        sink = static_cast<std::uint32_t>(out.size());
    }

    // Demo 8 — std::vector event log on the heap
    {
        demo08::classic::EventLog event_log;
        event_log.record({1u, 2u, 3u});
        sink = event_log.checksum();
    }

    // Demo 9 — std::function observers, type-erased
    {
        std::uint32_t alarms = 0;
        demo09::classic::Subject subject;
        subject.subscribe([&alarms](const demo09::classic::Event& e) {
            if (e.code == 0xFFu) {
                ++alarms;
            }
        });
        subject.notify({0xFFu, 42});
        sink = alarms;
    }

    puts("[classic firmware] done");
    return 0;
}
