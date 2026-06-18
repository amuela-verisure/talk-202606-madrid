// Minimal syscall stubs for newlib-nano (bare-metal, no OS).
#include <cstdint>
#include <sys/stat.h>

extern "C" {

extern uint32_t _end;

__attribute__((used, externally_visible))
void* _sbrk(int incr) {
    static uint32_t* heap_end = &_end;
    uint32_t* prev = heap_end;
    heap_end += incr / 4;
    return prev;
}

// ARM semihosting SYS_WRITE0 — prints on the QEMU/debugger console.
// (Traps without a debugger attached; this target is QEMU mps2-an386.)
static void semihost_write0(const char* str) {
    register uint32_t r0 asm("r0") = 0x04u; // SYS_WRITE0
    register const char* r1 asm("r1") = str;
    asm volatile("bkpt 0xAB" : "+r"(r0) : "r"(r1) : "memory");
}

__attribute__((used, externally_visible))
int _write(int /*fd*/, const char* buf, int len) {
    char chunk[64];
    int written = 0;
    while (written < len) {
        const int n = (len - written) < 63 ? (len - written) : 63;
        for (int i = 0; i < n; ++i) {
            chunk[i] = buf[written + i];
        }
        chunk[n] = '\0';
        semihost_write0(chunk);
        written += n;
    }
    return len;
}
// Semihosting SYS_EXIT: cleanly terminates the QEMU process.
__attribute__((used, externally_visible, noreturn))
void _exit(int /*status*/) {
    register uint32_t r0 asm("r0") = 0x18u;    // SYS_EXIT
    register uint32_t r1 asm("r1") = 0x20026u; // ADP_Stopped_ApplicationExit
    asm volatile("bkpt 0xAB" : "+r"(r0) : "r"(r1) : "memory");
    while (true) {}
}

__attribute__((used, externally_visible))
int _read(int /*fd*/, char* /*buf*/, int /*len*/) { return 0; }
__attribute__((used, externally_visible))
int _close(int /*fd*/) { return -1; }
__attribute__((used, externally_visible))
int _fstat(int /*fd*/, struct stat* st) { st->st_mode = S_IFCHR; return 0; }
__attribute__((used, externally_visible))
int _isatty(int /*fd*/) { return 1; }
__attribute__((used, externally_visible))
int _lseek(int /*fd*/, int /*offset*/, int /*whence*/) { return 0; }

} // extern "C"
