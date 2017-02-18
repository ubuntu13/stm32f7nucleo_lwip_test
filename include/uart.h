#ifndef __UART_H
#define __UART_H

#include "stm32f7xx_hal.h"
#include "stdio.h"
#include "stdlib.h"

extern uint8_t usart_sta;
extern uint8_t usart_buf[100];
extern char uart5_send_buf[100];
extern uint8_t usart_sta;
extern uint8_t message_len;

void uart5_init(uint32_t boundRate);
void uart5_send_byte(uint8_t byte);
void uart5_send(const uint8_t* pBuf, uint16_t nLen);
void uart5_print(const char* pBuf);
void MY_PRINT_FLOAT_3DIGITS(float MyFloat);

#define 	MY_NOTICE  					uart5_print
#define 	MY_PRINTF(fmt, args...)		{snprintf(uart5_send_buf, 100, fmt, args); uart5_print(uart5_send_buf);}

#endif

