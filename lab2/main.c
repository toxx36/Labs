#include "main.h"

#define LED_COUNT 4
#define LED_FIRST_PIN GPIO_Pin_12
#define LED_PORT GPIOD

void init(void);
void LED_switch(uint8_t);

int main(void)
{
	uint8_t turn = 0;
	
	init();
	
	while (1) {
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1) turn = 1; //read button state
	if(TIM_GetFlagStatus(TIM2,TIM_FLAG_Update)) { //IF for button detection between toggles
		TIM_ClearFlag(TIM2,TIM_FLAG_Update);
		LED_switch(turn); //toggle when delay triggered
		turn = 0;
	}
  }
}

void init(void) {
	GPIO_InitTypeDef GPIO_InitStructure = {0};

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
	TIM_Cmd(TIM2,ENABLE);
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
