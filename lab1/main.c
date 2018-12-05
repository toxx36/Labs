#include "main.h"

#define PERIOD 2000000
#define LED_COUNT 4
#define LED_FIRST_PIN GPIO_Pin_12
#define LED_PORT GPIOD

void init(void);
void delay(uint32_t);
void LED_switch(uint8_t);

int main(void)
{
	uint8_t turn = 0;
	
	init();
	
	while (1) {
	turn = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0); //read button state
	LED_switch(turn);
	delay(PERIOD);
  }
}

void init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

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
}

void delay(uint32_t time) {
	for(; time > 0; time--);
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
