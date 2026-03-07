cmake_minimum_required(VERSION 3.21)

add_executable(flash.elf
    Src/main.c
    Src/config.c
)

# local headers 
target_include_directories(flash.elf
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Inc
)

target_link_libraries(flash.elf
    PRIVATE fw_core_lib
    PRIVATE bare_drivers
    PRIVATE interface_layer  
)

target_link_options(flash.elf
    PRIVATE
        -T${STM32F411_LINKER_SCRIPT}
        -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/flash.map
        -Wl,--print-memory-usage
        -static
        -Wl,--start-group -lc -lm -Wl,--end-group
)

add_custom_command(TARGET flash.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary flash.elf flash.bin
    COMMAND ${CMAKE_SIZE}    flash.elf
    COMMENT "Generating flash.bin and printing memory usage"
)

# Target para gravar no microcontrolador via JLink
add_custom_target(load
    COMMAND jlink ${CMAKE_CURRENT_SOURCE_DIR}/flash.jlink
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS flash.elf
    COMMENT "Flashing firmware via JLink"
)