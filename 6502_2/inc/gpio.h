#ifndef TERMINAL_GUI_H
#define TERMINAL_GUI_H

#include "cpu.h"
#include "uart.h"
#include "gpio.h"

void term_init(void);
void term_clear_screen(void);
void term_pritnf(const char* format, ...);

#endif