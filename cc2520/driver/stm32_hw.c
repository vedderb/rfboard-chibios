/***********************************************************************************
 * INCLUDES
 */
#include "stm32_hw.h"


/***********************************************************************************
 * GLOBAL VARIABLES
 */


/***********************************************************************************
 * FUNCTIONS
 */
static void halRadioSpiInit(void);
static void halMcuRfInterfaceInit(void);

/***********************************************************************************
 * @fn          halRadioSpiInit
 *
 * @brief       Initalise Radio SPI interface
 *
 * @param       none
 *
 * @return      none
 */
static void halRadioSpiInit(void)
{
	/*
	 * Initializes the SPI driver 2. The SPI2 signals are routed as follow:
	 * PB12 - NSS.
	 * PB13 - SCK.
	 * PB14 - MISO.
	 * PB15 - MOSI.
	 *
	 * TODO: Initialize cc2520
	 */
	palSetPad(GPIOB, 12);
	palSetPadMode(GPIOB, 12, PAL_MODE_OUTPUT_PUSHPULL |
			PAL_STM32_OSPEED_HIGHEST);           /* NSS.     */
	palSetPadMode(GPIOB, 13, PAL_MODE_ALTERNATE(GPIO_AF_SPI2) |
			PAL_STM32_OSPEED_HIGHEST);           /* SCK.     */
	palSetPadMode(GPIOB, 14, PAL_MODE_ALTERNATE(GPIO_AF_SPI2));              /* MISO.    */
	palSetPadMode(GPIOB, 15, PAL_MODE_ALTERNATE(GPIO_AF_SPI2) |
			PAL_STM32_OSPEED_HIGHEST);           /* MOSI.    */

	// Configure SPI
	SPI_InitTypeDef spiInitStruct;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	spiInitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; // 5.25 MHz
	spiInitStruct.SPI_CPHA = SPI_CPHA_1Edge;
	spiInitStruct.SPI_CPOL = SPI_CPOL_Low;
	spiInitStruct.SPI_CRCPolynomial = 7;
	spiInitStruct.SPI_DataSize = SPI_DataSize_8b;
	spiInitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	spiInitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	spiInitStruct.SPI_Mode = SPI_Mode_Master;
	spiInitStruct.SPI_NSS = SPI_NSS_Soft;
	SPI_Init(SPI2, &spiInitStruct);
	SPI_Cmd(SPI2, ENABLE);
}


/***********************************************************************************
 * @fn      halMcuRfInterfaceInit
 *
 * @brief   Initialises SPI interface to CC2520 and configures reset and vreg
 *          signals as MCU outputs.
 *
 * @param   none
 *
 * @return  none
 */
static void halMcuRfInterfaceInit(void)
{
	// Initialize the CC2520 interface
	CC2520_SPI_END();
	CC2520_RESET_OPIN(0);
	CC2520_BASIC_IO_DIR_INIT();
}


/***********************************************************************************
 * @fn      halAssyInit
 *
 * @brief   Initialize interfaces between radio and MCU
 *
 * @param   none
 *
 * @return  none
 */
void halAssyInit(void)
{
	CC2520_GPIO0_DIR_OUT();
	CC2520_GPIO1_DIR_OUT();
	CC2520_GPIO2_DIR_OUT();
	CC2520_GPIO3_DIR_OUT();
	CC2520_GPIO4_DIR_OUT();
	CC2520_GPIO5_DIR_OUT();

	halRadioSpiInit();
	halMcuRfInterfaceInit();
#ifndef MRFI_CC2520
	//halDigioConfig(&pinRadio_GPIO0);
#endif
}
