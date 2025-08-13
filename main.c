/*
 * ============================================================================
 * Title      : HID-to-UART Keyboard Bridge with ANSI Support (Fixed)
 * Author     : Don Moran
 * Target     : Raspberry Pi Pico (RP2040)
 * ============================================================================
 */

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "tusb.h"
#include "pico/stdio_uart.h"

#define UART_ID          uart0
#define UART_TX_PIN      16
#define UART_RX_PIN      17
#define UART_BAUD_RATE   115200
#define LED_PIN          25

#define DEBUG_UART_ID uart1
#define DEBUG_UART_TX_PIN 4
#define DEBUG_UART_BAUD 115200

// Modifier masks
#define HID_MOD_LSHIFT  0x02
#define HID_MOD_RSHIFT  0x20
#define HID_MOD_LCTRL   0x01
#define HID_MOD_RCTRL   0x10

// Arrow key HID codes
#define HID_KEY_ARROW_RIGHT  0x4F
#define HID_KEY_ARROW_LEFT   0x50
#define HID_KEY_ARROW_DOWN   0x51
#define HID_KEY_ARROW_UP     0x52
#define HID_KEY_CAPS_LOCK    0x39

// Global state
static uint8_t prev_keys[6] = {0};
static uint8_t prev_mod = 0;
static bool caps_lock = false;
static bool in_escape_sequence = false;
static bool esc_pending = false;  // Moved to global state for better tracking

// === UART Output ===
void send_char_uart(uint8_t c) {
    uart_putc_raw(UART_ID, c);
}

// === ASCII Lookup ===
char hid_keycode_to_ascii(uint8_t code, bool upper) {
    const char plain[128] = {
        [0x04] = 'a', [0x05] = 'b', [0x06] = 'c', [0x07] = 'd',
        [0x08] = 'e', [0x09] = 'f', [0x0A] = 'g', [0x0B] = 'h',
        [0x0C] = 'i', [0x0D] = 'j', [0x0E] = 'k', [0x0F] = 'l',
        [0x10] = 'm', [0x11] = 'n', [0x12] = 'o', [0x13] = 'p',
        [0x14] = 'q', [0x15] = 'r', [0x16] = 's', [0x17] = 't',
        [0x18] = 'u', [0x19] = 'v', [0x1A] = 'w', [0x1B] = 'x',
        [0x1C] = 'y', [0x1D] = 'z',
        [0x1E] = '1', [0x1F] = '2', [0x20] = '3', [0x21] = '4',
        [0x22] = '5', [0x23] = '6', [0x24] = '7', [0x25] = '8',
        [0x26] = '9', [0x27] = '0',
        [0x28] = '\n', [0x29] = 0x1B, [0x2A] = '\b', [0x2B] = '\t',
        [0x2C] = ' ', [0x2D] = '-', [0x2E] = '=', [0x2F] = '[',
        [0x30] = ']', [0x31] = '\\', [0x33] = ';', [0x34] = '\'',
        [0x35] = '`', [0x36] = ',', [0x37] = '.', [0x38] = '/',
    };

    const char shifted[128] = {
        [0x04] = 'A', [0x05] = 'B', [0x06] = 'C', [0x07] = 'D',
        [0x08] = 'E', [0x09] = 'F', [0x0A] = 'G', [0x0B] = 'H',
        [0x0C] = 'I', [0x0D] = 'J', [0x0E] = 'K', [0x0F] = 'L',
        [0x10] = 'M', [0x11] = 'N', [0x12] = 'O', [0x13] = 'P',
        [0x14] = 'Q', [0x15] = 'R', [0x16] = 'S', [0x17] = 'T',
        [0x18] = 'U', [0x19] = 'V', [0x1A] = 'W', [0x1B] = 'X',
        [0x1C] = 'Y', [0x1D] = 'Z',
        [0x1E] = '!', [0x1F] = '@', [0x20] = '#', [0x21] = '$',
        [0x22] = '%', [0x23] = '^', [0x24] = '&', [0x25] = '*',
        [0x26] = '(', [0x27] = ')',
        [0x2D] = '_', [0x2E] = '+', [0x2F] = '{', [0x30] = '}',
        [0x31] = '|', [0x33] = ':', [0x34] = '"', [0x35] = '~',
        [0x36] = '<', [0x37] = '>', [0x38] = '?',
    };

    if (upper && shifted[code]) return shifted[code];
    return plain[code];
}

// Helper function to check if a keycode is a letter (A-Z)
bool is_letter_key(uint8_t keycode) {
    return (keycode >= 0x04 && keycode <= 0x1D);
}

