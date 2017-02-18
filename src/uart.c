/*
 * uart.c
 *
 *  Created on: 2016Äê12ÔÂ1ÈÕ
 *      Author: Administrator
 */

#include "uart.h"

UART_HandleTypeDef UART5_Handle;
uint8_t usart_buf[100];
char uart5_send_buf[100];
uint8_t usart_sta = 0;
uint8_t message_len = 0;

void uart5_init(uint32_t boundRate)
{
	/* Configures the nested vectored interrupt controller */
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable the USART clocks */
	__HAL_RCC_UART5_CLK_ENABLE();

	/* Enable GPIO clock */
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/* USART5 TX pin configuration */
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Pin = GPIO_PIN_8;
	GPIO_InitStructure.Alternate = GPIO_AF7_USART1;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* USART5 RX pin configuration */
	GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Pin = GPIO_PIN_8;
	GPIO_InitStructure.Alternate = GPIO_AF7_USART1;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* USARTx configured as follow:
		- BaudRate = 115200 baud
		- Word Length = 8 Bits
		- One Stop Bit
		- No parity
		- Hardware flow control disabled (RTS and CTS signals)
		- Receive and transmit enabled
	*/
	UART5_Handle.Instance = UART5;
	UART5_Handle.Init.WordLength = UART_WORDLENGTH_8B;
	UART5_Handle.Init.StopBits = UART_STOPBITS_1;
	UART5_Handle.Init.Parity = UART_PARITY_NONE;
	UART5_Handle.Init.Mode = UART_MODE_TX_RX;
	UART5_Handle.Init.BaudRate = boundRate;
	UART5_Handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	HAL_UART_Init(&UART5_Handle);

	HAL_NVIC_SetPriority(UART5_IRQn, 0, 1);
	HAL_NVIC_EnableIRQ(UART5_IRQn);

	__HAL_UART_ENABLE_IT(&UART5_Handle, UART_IT_RXNE);

	__HAL_UART_ENABLE(&UART5_Handle);
}

void uart5_send_byte(uint8_t byte)
{
	while(__HAL_UART_GET_FLAG(&UART5_Handle, UART_FLAG_TXE) == RESET);
	UART5_Handle.Instance->TDR = byte;
}

void uart5_send(const uint8_t* pBuf, uint16_t nLen)
{
	uint16_t i = 0;

	for(i=0; i<nLen; i++)
	{
		uart5_send_byte(pBuf[i]);
	}
}

void uart5_print(const char* pBuf)
{
	for(;;)
	{
		if(pBuf == NULL)
			break;

		if(*pBuf == 0)
			break;

		uart5_send_byte(*pBuf);

		pBuf++;
	}
}

void MY_PRINT_FLOAT_3DIGITS(float MyFloat)
{
	char MyString[20];
	int d,f;

	if(MyFloat >= 0)
	{
		d = (int)(MyFloat) * 1000; // Decimal precision: 3 digits
		f = (MyFloat * 1000) - d; // Decimal precision: 3 digits
		sprintf(MyString, "+%d.%03d", d/1000, f);
	}
	else
	{
		d = (int)(-MyFloat) * 1000; // Decimal precision: 3 digits
		f = (-MyFloat * 1000) - d; // Decimal precision: 3 digits
		sprintf(MyString, "-%d.%03d", d/1000, f);
	}

	uart5_print(MyString);
}

void UART5_IRQHandler(void)
{
	uint8_t byte;

	if(__HAL_UART_GET_FLAG(&UART5_Handle, UART_FLAG_RXNE) == SET)
	{
		byte = UART5_Handle.Instance->RDR;

		//DT_Data_Receive_Prepare(byte);
	}
}
