/*
 * utils.c
 *
 *  Created on: 7 mar 2013
 *      Author: benjamin
 */

#include "utils.h"
#include "ch.h"
#include "hal.h"
#include "stm32f4xx.h"
#include <math.h>

void utils_init(void) {
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);

	// Microsecond delay timer
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13, ENABLE);
	uint16_t PrescalerValue = (uint16_t) ((RCC_Clocks.SYSCLK_Frequency / 2) / 1000000) - 1;
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM13, &TIM_TimeBaseStructure);
	TIM_Cmd(TIM13, ENABLE);
}

void utils_delay_us(unsigned int us) {
	unsigned int goal = TIM13->CNT;
	goal += us;
	goal &= 0xFFFF;

	while(TIM13->CNT > goal){};
	while(TIM13->CNT < goal){};
}
