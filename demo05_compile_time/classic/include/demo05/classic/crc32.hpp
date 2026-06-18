#pragma once
// Classic runtime initialization: a global mutable table in .bss, filled at
// boot (or lazily, guarded by a flag checked on EVERY call).
#include <cstddef>
#include <cstdint>
#include <span>

namespace demo05::classic {

// 1024 bytes of RAM + an init function + an "is it initialized yet?" hazard.
void crc32_init();

[[nodiscard]] std::uint32_t crc32_compute(std::span<const std::uint8_t> data);

} // namespace demo05::classic
