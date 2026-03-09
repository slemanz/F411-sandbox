cmake_minimum_required(VERSION 3.21)

set(INTERFACE_SOURCES
    Src/interface_analog.c
    Src/interface_comm.c
    Src/interface_io.c
    Src/interface_pwm.c
    Src/interface_timebase.c
    Src/protocol_i2c.c
    Src/protocol_uart.c
)

add_library(interface_layer STATIC ${INTERFACE_SOURCES})

target_include_directories(interface_layer
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Inc
)

target_link_libraries(interface_layer
    PUBLIC
        bare_drivers
        fw_core_lib
)
