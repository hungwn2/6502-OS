#ifndef SYSTICK_H
#define SYSTICK_H

#include "stm32f4xx.h"
#include <stdint.h>

void systick_init(uint32_t sys_freq);

uint32_t systick_get_ms(void);

#endif