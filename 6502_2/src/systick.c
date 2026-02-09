/*
 * systick.c
 *
 * Implements a non-blocking 1ms SysTick interrupt for STM32C0.
 */

#include "systick.h"
#include "stm32c0xx.h" // CMSIS header for C0-series

// --- SysTick Control Register Defines ---
#define CTRL_ENABLE      (1U << 0) // Bit 0: Counter Enable
#define CTRL_TICKINT     (1U << 1) // Bit 1: Tick Interrupt Enable
#define CTRL_CLKSRC      (1U << 2) // Bit 2: Clock Source (1 = Processor Clock)

// This global variable is incremented by the interrupt handler
static volatile uint32_t g_ms_ticks = 0;

void systick_init(uint32_t sys_freq) {
    // 1. Calculate Reload Value for 1ms
    // (We want 1000 interrupts per second)
    uint32_t reload_val = (sys_freq / 1000U) - 1U;

    // 2. Set the Reload Register (LOAD)
    SysTick->LOAD = reload_val;

    // 3. Clear the Current Value Register (VAL)
    SysTick->VAL = 0U;

    // 4. Configure the Control and Status Register (CTRL)
    // Set CLKSOURCE = Processor clock
    // Set TICKINT   = Enable interrupt
    // Set ENABLE    = Enable the timer
    SysTick->CTRL = CTRL_CLKSRC | CTRL_TICKINT | CTRL_ENABLE;
}

uint32_t systick_get_ms(void) {
    // Return the volatile tick count
    return g_ms_ticks;
}

/**
 * @brief SysTick Interrupt Service Routine (ISR)
 * This function's name is defined by the ARM CMSIS standard.
 */
void SysTick_Handler(void) {
    g_ms_ticks++;
}

