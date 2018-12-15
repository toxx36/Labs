#include "led.h"
#include "globals.h"

static uint16_t intens;
static uint16_t rgb_curr[3];

void LED_init_val(void) {

	rgb_curr[0] = 0;
	rgb_curr[1] = 0;
	rgb_curr[2] = 0;
	LED_set_intensity(INTENSITY_STEP_COUNT);
}

void LED_init_periph(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	
	//init board LEDs for PWM tim1
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_TIM1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_TIM1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_TIM1);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//init TIM1
	TIM_TimeBaseInitTypeDef TIM_BaseStruct;
	TIM_TimeBaseStructInit(&TIM_BaseStruct);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	TIM_BaseStruct.TIM_Prescaler = 0x1500; //optimal for green
	TIM_BaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_BaseStruct.TIM_Period = TIM_PERIOD; //!!!
	TIM_BaseStruct.TIM_ClockDivision = 0;
	TIM_BaseStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_BaseStruct); // Initialize timer with chosen settings

	//OC init PWM
	TIM_OCInitTypeDef TIM_OCStruct;
	TIM_OCStructInit(&TIM_OCStruct);
	TIM_OCStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCStruct.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCStruct.TIM_Pulse = 0;
	TIM_OC1Init(TIM1, &TIM_OCStruct);
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_OC2Init(TIM1, &TIM_OCStruct);
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_OC3Init(TIM1, &TIM_OCStruct);
	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);

	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	TIM_Cmd(TIM1, ENABLE); // Start timer
	
	LED_init_val();
}

uint16_t LED_calc_color(uint16_t *value) {
	return 	(intens == INTENSITY_STEP_COUNT) ?
				(*value == MAX_INTENSITY) ? MAX_INTENSITY : //intens is max & brightness is max
				( (uint32_t)(*value) * (*value) / MAX_INTENSITY) : //intens is max 
			( ( ((uint32_t)(*value) * (*value) / INTENSITY_STEP_COUNT) * (intens)) / MAX_INTENSITY); //other case
}

void LED_set_intensity(uint16_t intensity) {
	intens = intensity;
	LED_set_color_raw(rgb_curr);
}

void LED_set_color_raw(uint16_t *rgb) {	
	TIM_SetCompare1(USING_TIMER, LED_calc_color(rgb));
	TIM_SetCompare2(USING_TIMER, LED_calc_color(rgb+1));
	TIM_SetCompare3(USING_TIMER, LED_calc_color(rgb+2));
}

void LED_set_color_RGB(uint8_t *color) {
	uint8_t i;
	for(i=0;i<3;i++) {
		if(color[i] == 0) rgb_curr[i] = 0;
		else if(color[i] == 0xFF) rgb_curr[i] = MAX_INTENSITY;
		else rgb_curr[i] = (MAX_INTENSITY * color[i]) / 0xFF;
	}
	LED_set_color_raw(rgb_curr);
}

void LED_set_color_HEX(uint32_t color) {
	uint8_t rgb[3];
	rgb[0] = color>>16;
	rgb[1] = (color>>8)&0xFF;
	rgb[2] = color&0xFF;
	LED_set_color_RGB(rgb);
}
