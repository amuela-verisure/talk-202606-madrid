// Cortex-M4 startup for MPS2-AN386 (QEMU target).
// Copies .data, zeros .bss, calls main().
#include <cstdint>

extern "C" {

extern uint32_t _etext;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;
extern uint32_t _estack;

extern int main();
extern void __libc_init_array();
[[noreturn]] extern void _exit(int status);

void Reset_Handler() {
    // Enable the FPU (CP10/CP11 full access) — required for hard-float code.
    auto* cpacr = reinterpret_cast<volatile uint32_t*>(0xE000ED88u);
    *cpacr |= (0xFu << 20u);
    asm volatile("dsb; isb");

    // Copy .data from flash to RAM.
    uint32_t* src = &_etext;
    uint32_t* dst = &_sdata;
    while (dst < &_edata) {
        *dst++ = *src++;
    }

    // Zero .bss.
    dst = &_sbss;
    while (dst < &_ebss) {
        *dst++ = 0;
    }

    // Run static constructors (empty array in the modern build — verified via nm).
    __libc_init_array();

    // Exit QEMU via semihosting when main returns.
    _exit(main());
}

void Default_Handler() {
    while (true) {}
}

__attribute__((weak, alias("Default_Handler"))) void NMI_Handler();
__attribute__((weak, alias("Default_Handler"))) void HardFault_Handler();
__attribute__((weak, alias("Default_Handler"))) void MemManage_Handler();
__attribute__((weak, alias("Default_Handler"))) void BusFault_Handler();
__attribute__((weak, alias("Default_Handler"))) void UsageFault_Handler();
__attribute__((weak, alias("Default_Handler"))) void SVC_Handler();
__attribute__((weak, alias("Default_Handler"))) void DebugMon_Handler();
__attribute__((weak, alias("Default_Handler"))) void PendSV_Handler();
__attribute__((weak, alias("Default_Handler"))) void SysTick_Handler();

__attribute__((section(".isr_vector"), used))
void (*const vector_table[])() = {
    reinterpret_cast<void (*)()>(&_estack),
    Reset_Handler,
    NMI_Handler,
    HardFault_Handler,
    MemManage_Handler,
    BusFault_Handler,
    UsageFault_Handler,
    nullptr, nullptr, nullptr, nullptr,
    SVC_Handler,
    DebugMon_Handler,
    nullptr,
    PendSV_Handler,
    SysTick_Handler,
};

} // extern "C"
