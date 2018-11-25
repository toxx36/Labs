#include "main.h"

//#define CHANGE_SPEED 50
//#define LED_COUNT 3
#define LED_FIRST_PIN GPIO_Pin_8
#define BTN_LEFT GPIO_Pin_0
#define BTN_RIGHT GPIO_Pin_1
#define MS_1 168000
#define TIM_PERIOD 0xFF

uint32_t intens = TIM_PERIOD;
uint32_t color = 1;

void delay(uint32_t time) {
	for (; time > 0; time--)
		;
}

//Toggles every second LEDs (R,G) on board
void TIM2_IRQHandler() {
	if (TIM_GetITStatus(TIM2, TIM_IT_Update)) {
		GPIO_ToggleBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_14);
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}

void EXTI0_IRQHandler(void) {
	if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
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
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}

void EXTI1_IRQHandler(void) {
	if (EXTI_GetITStatus(EXTI_Line1) != RESET) {
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
		EXTI_ClearITPendingBit(EXTI_Line1);
	}
}

int main(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);

	//init board LEDs */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14
			| GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	//init buttons
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin = BTN_LEFT | BTN_RIGHT;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

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
	// int8_t button, cycle, move = 0;
	// uint8_t duty = 0;

	//init TIM2
	//Update Event (Hz) = timer_clock / ((TIM_Prescaler + 1) * (TIM_Period + 1)) == 1 HZ
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_TimeBaseInitTypeDef TimBaseInit;
	TimBaseInit.TIM_Prescaler = 42000 - 1;
	TimBaseInit.TIM_CounterMode = TIM_CounterMode_Up;
	TimBaseInit.TIM_Period = 2000 - 1;
	TimBaseInit.TIM_ClockDivision = 0;
	TIM_TimeBaseInit(TIM2, &TimBaseInit);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	//IRQ TIM2 config
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);

	TIM_Cmd(TIM2, ENABLE);

	//EXTI 0-1 init for buttons
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource0);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource1);

	EXTI_InitTypeDef EXTI_InitStruct;
	EXTI_InitStruct.EXTI_Line = EXTI_Line0 | EXTI_Line1;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStruct);

	NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);

	NVIC_InitStruct.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_Init(&NVIC_InitStruct);

	//light up at first time
	TIM_SetCompare1(TIM1, ((color & 1) * ((intens * intens) / TIM_PERIOD)));
	TIM_SetCompare2(TIM1,
			((color & (1 << 1)) * ((intens * intens) / TIM_PERIOD)));
	TIM_SetCompare3(TIM1,
			((color & (2 << 1)) * ((intens * intens) / TIM_PERIOD)));

	while (1) {

		/*	if(!GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_1) && !GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_0))
		 TIM_SetCompare1(TIM1,200);
		 else if(!GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_0))
		 TIM_SetCompare2(TIM1,200);
		 else if(!GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_1))
		 TIM_SetCompare3(TIM1,200);
		 else {
		 TIM_SetCompare1(TIM1,0);
		 TIM_SetCompare2(TIM1,0);
		 TIM_SetCompare3(TIM1,0);
		 }
		 */

	}
}
