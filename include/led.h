#ifndef LED_H_
#define LED_H_

#include "stm32f7xx_hal.h"

typedef enum
{
	LED1 = 0,
	LED2 = 1,
	LED3 = 2,
} Led_Typedef;

#define LEDnum					3

#define LED_LD1_PIN				GPIO_PIN_0
#define LED_LD1_GPIO_PORT		GPIOB

#define LED_LD2_PIN				GPIO_PIN_7
#define LED_LD2_GPIO_PORT		GPIOB

#define LED_LD3_PIN				GPIO_PIN_14
#define LED_LD3_GPIO_PORT		GPIOB

void LED_Init(void);
void LED_On(Led_Typedef Led);
void LED_Off(Led_Typedef Led);
void LED_Toggle(Led_Typedef Led);


#endif /* LED_H_ */
