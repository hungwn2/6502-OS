/*
 * uart.c
 *
 * Implements bare-metal UART functions for USART2 (on PA2).
 * Assumes an STM32C0-series chip and 48MHz HSI48 clock.
 */

#include "uart.h"
#include "stm32c0xx.h" // CMSIS header for C0-series

// --- RCC Defines ---
#define GPIOAEN (RCC_IOPENR_GPIOAEN)    // (1U << 0)
#define UART2EN (RCC_APBENR1_USART2EN)  // (1U << 17)

// --- GPIOA Defines (for PA2) ---
#define MODER2_MASK (GPIO_MODER_MODE2_Msk) // (3U << 4)
#define MODER2_AF   (GPIO_MODER_MODE2_1)   // (2U << 4) (Mode 10 = AF)

// AF1 (0001) for USART2_TX on PA2 in C0-series
#define AFR0_AF1_2  (1U << GPIO_AFRL_AFSEL2_Pos) // (1U << 8)
#define AFR0_MASK2  (GPIO_AFRL_AFSEL2_Msk)       // (0xFU << 8)

// --- USART2 Defines ---
#define CR1_TE (USART_CR1_TE) // Transmitter Enable
#define CR1_UE (USART_CR1_UE) // USART Enable
#define SR_TXE (USART_ISR_TXE_TXFNF) // Transmit Data Register Empty

// --- Clock & Baud Rate (Default 48MHz HSI) ---
#define PCLK      48000000
#define BAUD_RATE 115200

// --- Private Helper ---

static uint16_t compute_uart_bd(uint32_t periph_clk, uint32_t baudrate) {
    // Formula for BRR from C0 datasheet
    return (uint16_t)((periph_clk + (baudrate / 2U)) / baudrate);
}

// --- Public API ---

void uart_init(void) {
    // 1. Enable GPIOA Clock
    RCC->IOPENR |= GPIOAEN;

    // 2. Enable UART2 Clock
    // UART2 is on the APBENR1 bus
    RCC->APBENR1 |= UART2EN;

    // 3. Set PA2 to Alternate Function (AF) Mode
    GPIOA->MODER &= ~MODER2_MASK;
    GPIOA->MODER |= MODER2_AF;

    // 4. Set the Alternate Function to AF1 (for USART2 on PA2)
    GPIOA->AFR[0] &= ~AFR0_MASK2;
    GPIOA->AFR[0] |= AFR0_AF1_2;

    // 5. Configure Baud Rate (assuming 48MHz PCLK)
    USART2->BRR = compute_uart_bd(PCLK, BAUD_RATE);

    // 6. Enable Transmitter and UART
    USART2->CR1 = CR1_TE | CR1_UE; // Set both, clear all other bits
}

void uart_write_byte(int ch) {
    // Wait for the Transmit Data Register (TXE) to be empty
    // On C0, this flag is TXE_TXFNF
    while (!(USART2->ISR & SR_TXE)) {
        // Spin-wait
    }

    // Write the byte to the Transmit Data Register (TDR)
    USART2->TDR = (ch & 0xFF);
}

void uart_write_string(const char* str) {
    while (*str) {
        uart_write_byte(*str++);
    }
}