#pragma once
// Modern fixed-capacity container: storage is part of the object, on the
// stack or in .bss — sized at COMPILE TIME, no allocator, no realloc, no
// bad_alloc. Overflow is an explicit, testable result, not a surprise.
// [C++26] std::inplace_vector (P0843) standardizes exactly this.
#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace demo08::modern {

struct Event {
    std::uint32_t timestamp;
    std::uint16_t code;
    std::uint16_t source;
};

template <typename T, std::size_t Capacity>
    requires std::is_trivially_copyable_v<T>
class FixedVector {
public:
    // The failure mode is in the signature, not in an exception spec.
    [[nodiscard]] constexpr bool push_back(const T& value) {
        if (size_ == Capacity) {
            return false;
        }
        storage_[size_++] = value;
        return true;
    }

    [[nodiscard]] constexpr std::size_t size() const { return size_; }
    [[nodiscard]] static constexpr std::size_t capacity() { return Capacity; }
    [[nodiscard]] constexpr const T& operator[](std::size_t i) const {
        return storage_[i];
    }
    constexpr void clear() { size_ = 0; }

    [[nodiscard]] constexpr const T* begin() const { return storage_.data(); }
    [[nodiscard]] constexpr const T* end() const {
        return storage_.data() + size_;
    }

private:
    std::array<T, Capacity> storage_{};
    std::size_t size_{0};
};

inline constexpr std::size_t log_capacity = 64;

class EventLog {
public:
    [[nodiscard]] constexpr bool record(const Event& event) {
        return events_.push_back(event);
    }

    [[nodiscard]] constexpr std::size_t size() const { return events_.size(); }
    [[nodiscard]] constexpr const Event& operator[](std::size_t i) const {
        return events_[i];
    }
    [[nodiscard]] std::uint32_t checksum() const;
    constexpr void clear() { events_.clear(); }

private:
    FixedVector<Event, log_capacity> events_;
};

// The whole log is one flat object — memory budget readable in the type.
static_assert(sizeof(EventLog) ==
              log_capacity * sizeof(Event) + sizeof(std::size_t));

} // namespace demo08::modern
