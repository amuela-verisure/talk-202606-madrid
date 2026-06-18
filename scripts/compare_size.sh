#!/usr/bin/env bash
# Compare .text, .data, .bss sections between classic and modern firmware.
set -euo pipefail

BUILD_DIR="${1:-build/arm}"
FW_CLASSIC="$BUILD_DIR/firmware_classic"
FW_MODERN="$BUILD_DIR/firmware_modern"

if [[ ! -f "$FW_CLASSIC" ]] || [[ ! -f "$FW_MODERN" ]]; then
    echo "Usage: $0 [build-dir]"
    echo "Expected: $FW_CLASSIC and $FW_MODERN"
    exit 1
fi

echo "=== classic firmware (virtual + heap + exceptions) ==="
arm-none-eabi-size "$FW_CLASSIC"
echo ""
echo "=== modern firmware (concepts + variant + expected, no heap) ==="
arm-none-eabi-size "$FW_MODERN"
echo ""

read -r c_text c_data c_bss _ <<< "$(arm-none-eabi-size "$FW_CLASSIC" | tail -1)"
read -r m_text m_data m_bss _ <<< "$(arm-none-eabi-size "$FW_MODERN" | tail -1)"

printf "=== Section deltas (modern - classic) ===\n"
printf "%-8s %8s %8s %8s\n" "Section" "classic" "modern" "Delta"
printf "%-8s %8d %8d %+8d\n" ".text" "$c_text" "$m_text" "$((m_text - c_text))"
printf "%-8s %8d %8d %+8d\n" ".data" "$c_data" "$m_data" "$((m_data - c_data))"
printf "%-8s %8d %8d %+8d\n" ".bss" "$c_bss" "$m_bss" "$((m_bss - c_bss))"

echo ""
echo "=== exception machinery check ==="
echo "classic .ARM.extab/.exidx sections:"
arm-none-eabi-readelf -S "$FW_CLASSIC" | grep -E "extab|exidx" || echo "  none"
echo "modern .ARM.extab/.exidx sections:"
arm-none-eabi-readelf -S "$FW_MODERN" | grep -E "extab|exidx" || echo "  none"
echo ""
echo "classic unwinder symbols: $(arm-none-eabi-nm "$FW_CLASSIC" | grep -c -E "_Unwind|__cxa_throw" || true)"
echo "modern unwinder symbols:  $(arm-none-eabi-nm "$FW_MODERN" | grep -c -E "_Unwind|__cxa_throw" || true)"
echo "classic malloc symbols:   $(arm-none-eabi-nm "$FW_CLASSIC" | grep -c -E " _?malloc$" || true)"
echo "modern malloc symbols:    $(arm-none-eabi-nm "$FW_MODERN" | grep -c -E " _?malloc$" || true)"
