#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "myUSB.h"
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "shell.h"

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "led.h"
#include "myUSB.h"
#include "utils.h"

// CC2520
#include "hal_cc2520.h"
#include "hal_rf.h"
#include "hal_rf_util.h"
#include "basic_rf.h"

/*
 * Timers used:
 * TIM13: utils
 */

// Settings
#define RF_CHANNEL			26
#define PAN_ID				0xFA11
#define NODE_ADDRESS		0x001
#define DEST_ADDRESS		0xFFFF // 0xFFFF sends to all nodes

#define RX_BUFFER_LENGTH	255

// Private variables
static basicRfCfg_t basicRfConfig;
static WORKING_AREA(rf_rx_thread_wa, 2048);
static WORKING_AREA(rf_tx_thread_wa, 2048);
static uint8_t rx_buffer[RX_BUFFER_LENGTH];

#define SHELL_WA_SIZE   THD_WA_SIZE(512)

static void cmd_mem(BaseSequentialStream *chp, int argc, char *argv[]) {
	size_t n, size;

	(void)argv;
	if (argc > 0) {
		chprintf(chp, "Usage: mem\r\n");
		return;
	}
	n = chHeapStatus(NULL, &size);
	chprintf(chp, "core free memory : %u bytes\r\n", chCoreStatus());
	chprintf(chp, "heap fragments   : %u\r\n", n);
	chprintf(chp, "heap free total  : %u bytes\r\n", size);
}

static void cmd_clocks(BaseSequentialStream *chp, int argc, char *argv[]) {
	(void)argv;
	if (argc > 0) {
		chprintf(chp, "Usage: clocks\r\n");
		return;
	}

	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);

	chprintf(chp, "System Clock:  %u\r\n", RCC_Clocks.SYSCLK_Frequency);
	chprintf(chp, "HCLK        :  %u\r\n", RCC_Clocks.HCLK_Frequency);
	chprintf(chp, "PCLK1       :  %u\r\n", RCC_Clocks.PCLK1_Frequency);
	chprintf(chp, "PCLK2       :  %u\r\n", RCC_Clocks.PCLK2_Frequency);
}

static void cmd_threads(BaseSequentialStream *chp, int argc, char *argv[]) {
	static const char *states[] = {THD_STATE_NAMES};
	Thread *tp;

	(void)argv;
	if (argc > 0) {
		chprintf(chp, "Usage: threads\r\n");
		return;
	}
	chprintf(chp, "    addr    stack prio refs     state           name time    \r\n");
	chprintf(chp, "-------------------------------------------------------------\r\n");
	tp = chRegFirstThread();
	do {
		chprintf(chp, "%.8lx %.8lx %4lu %4lu %9s %14s %lu\r\n",
				(uint32_t)tp, (uint32_t)tp->p_ctx.r13,
				(uint32_t)tp->p_prio, (uint32_t)(tp->p_refs - 1),
				states[tp->p_state], tp->p_name, (uint32_t)tp->p_time);
		tp = chRegNextThread(tp);
	} while (tp != NULL);
}

static const ShellCommand commands[] = {
		{"mem", cmd_mem},
		{"clocks", cmd_clocks},
		{"threads", cmd_threads},
		{NULL, NULL}
};

static const ShellConfig shell_cfg1 = {
		(BaseSequentialStream *)&SDU1,
		commands
};

static msg_t rf_rx_thread(void *arg) {
	(void)arg;
	uint8_t len;

	chRegSetThreadName("RF rx");

	for(;;) {
		if (basicRfPacketIsReady()) {
			len = basicRfReceive(rx_buffer, RX_BUFFER_LENGTH, NULL);

			if (len > 0) {
				led_write(LED_RED, rx_buffer[0]);
			}
		}

		chThdSleepMicroseconds(100);
	}

	return 0;
}

static msg_t rf_tx_thread(void *arg) {
	(void)arg;

	uint8_t tx_buffer[10];

	chRegSetThreadName("RF tx");

	for(;;) {
		tx_buffer[0] = 1;
		if (basicRfSendPacket(DEST_ADDRESS, tx_buffer, 1) == SUCCESS) {
			led_write(LED_GREEN, 1);
		} else {
			led_write(LED_GREEN, 0);
		}
		chThdSleepMilliseconds(100);

		tx_buffer[0] = 0;
		if (basicRfSendPacket(DEST_ADDRESS, tx_buffer, 1) == SUCCESS) {
			led_write(LED_GREEN, 1);
		} else {
			led_write(LED_GREEN, 0);
		}
		chThdSleepMilliseconds(100);
	}

	return 0;
}

int main(void) {
	halInit();
	chSysInit();
	utils_init();
	led_init();
	myUSBinit();

	// rf
	halAssyInit();
	basicRfConfig.panId = PAN_ID;
	basicRfConfig.channel = RF_CHANNEL;
	basicRfConfig.ackRequest = TRUE;
	basicRfConfig.myAddr = NODE_ADDRESS;

	if(basicRfInit(&basicRfConfig) == FAILED) {
		for(;;) {}
	}

	basicRfReceiveOn();

	chThdCreateStatic(rf_rx_thread_wa, sizeof(rf_rx_thread_wa),
			NORMALPRIO, rf_rx_thread, NULL);
	chThdCreateStatic(rf_tx_thread_wa, sizeof(rf_tx_thread_wa),
			NORMALPRIO, rf_tx_thread, NULL);

	static Thread *shelltp = NULL;

	for(;;) {
		if (!shelltp && (SDU1.config->usbp->state == USB_ACTIVE))
			shelltp = shellCreate(&shell_cfg1, SHELL_WA_SIZE, NORMALPRIO);
		else if (chThdTerminated(shelltp)) {
			chThdRelease(shelltp);
			shelltp = NULL;
		}

		chThdSleepMilliseconds(100);
	}
}
