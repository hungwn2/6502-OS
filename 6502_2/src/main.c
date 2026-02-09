// main.c  (STM32F401RE)

#include <stdint.h>

#include "systick.h"
#include "uart.h"
#include "cpu.h"
#include "bus.h"
#include "gui.h"

#include "stm32f4xx.h"   // <-- F401 register definitions

// F401RE typical default after reset is HSI = 16 MHz (unless you configure PLL)
#define SYS_CLOCK_FREQ           16000000UL

#define TARGET_CYCLES_PER_SECOND 1000000ULL  // 1.0 MHz emulated CPU
#define CYCLES_PER_MILLISECOND   (TARGET_CYCLES_PER_SECOND / 1000ULL)

cpu_t cpu;

static void delay_ms(uint32_t ms)
{
    uint32_t start = systick_get_ms();
    while ((uint32_t)(systick_get_ms() - start) < ms) {
        __NOP();
    }
}

int main(void)
{
    // If you later switch to PLL, update SYS_CLOCK_FREQ accordingly.
    systick_init(SYS_CLOCK_FREQ);
    uart_init(SYS_CLOCK_FREQ);

    bus_reset();
    cpu_reset(&cpu);

    term_init();
    term_update_dashboard(&cpu);

    uint32_t last_time_ms = systick_get_ms();
    uint64_t cycle_budget = 0;
    uint32_t last_gui_update_ms = last_time_ms;

    while (1)
    {
        uint32_t current_time_ms = systick_get_ms();
        uint32_t elapsed_ms = (uint32_t)(current_time_ms - last_time_ms);

        if (elapsed_ms > 0) {
            cycle_budget += (CYCLES_PER_MILLISECOND * (uint64_t)elapsed_ms);
            last_time_ms = current_time_ms;
        }

        if (cycle_budget > 0)
        {
            uint64_t cycles_before = cpu.cycles;
            cpu_step(&cpu);                         // Run one instruction
            uint64_t cycles_ran = cpu.cycles - cycles_before;

            if (cycles_ran <= cycle_budget) cycle_budget -= cycles_ran;
            else cycle_budget = 0;                  // Overspent
        }

        // Update Terminal GUI ~10 Hz
        if ((uint32_t)(current_time_ms - last_gui_update_ms) > 100U)
        {
            last_gui_update_ms = current_time_ms;

            // If your GUI expects to be “ticked” here, call its update/tick.
            // Your original code called systick_handler() which is unusual to call manually.
            // Prefer a GUI tick function if you have one; otherwise keep your existing call:
            systick_handler();
        }
    }
}
