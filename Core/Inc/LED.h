#ifndef __LED_H__
#define __LED_H__

#include "main.h"

void LED_Init(void);
#define LED1_Toggle()   HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1);
//#define LED2_Toggle()   HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_2);
//
//
//#define LED1_OFF()      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
//#define LED2_OFF()      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);

#endif