// Fixed handle_key function with proper ESC state management
void handle_key(uint8_t mod, uint8_t keycode) {
    // Clear pending ESC for special keys
    if (esc_pending && keycode != 0x29) {
        esc_pending = false;
    }

    // Handle special keys first
    if (keycode == 0x28) { // Enter
        send_char_uart('\r');
        return;
    }
    
    if (keycode == HID_KEY_CAPS_LOCK) {
        caps_lock = !caps_lock;
        esc_pending = false;  // Ensure ESC state cleared
        return;
    }

    // Handle arrow keys
    if (keycode == HID_KEY_ARROW_UP) {
        esc_pending = false;
        send_char_uart(0x1B); send_char_uart('['); send_char_uart('A');
        return;
    }
    if (keycode == HID_KEY_ARROW_DOWN) {
        esc_pending = false;
        send_char_uart(0x1B); send_char_uart('['); send_char_uart('B');
        return;
    }
    if (keycode == HID_KEY_ARROW_RIGHT) {
        esc_pending = false;
        send_char_uart(0x1B); send_char_uart('['); send_char_uart('C');
        return;
    }
    if (keycode == HID_KEY_ARROW_LEFT) {
        esc_pending = false;
        send_char_uart(0x1B); send_char_uart('['); send_char_uart('D');
        return;
    }

    // Handle ESC key
    if (keycode == 0x29) { // ESC
        if (esc_pending) {
            // Second ESC press - send single ESC and clear state
            esc_pending = false;
            send_char_uart(0x1B);
        } else {
            // First ESC press - set pending state
            esc_pending = true;
            send_char_uart(0x1B);
        }
        return;
    }

    // Handle pending ESC state
    if (esc_pending) {
        esc_pending = false;
        if (keycode == 0x2F) { // '['
            in_escape_sequence = true;
            send_char_uart('[');
            return;
        }
        // Not part of escape sequence - process normally
    }

    // Handle in-progress escape sequences
    if (in_escape_sequence) {
        bool shift = mod & (HID_MOD_LSHIFT | HID_MOD_RSHIFT);
        char raw = hid_keycode_to_ascii(keycode, shift);
        if (raw) {
            send_char_uart(raw);
            if ((raw >= 'A' && raw <= 'Z') || (raw >= 'a' && raw <= 'z') || raw == '~') {
                in_escape_sequence = false;
            }
        }
        return;
    }

    // Handle Ctrl+letter combinations
    bool ctrl = mod & (HID_MOD_LCTRL | HID_MOD_RCTRL);
    if (ctrl && is_letter_key(keycode)) {
        char ctrl_ch = (keycode - 0x04) + 1;
        send_char_uart(ctrl_ch);
        return;
    }

    // Normal key processing
    bool shift = mod & (HID_MOD_LSHIFT | HID_MOD_RSHIFT);
    bool upper = is_letter_key(keycode) ? (shift ^ caps_lock) : shift;
    char ch = hid_keycode_to_ascii(keycode, upper);
    
    if (ch) {
        send_char_uart(ch);
    }
}

// === USB Callbacks ===
void tuh_mount_cb(uint8_t dev_addr) {
    // Device mounted
}

void tuh_umount_cb(uint8_t dev_addr) {
    // Reset state
    memset(prev_keys, 0, sizeof(prev_keys));
    prev_mod = 0;
    caps_lock = false;
    in_escape_sequence = false;
    esc_pending = false;
}

void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* desc_report, uint16_t desc_len) {
    tuh_hid_receive_report(dev_addr, instance);
}

void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len) {
    if (len == 8) {
        uint8_t mod = report[0];
        uint8_t keycode[6];
        memcpy(keycode, &report[2], 6);

        // Only process new keypresses
        if (memcmp(keycode, prev_keys, 6) == 0 && mod == prev_mod) {
            tuh_hid_receive_report(dev_addr, instance);
            return;
        }

        for (int i = 0; i < 6; i++) {
            uint8_t key = keycode[i];
            if (key && memchr(prev_keys, key, 6) == NULL) {
                handle_key(mod, key);
            }
        }

        memcpy(prev_keys, keycode, 6);
        prev_mod = mod;
    }

    tuh_hid_receive_report(dev_addr, instance);
}

// === Main ===
int main() {
    // Initialize debug UART
    stdio_uart_init_full(DEBUG_UART_ID, DEBUG_UART_BAUD, DEBUG_UART_TX_PIN, -1);
    sleep_ms(2000);
    printf("\nHID-to-UART: Fixed ESC handling\n");

    // Initialize hardware
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    
    uart_init(UART_ID, UART_BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    uart_set_format(UART_ID, 8, 1, UART_PARITY_NONE);

    // Initialize USB
    tusb_init();

    while (1) {
        tuh_task();
        sleep_ms(1);
    }
}
