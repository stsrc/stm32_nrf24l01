#ifndef _STUPID_DELAY_H_
#define _STUPID_DELAY_H_
#include <stm32f0xx.h>
#include <core_cm0.h>
void delay_init();
void delay_ms(uint32_t delay_in_ms);
void delay_us(uint32_t delay_in_us);
#endif
