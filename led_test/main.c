#include "main.h"

#define CHANGE_SPEED 50
#define LED_COUNT 3
#define LED_FIRST_PIN GPIO_Pin_8
#define BTN_INC_PIN GPIO_Pin_0
#define BTN_DEC_PIN GPIO_Pin_1

void PWM(uint8_t duty, uint16_t pin, uint16_t dly)
{
	uint16_t period;
	for(;dly>0;dly--)
	{
		if(duty==0) GPIO_SetBits(GPIOA, pin);
		else if(duty==0xFF) GPIO_ResetBits(GPIOA, pin);
		else
		{
			GPIO_ResetBits(GPIOA, pin);
			for(period = 0; period<0xFF; period++)
			{
			 if(duty==period) GPIO_SetBits(GPIOA, pin);
			}
		}
	}
}


void TIM2_IRQHandler()
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update))
    {
	GPIO_ToggleBits(GPIOD,GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);   
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}


int main(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable peripheral clock for LEDs port */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

  /* Init LEDs */
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

  GPIO_Init(GPIOD, &GPIO_InitStructure);

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  /* Init LEDs */
  GPIO_InitStructure.GPIO_Pin   = LED_FIRST_PIN | LED_FIRST_PIN<<1 | LED_FIRST_PIN<<2;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

  GPIO_Init(GPIOA, &GPIO_InitStructure);


  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

  /* Init buttons */
  GPIO_InitStructure.GPIO_Pin   = BTN_INC_PIN | BTN_DEC_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;

  GPIO_Init(GPIOE, &GPIO_InitStructure);

  GPIO_ResetBits(GPIOA, LED_FIRST_PIN);
  GPIO_SetBits(GPIOA, LED_FIRST_PIN<<1);
  GPIO_SetBits(GPIOA, LED_FIRST_PIN<<2);

  int8_t button, cycle, move = 0;
  uint8_t duty = 0;


//Update Event (Hz) = timer_clock / ((TIM_Prescaler + 1) * (TIM_Period + 1)) == 1 HZ
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  TIM_TimeBaseInitTypeDef TimBaseInit;
  TimBaseInit.TIM_Prescaler = 42000-1;
  TimBaseInit.TIM_CounterMode = TIM_CounterMode_Up;
  TimBaseInit.TIM_Period = 2000-1;
  TimBaseInit.TIM_ClockDivision = 0;
  
  TIM_TimeBaseInit(TIM2, &TimBaseInit);
  
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

//IT config
  NVIC_InitTypeDef NVIC_InitStruct;
  NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStruct);
 

  TIM_Cmd(TIM2,ENABLE);



  while (1) {
	button = GPIO_ReadInputDataBit(GPIOE, BTN_INC_PIN)-GPIO_ReadInputDataBit(GPIOE, BTN_DEC_PIN);
	if(button)
		{
			move=button*(!(cycle%2)-cycle%2);
			if((duty==0xFF && move>0) || (duty==0 && move<0))
			{
				cycle-=button;
				if(cycle<0) cycle+=LED_COUNT*2;
				duty-=move;
			}
			else duty+=move;
		}
	PWM((duty*duty)/duty,LED_FIRST_PIN<<((1+cycle/2-cycle%2)%LED_COUNT),CHANGE_SPEED);
  }
}
