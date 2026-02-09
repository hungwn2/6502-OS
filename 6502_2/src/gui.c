/**
 * terminal_gui.c
 *
 * Implements the text-based dashboard.
 * This is where we build our own simple "printf"
 * functionality using our bare-metal drivers.
 */

#include "terminal_gui.h"
#include "uart.h"     // Depends only on the bare-metal UART driver
#include <stdbool.h>

// --- ANSI Escape Codes ---
#define ANSI_CLEAR_SCREEN "\x1B[2J"
#define ANSI_CURSOR_HOME  "\x1B[H"

// --- Private Helper Functions ---

/**
 * @brief Converts a 4-bit nibble (0-15) to its ASCII hex char.
 */
static char to_hex_char(uint8_t nibble) {
    nibble &= 0x0F;
    if (nibble < 10) {
        return nibble + '0';
    }
    return (nibble - 10) + 'A';
}

/**
 * @brief Prints an unsigned 32-bit value in hex.
 * @param val The value to print.
 * @param digits The number of hex digits to print (e.g., 2, 4, 8).
 */
static void term_print_hex(uint32_t val, uint8_t digits) {
    // Print from the most significant digit down
    for (int i = (digits - 1) * 4; i >= 0; i -= 4) {
        uart_write_byte(to_hex_char((val >> i)));
    }
}

/**
 * @brief Moves the terminal cursor to a specific (row, col)
 * (Note: Terminal rows/cols are 1-based, not 0-based)
 * @param row The row number (1 = top).
 * @param col The column number (1 = left).
 */
static void term_goto_xy(uint8_t row, uint8_t col) {
    // We can't use printf, so we build the string manually.
    // Max string: "\x1B[255;255H" = 10 chars + null
    char buf[12];
    
    // Manual integer-to-string for row
    int i = 0;
    buf[i++] = '\x1B'; // Escape
    buf[i++] = '[';     // [
    
    // Convert 'row' to string
    if (row >= 100) buf[i++] = (row / 100) % 10 + '0';
    if (row >= 10)  buf[i++] = (row / 10) % 10 + '0';
    buf[i++] = (row % 10) + '0';
    
    buf[i++] = ';'; // Separator
    
    // Convert 'col' to string
    if (col >= 100) buf[i++] = (col / 100) % 10 + '0';
    if (col >= 10)  buf[i++] = (col / 10) % 10 + '0';
    buf[i++] = (col % 10) + '0';
    
    buf[i++] = 'H'; // Command
    buf[i++] = '\0'; // Null terminator
    
    uart_write_string(buf);
}

// --- Public API Functions ---

void term_init(void) {
    uart_write_string(ANSI_CLEAR_SCREEN);
    uart_write_string(ANSI_CURSOR_HOME);
}

void term_update_dashboard(cpu_6502_t* cpu) {
    // --- Title ---
    term_goto_xy(1, 1);
    uart_write_string("--- STM32C0-6502 Emulator Dashboard ---");
    
    // --- Registers ---
    term_goto_xy(3, 1);
    uart_write_string("PC: $"); term_print_hex(cpu->pc, 4);
    
    term_goto_xy(3, 14);
    uart_write_string("SP: $01"); term_print_hex(cpu->sp, 2);
    
    term_goto_xy(4, 1);
    uart_write_string(" A: $"); term_print_hex(cpu->a, 2);
    
    term_goto_xy(4, 14);
    uart_write_string(" X: $"); term_print_hex(cpu->x, 2);

    term_goto_xy(4, 27);
    uart_write_string(" Y: $"); term_print_hex(cpu->y, 2);

    // --- Status Flags ---
    term_goto_xy(6, 1);
    uart_write_string("Flags (P): N V - B D I Z C");
    
    term_goto_xy(7, 1);
    uart_write_string("           "); // Align under the letters
    
    // Print the 8 flags by checking each bit in the 'P' register
    uart_write_byte(cpu->p & FLAG_N ? 'N' : '.');
    uart_write_byte(' ');
    uart_write_byte(cpu->p & FLAG_V ? 'V' : '.');
    uart_write_byte(' ');
    uart_write_byte('-'); // Unused flag
    uart_write_byte(' ');
    uart_write_byte(cpu->p & FLAG_B ? 'B' : '.');
    uart_write_byte(' ');
    uart_write_byte(cpu->p & FLAG_D ? 'D' : '.');
    uart_write_byte(' ');
    uart_write_byte(cpu->p & FLAG_I ? 'I' : '.');
    uart_write_byte(' ');
    uart_write_byte(cpu->p & FLAG_Z ? 'Z' : '.');
    uart_write_byte(' ');
    uart_write_byte(cpu->p & FLAG_C ? 'C' : '.');
    
    // --- Console Output ---
    term_goto_xy(9, 1);
    uart_write_string("--- 6502 CONSOLE OUTPUT ---");
    
    // Move cursor to the console area, ready for the
    // 6502's own UART output (from bus_write at $F001).
    term_goto_xy(10, 1);
}