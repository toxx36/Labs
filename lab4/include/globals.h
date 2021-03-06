#ifndef GLOBALS_H
#define GLOBALS_H

#include <stm32f4xx.h>

#define LED_FIRST_PIN GPIO_Pin_8
#define BTN_LEFT GPIO_Pin_0
#define BTN_RIGHT GPIO_Pin_1
#define TIM_PERIOD 0xFF

#define MS_1 168000
#define US_350 58800

extern volatile uint32_t intens;
extern volatile uint32_t color;

#endif
