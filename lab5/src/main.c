#include "main.h"
#include "init.h"
#include "irq.h"
#include "led.h"
#include "globals.h"
#include "delay.h"

int32_t intensity;
uint16_t rgb[3];
int8_t direction;

void TIM2_IRQHandler() {
	if (TIM_GetITStatus(TIM2, TIM_IT_Update)) {
		IRQ_TIM_LEDFlash();
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}

void EXTI0_IRQHandler(void) {
	if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
		//IRQ_EXTI_color();
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}

void EXTI1_IRQHandler(void) {
	if (EXTI_GetITStatus(EXTI_Line1) != RESET) {
		//IRQ_EXTI_intens();
		direction = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) ? -1 : 1;
		LED_change_intensity(&intensity, &direction);
		LED_set_color(rgb,&intensity);
		delay(MS_1*20); //bad idea
		EXTI_ClearITPendingBit(EXTI_Line1);
	}
}

int main(void) {
	LED_init();
    intensity = INTENSITY_STEP_COUNT;
    rgb[0] = MAX_INTENSITY;
    rgb[1] = 0;
    rgb[2] = 0;
    direction = 1;
	init_all();
	LED_set_color(rgb,&intensity);
	
	while (1) { 
		if(!GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_0))
		{
			direction = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) ? -1 : 1;
			LED_change_color(rgb, &direction);//, &cycle);
			LED_set_color(rgb,&intensity);
			delay(US_350);
		}
	}
}
