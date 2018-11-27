#include "irq.h"
#include "delay.h"
#include "globals.h"

void IRQ_EXTI_color(void) {
	GPIO_ToggleBits(GPIOD, GPIO_Pin_13);
	delay(MS_1 * 10);
	color++;
	if (color > 7)
		color = 1;
	TIM_SetCompare1(TIM1, ((color & 1)?1:0) * ((intens * intens) / TIM_PERIOD) );
	TIM_SetCompare2(TIM1,
			(((color & (1 << 1))?1:0) * ((intens * intens) / TIM_PERIOD)));
	TIM_SetCompare3(TIM1,
			(((color & (2 << 1))?1:0) * ((intens * intens) / TIM_PERIOD)));
}

void IRQ_EXTI_intens(void) {
	GPIO_ToggleBits(GPIOD, GPIO_Pin_15);
	delay(MS_1 * 10);
	intens += TIM_PERIOD / 10;
	if (intens > TIM_PERIOD)
		intens = TIM_PERIOD / 10;
	TIM_SetCompare1(TIM1, (((color & 1)?1:0) * ((intens * intens) / TIM_PERIOD)));
	TIM_SetCompare2(TIM1,
			(((color & (1 << 1))?1:0) * ((intens * intens) / TIM_PERIOD)));
	TIM_SetCompare3(TIM1,
			(((color & (2 << 1))?1:0) * ((intens * intens) / TIM_PERIOD)));
}

void IRQ_TIM_LEDFlash(void) {
	GPIO_ToggleBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_14);
}
