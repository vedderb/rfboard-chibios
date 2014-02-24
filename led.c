/*
 * LED.c
 *
 *  Created on: 6 dec 2013
 *      Author: benjamin
 */

#include "led.h"
#include "ch.h"
#include "hal.h"

void led_init(void) {
	palSetPadMode(GPIOA, 2,
			PAL_MODE_OUTPUT_PUSHPULL |
			PAL_STM32_OSPEED_HIGHEST);
	palSetPadMode(GPIOA, 3,
			PAL_MODE_OUTPUT_PUSHPULL |
			PAL_STM32_OSPEED_HIGHEST);

	led_write(LED_RED, 0);
	led_write(LED_GREEN, 0);
}

void led_write(int num, int state) {
	switch (num) {
	case LED_RED:
		palWritePad(GPIOA, 3, state);
		break;

	case LED_GREEN:
		palWritePad(GPIOA, 2, state);
		break;

	default:
		break;
	}
}

void led_toggle(int num) {
	switch (num) {
	case LED_RED:
		palTogglePad(GPIOA, 3);
		break;

	case LED_GREEN:
		palTogglePad(GPIOA, 2);
		break;

	default:
		break;
	}
}
