// MIT License
// 
// Copyright (c) 2020 Lehrstuhl Informatik 11 - RWTH Aachen University
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// 
// This file is part of cpm_lab.
// 
// Author: i11 - Embedded Software, RWTH Aachen University

/*
 * led.c
 *
 * Created: 20.11.2018 09:50:00
 *  Author: maczijewski
 */ 


#include <avr/io.h>
#include <util/delay.h>
#include "util.h"
#include "led.h"

uint8_t identification_LED_period_ticks[26]  = { 1, 4, 7, 10, 13, 16, 7, 10, 13, 16, 19, 10, 13, 16, 19, 22, 13, 16, 19, 22, 25, 16, 19, 22, 25, 28 };
uint8_t identification_LED_enabled_ticks[26] = { 0, 2, 2,  2,  2,  2, 5,  5,  5,  5,  5,  8,  8,  8,  8,  8, 11, 11, 11, 11, 11, 14, 14, 14, 14, 14 };
uint8_t vehicle_id = 0;
uint32_t tick_count = 0;

void led_set_state(spi_mosi_data_t* spi_mosi_data) {
	if ((PINA & 1) == 0) return; // test mode
	
	CLEAR_BIT(PORTC, 0);
	CLEAR_BIT(PORTC, 1);
	CLEAR_BIT(PORTC, 2);
	
	vehicle_id = spi_mosi_data->vehicle_id;
	
	if(vehicle_id==0 && tick_count % 25 == 0)
	{
		SET_BIT(PORTC, 0);
		SET_BIT(PORTC, 1);
		SET_BIT(PORTC, 2);
	}	
	if (vehicle_id != 0)
	{
		SET_BIT(PORTC, 0);
		SET_BIT(PORTC, 1);
		SET_BIT(PORTC, 2);
	}
	
	
}


void led_setup() {
	SET_BIT(DDRC, 0);
	SET_BIT(DDRC, 1);
	SET_BIT(DDRC, 2);
	SET_BIT(DDRC, 7);
}

void toggle_led()
{
	if ((PINA & 1) == 0) return; // test mode 
	
	tick_count++;
	if(tick_count % identification_LED_period_ticks[vehicle_id] < identification_LED_enabled_ticks[vehicle_id])
        {
            SET_BIT(PORTC, 7);
        }
	else
		{
			CLEAR_BIT(PORTC, 7);
		}
}

void test_led(uint8_t led1, uint8_t led2, uint8_t led3, uint8_t led4){
	CLEAR_BIT(PORTC, 0);
	CLEAR_BIT(PORTC, 1);
	CLEAR_BIT(PORTC, 2);
	CLEAR_BIT(PORTC, 7);
	
	if (led1) SET_BIT(PORTC, 0);
	if (led2) SET_BIT(PORTC, 1);
	if (led3) SET_BIT(PORTC, 2);
	if (led4) SET_BIT(PORTC, 7);
}