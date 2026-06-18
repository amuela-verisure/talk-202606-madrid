# ARM Cortex-M4 cross-compilation toolchain (QEMU mps2-an386 target).
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER   arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(CMAKE_SIZE    arm-none-eabi-size)
set(CMAKE_AR      arm-none-eabi-ar)

set(CPU_FLAGS "-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16")
set(CMAKE_C_FLAGS_INIT   "${CPU_FLAGS} -fno-exceptions")
set(CMAKE_CXX_FLAGS_INIT "${CPU_FLAGS} -fno-exceptions -fno-rtti")

set(CMAKE_EXE_LINKER_FLAGS_INIT
    "-specs=nosys.specs -specs=nano.specs -Wl,--gc-sections")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
