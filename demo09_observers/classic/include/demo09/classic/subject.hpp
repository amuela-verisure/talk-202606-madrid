#pragma once
// Classic observer: std::function type erasure in a std::vector.
// Flexible — and each subscription may heap-allocate, each notify walks a
// list of indirect calls the compiler cannot see through.
#include <cstdint>
#include <functional>
#include <vector>

namespace demo09::classic {

struct Event {
    std::uint16_t code;
    std::int32_t value;
};

class Subject {
public:
    using Handler = std::function<void(const Event&)>;

    // Possible heap allocation per subscription (captures beyond SBO).
    void subscribe(Handler handler) { handlers_.push_back(std::move(handler)); }

    void notify(const Event& event) const {
        for (const auto& handler : handlers_) {
            handler(event); // type-erased indirect call, per observer
        }
    }

    [[nodiscard]] std::size_t observer_count() const { return handlers_.size(); }

private:
    std::vector<Handler> handlers_;
};

} // namespace demo09::classic
