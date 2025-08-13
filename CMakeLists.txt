cmake_minimum_required(VERSION 3.13)

# Point to standalone TinyUSB if used externally
set(PICO_TINYUSB_PATH "/home/noneya/pico/tinyusb")

# Import Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Project config
project(uart_master C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize Pico SDK (this includes TinyUSB if PICO_TINYUSB_PATH is set)
pico_sdk_init()

add_executable(uart_master
    main.c
)

# Standard I/O options (USB and UART off)
pico_enable_stdio_usb(uart_master 0)
pico_enable_stdio_uart(uart_master 0)
# Enable USB and UART debug output
pico_enable_stdio_uart(uart_master 1)  # ✅ enable UART output

# Compile definitions for USB host
target_compile_definitions(uart_master PRIVATE
    CFG_TUSB_RHPORT0_MODE=OPT_MODE_HOST
    CFG_TUSB_OS=OPT_OS_PICO
    PICO_NO_USB=0
    PICO_USB_HOST=1
)

# Include directories
target_include_directories(uart_master PRIVATE
    ${PICO_TINYUSB_PATH}/src           # For TinyUSB core + class headers
    ${CMAKE_CURRENT_LIST_DIR}          # For tusb_config.h
)

# Link against required libraries
target_link_libraries(uart_master
    pico_stdlib
    hardware_uart
    tinyusb_host
)

# Generate additional outputs (.uf2, etc.)
pico_add_extra_outputs(uart_master)
