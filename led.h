/*
 * LED.h
 *
 *  Created on: 6 dec 2013
 *      Author: benjamin
 */

#ifndef LED_H_
#define LED_H_

// Functions
void led_init(void);
void led_write(int num, int state);
void led_toggle(int num);

// Definitions
#define LED_RED		0
#define LED_GREEN	1

#endif /* LED_H_ */
