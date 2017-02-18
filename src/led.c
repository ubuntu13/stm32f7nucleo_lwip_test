#include "led.h"

/* LED GPIOs */
const uint16_t LED_GPIO_PINS[LEDnum] = {LED_LD1_PIN, LED_LD2_PIN, LED_LD3_PIN};
GPIO_TypeDef*  LED_GPIO_PORTS[LEDnum] = {GPIOB, GPIOB, GPIOB};

void LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	/* Enable the GPIO_LED Clock */
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/* Configure the GPIO_LED pin */
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;

	for(uint8_t i=0; i<LEDnum; i++)
	{
		GPIO_InitStructure.Pin = LED_GPIO_PINS[i];
		HAL_GPIO_Init(LED_GPIO_PORTS[i], &GPIO_InitStructure);
		LED_Off(i);
	}
}

void LED_On(Led_Typedef Led)
{
	HAL_GPIO_WritePin(LED_GPIO_PORTS[Led], LED_GPIO_PINS[Led], GPIO_PIN_SET);
}

void LED_Off(Led_Typedef Led)
{
	HAL_GPIO_WritePin(LED_GPIO_PORTS[Led],LED_GPIO_PINS[Led], GPIO_PIN_RESET);
}

void LED_Toggle(Led_Typedef Led)
{
	HAL_GPIO_TogglePin(LED_GPIO_PORTS[Led],LED_GPIO_PINS[Led]);
}
