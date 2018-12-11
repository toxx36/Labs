#include "main.h"

#define LED_COUNT 4
#define LED_FIRST_PIN GPIO_Pin_12
#define LED_PORT GPIOD

static volatile uint8_t flag;

void init(void);
void LED_switch(uint8_t turn);

int main(void)
{	
	init();
	
	while (1) {
	__WFI();
  }
}

//led flash handler
void TIM2_IRQHandler() {
	if (TIM_GetITStatus(TIM2, TIM_IT_Update)) {
		LED_switch(flag);
		flag = 0;
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}

//button handler
void EXTI0_IRQHandler(void) {
	if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
		flag = 1; //if button pressed set flag to reverse flashing direction
		//debounce isn't necessary
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}


void init(void) {
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	NVIC_InitTypeDef NVIC_InitStruct = {0};
	EXTI_InitTypeDef EXTI_InitStruct = {0};

	/* Init LEDs on discovery board */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* Init user button on discovery board */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
	
	//init TIM2 for 1 second
	//Update Event (Hz) = timer_clock / ((TIM_Prescaler + 1) * (TIM_Period + 1)) == 1 HZ
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_TimeBaseInitTypeDef TimBaseInit = {0};
	TimBaseInit.TIM_Prescaler = 42000 - 1;
	TimBaseInit.TIM_CounterMode = TIM_CounterMode_Up;
	TimBaseInit.TIM_Period = 2000 - 1;
	TimBaseInit.TIM_ClockDivision = 0;
	TIM_TimeBaseInit(TIM2, &TimBaseInit);
	
	//init TIM2 interrupt
	NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	TIM_Cmd(TIM2,ENABLE);	
	
	//init button interrupt
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
	
	NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	EXTI_InitStruct.EXTI_Line = EXTI_Line0;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStruct);
}

void LED_switch(uint8_t turn) {
	static int8_t current_led = 0;
	static int8_t direction = 1;
	if(turn) direction = direction ? 0 : 1; //change direction if needed
	current_led += direction ? -1 : 1; //which led will be turned on next
	if(current_led >= LED_COUNT) current_led = 0; //overflow led count solve
	else if(current_led < 0) current_led = LED_COUNT-1; //underflow led count
	GPIO_Write(LED_PORT, LED_FIRST_PIN<<current_led); //write led states to port
}
