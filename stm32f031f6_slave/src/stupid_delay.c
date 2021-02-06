/*
 * where-is-catiusha - cat localisation project
 * Copyright (C) 2021 Konrad Gotfryd
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "stupid_delay.h"
uint64_t __IO delay_val;

void delay_init(){
	if(SysTick_Config(SystemCoreClock / 100000UL)){
		while(1);
	}
}

void delay_ms(uint32_t value){	//unit of value = [ms]
	delay_val = value * 1000;
	while(delay_val != 0);
}

void delay_us(uint32_t value){
	delay_val = value - (value % 10);
	while(delay_val != 0);
}

void SysTick_Handler(void){
	if(delay_val != 0) delay_val -= 10;
}
