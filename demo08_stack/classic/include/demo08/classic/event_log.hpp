#pragma once
// Classic dynamic container: std::vector. Convenient — and every push_back
// may realloc, move the whole log, fragment the heap, and (with exceptions
// enabled) throw bad_alloc at the worst possible moment.
#include <cstdint>
#include <vector>

namespace demo08::classic {

struct Event {
    std::uint32_t timestamp;
    std::uint16_t code;
    std::uint16_t source;
};

class EventLog {
public:
    EventLog() = default;

    // The disciplined variant: pre-size the vector. Removes the realloc/move
    // on push_back — but the storage is still on the heap, the "budget" is a
    // runtime argument the type does not remember, and growing PAST it
    // silently reallocs (or throws bad_alloc) instead of failing visibly.
    explicit EventLog(std::size_t expected_capacity) {
        events_.reserve(expected_capacity);
    }

    // Unbounded: memory use depends on runtime history, not on a budget.
    void record(const Event& event) { events_.push_back(event); }

    [[nodiscard]] std::size_t capacity() const { return events_.capacity(); }
    [[nodiscard]] std::size_t size() const { return events_.size(); }
    [[nodiscard]] const Event& operator[](std::size_t i) const {
        return events_[i];
    }
    [[nodiscard]] std::uint32_t checksum() const;
    void clear() { events_.clear(); } // capacity stays allocated... probably

private:
    std::vector<Event> events_;
};

} // namespace demo08::classic
