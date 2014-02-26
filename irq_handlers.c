/*
 * irq_handlers.c
 *
 *  Created on: 20 feb 2014
 *      Author: benjamin
 */
#include "ch.h"
#include "hal.h"
#include "hal_rf.h"

CH_IRQ_HANDLER(EXTI9_5_IRQHandler) {
	CH_IRQ_PROLOGUE();

	if (EXTI_GetITStatus(EXTI_Line7) != RESET) {
		EXTI_ClearITPendingBit(EXTI_Line7);

		halRfIRQ();
	}

	CH_IRQ_EPILOGUE();
}
