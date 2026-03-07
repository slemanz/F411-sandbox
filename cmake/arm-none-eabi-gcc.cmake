# Toolchain file for ARM Cortex-M4 (STM32F411)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR cortex-m4)

# Compiler
set(CMAKE_C_COMPILER   arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY      arm-none-eabi-objcopy)
set(CMAKE_SIZE         arm-none-eabi-size)

# CPU flags shared by all targets
set(CPU_FLAGS "-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16")

set(CMAKE_C_FLAGS_INIT   "${CPU_FLAGS}")
set(CMAKE_CXX_FLAGS_INIT "${CPU_FLAGS}")
set(CMAKE_ASM_FLAGS_INIT "${CPU_FLAGS}")

set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS_INIT} -ffunction-sections -fdata-sections -Wall -Wno-unused-parameter -O0 -g3" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_INIT} -ffunction-sections -fdata-sections -Wall -Wno-unused-parameter -O0 -g3" CACHE STRING "" FORCE)

set(CMAKE_EXE_LINKER_FLAGS_INIT
    "${CPU_FLAGS} -mthumb --specs=nosys.specs --specs=nano.specs -Wl,--gc-sections -static -Wl,--start-group -lc -lm -Wl,--end-group"
    CACHE STRING "" FORCE)

# Skip compiler checks (cross-compiling)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
