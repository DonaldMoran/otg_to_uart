# OTG to UART Bridge for Raspberry Pi Pico

This project implements a USB On-The-Go (OTG) to UART bridge on a Raspberry Pi Pico. It allows a host device (like a PC) to communicate with a UART device connected to the Pico, effectively acting as a USB-to-serial adapter.

## Project Description

The primary purpose of this project is to facilitate communication between a host computer and a UART-based peripheral. In my specific use case, this Pico acts as a bridge to an emulated 6502 computer running on another Raspberry Pi Pico. This setup is part of a larger system where the emulated 6502 computer (from the [PicoDVI-UART-Terminal](https://github.com/DonaldMoran/PicoDVI-UART-Terminal) project) communicates with yet another Pico using the PicoDVI library.

Essentially, this project enables a seamless serial communication link from a standard USB port on a computer to a custom UART interface, which then connects to the 6502 emulator.

## Features

*   **USB OTG Host/Device Capability**: Configured to act as a USB device, presenting a CDC (Communication Device Class) serial port to the host.
*   **UART Bridging**: Transparently forwards data between the USB CDC interface and the Pico's hardware UART.
*   **Configurable Baud Rate**: The UART baud rate can be configured within the `main.c` file.

## Building and Flashing

1.  **Prerequisites**:
    *   Raspberry Pi Pico SDK
    *   CMake
    *   GCC ARM Embedded Toolchain

2.  **Clone the repository**:
    ```bash
    git clone <this_repository_url>
    cd otg_to_uart
    ```

3.  **Build**:
    ```bash
    mkdir build
    cd build
    cmake ..
    make
    ```

4.  **Flash**:
    Copy the generated `uart_master.uf2` file (found in the `build` directory) to your Raspberry Pi Pico when it's in USB Bootloader mode (hold `BOOTSEL` while plugging it in).

## Usage

Once flashed, connect the Pico's USB port to your host computer. It should enumerate as a serial port. You can then use any serial terminal program (e.g., `minicom`, `PuTTY`, `screen`) to communicate with the UART device connected to the Pico's GPIO pins.

Ensure the UART connections (TX, RX, GND) on the Pico are correctly wired to your target UART device.

## Related Projects

*   **[PicoDVI-UART-Terminal](https://github.com/DonaldMoran/PicoDVI-UART-Terminal)**: This project is used in conjunction with the current one. It features an emulated 6502 computer running on a Raspberry Pi Pico, which then communicates with another Pico using the PicoDVI library. This `otg_to_uart` bridge provides the necessary serial link for a host computer to interact with that 6502 emulator.
