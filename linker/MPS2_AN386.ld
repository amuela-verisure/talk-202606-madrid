/* MPS2-AN386 (Cortex-M4) memory layout for QEMU. */
MEMORY
{
    FLASH (rx)  : ORIGIN = 0x00000000, LENGTH = 256K
    RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 64K
}

_estack = ORIGIN(RAM) + LENGTH(RAM);

ENTRY(Reset_Handler)

SECTIONS
{
    .text :
    {
        KEEP(*(.isr_vector))
        *(.text*)
        *(.rodata*)
        *(.gcc_except_table*)   /* LSDA tables (classic build with exceptions) */
        . = ALIGN(4);
    } > FLASH

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH

    .ARM.exidx :
    {
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
    } > FLASH

    .preinit_array :
    {
        . = ALIGN(4);
        KEEP(*(.preinit_array*))
        . = ALIGN(4);
    } > FLASH

    .init_array :
    {
        . = ALIGN(4);
        KEEP(*(.init_array*))
        . = ALIGN(4);
    } > FLASH

    .fini_array :
    {
        . = ALIGN(4);
        KEEP(*(.fini_array*))
        . = ALIGN(4);
    } > FLASH

    .init :
    {
        . = ALIGN(4);
        KEEP(*(.init))
        KEEP(*(.fini))
        . = ALIGN(4);
    } > FLASH

    _etext = .;

    .data : AT(_etext)
    {
        _sdata = .;
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > RAM

    .bss :
    {
        _sbss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > RAM

    _end = .;
    end = .;
}
