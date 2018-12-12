#include "main.h"
#include "init.h"
#include "led.h"
#include "globals.h"
#include "delay.h"

int32_t intensity;
int8_t direction;


void TIM2_IRQHandler() {
	if (TIM_GetITStatus(TIM2, TIM_IT_Update)) {
		//
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}

void EXTI0_IRQHandler(void) {
	if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
		//
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}

void EXTI1_IRQHandler(void) {
	if (EXTI_GetITStatus(EXTI_Line1) != RESET) {
		LED_set_intensity(intensity);
		intensity++;
		if(intensity >= INTENSITY_STEP_COUNT) intensity = 1;
		EXTI_ClearITPendingBit(EXTI_Line1);
	}
}

int main(void) {
	uint32_t hex_array[] = {0xe6194B,0x3cb44b,0xffe119,0x4363d8,0xf58231,0x911eb4,0x42d4f4,0xf032e6,0xbfef45,0xfabebe,0x469990,0xe6beff,0x9A6324,0x808000,0xffd8b1,0xffffff};
	uint32_t size = sizeof(hex_array)/sizeof(hex_array[0]);
	uint32_t i = 0;
	intensity = INTENSITY_STEP_COUNT;
	init_all();
	LED_init();
	
	while (1) {
		LED_set_color_HEX(hex_array[i]);
		i++;
		if(i >= size) i = 0;
		delay(MS_1*50);
	}
}
