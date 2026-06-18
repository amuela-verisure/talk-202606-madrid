#include "demo08/modern/event_log.hpp"

namespace demo08::modern {

std::uint32_t EventLog::checksum() const {
    std::uint32_t sum = 0;
    for (const auto& e : events_) {
        sum = sum * 31u + e.timestamp + e.code + e.source;
    }
    return sum;
}

} // namespace demo08::modern
