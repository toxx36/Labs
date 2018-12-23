#include "main.h"
#include "buttons.h"

#define LED_COUNT 4
#define LED_FIRST_PIN GPIO_Pin_12
#define LED_PORT GPIOD

void init(void);

int main(void)
{	
	uint8_t tmp;
	uint8_t mode = 1;
	init();
	ButtonInit();

	while (1) {
		tmp = Button_getClicks(bBoard,1);
		if(tmp == 5 && !Button_holded(bBoard,0)) {
			mode ^= 1;
			GPIO_ResetBits(LED_PORT,LED_FIRST_PIN<<0|LED_FIRST_PIN<<1|LED_FIRST_PIN<<2|LED_FIRST_PIN<<3);
		}
		if(mode == 0) {
			if(tmp > 0 && tmp < 5 && !Button_holded(bBoard,0) && !Button_onPress(bBoard)) {
				GPIO_ToggleBits(LED_PORT,LED_FIRST_PIN<<(tmp-1));
			}
			if(tmp-1 == 0 && Button_onHold(bBoard) && Button_withRepeat(bBoard)) {
				GPIO_ToggleBits(LED_PORT,LED_FIRST_PIN<<0|LED_FIRST_PIN<<1|LED_FIRST_PIN<<2|LED_FIRST_PIN<<3);
			}
			if(tmp-1 == 1 && Button_withRepeat(bBoard) && Button_onHold(bBoard)) {
				GPIO_ToggleBits(LED_PORT,LED_FIRST_PIN<<0);
			}
			if(tmp-1 == 2 && Button_withRepeat(bBoard) && Button_onHold(bBoard)) {
				GPIO_ToggleBits(LED_PORT,LED_FIRST_PIN<<1);
			}
			if(tmp-1 == 3 && Button_withRepeat(bBoard) && Button_onHold(bBoard)) {
				GPIO_ToggleBits(LED_PORT,LED_FIRST_PIN<<2);
			}
			if(tmp-1 == 4 && Button_withRepeat(bBoard) && Button_onHold(bBoard)) {
				GPIO_ToggleBits(LED_PORT,LED_FIRST_PIN<<3);
			}
		}
		else {
			if(Button_onPress(bBoard)) GPIO_SetBits(LED_PORT,LED_FIRST_PIN<<1);
			else GPIO_ResetBits(LED_PORT,LED_FIRST_PIN<<1);
			if(Button_withRepeat(bBoard)) GPIO_ToggleBits(LED_PORT,LED_FIRST_PIN<<3);
			if(Button_released(bBoard)) GPIO_ToggleBits(LED_PORT,LED_FIRST_PIN<<2);
			if(Button_pressed(bBoard)) GPIO_ToggleBits(LED_PORT,LED_FIRST_PIN<<0);
		}
	}
}

void TIM5_IRQHandler() {
	if (TIM_GetITStatus(TIM5, TIM_IT_Update)) {
		Buttons_IRQ();
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
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
	
}