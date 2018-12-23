#include "buttons.h"

typedef enum {bWaiting,bDebouncing,bClicksCounting,bHolding,bWaitRelease} ButtonsState; ///States of state machine

///The order matches the names of the buttons
const uint16_t pins[] = 		{GPIO_Pin_0};
const GPIO_TypeDef *ports[] = 	{GPIOA};
const GPIOPuPd_TypeDef pupd[] = {GPIO_PuPd_DOWN};
const uint32_t rcc[] = {RCC_AHB1Periph_GPIOA};

typedef struct button_state ///Contains all about button
{
	ButtonsState bState;
	uint8_t bOnPress;
	uint8_t bPressed;
	uint8_t bOnHold;
	uint8_t bHolded;
	uint8_t bReleased;
	uint8_t bHoldRepeat;
	uint8_t bClicks;
	uint32_t bClickTimer;
	uint32_t bTimer;
	uint16_t bPin;
	GPIO_TypeDef *bPort;
} ButtonState;

//Contains declared buttons
ButtonState btns[BTN_COUNT];

inline void ButtonInit(void)
{
	Buttons cur;
	uint8_t cnt;

	GPIO_InitTypeDef GPIO_InitStructure = {0};
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;

	//Enable RSS's
	for(cnt = 0; cnt < sizeof(rcc)/sizeof(rcc[0]); cnt++) {
		RCC_AHB1PeriphClockCmd((uint32_t)rcc[cnt], ENABLE);//need to add autoinit
	}

	//Init buttons
	for(cur = 0; cur < BTN_COUNT; cur++)
	{
		btns[cur].bOnPress = 0;
		btns[cur].bPressed = 0;
		btns[cur].bOnHold = 0;
		btns[cur].bHolded = 0;
		btns[cur].bReleased = 0;
		btns[cur].bHoldRepeat = 0;
		btns[cur].bClicks = 0;
		btns[cur].bClickTimer = 0;
		btns[cur].bState = bWaiting;
		btns[cur].bTimer = 0;
		btns[cur].bPin = (uint16_t)pins[cur];
		btns[cur].bPort = (GPIO_TypeDef *)ports[cur];
		GPIO_InitStructure.GPIO_PuPd = (GPIOPuPd_TypeDef)pupd[cur];
		GPIO_InitStructure.GPIO_Pin = btns[cur].bPin;
		GPIO_Init(btns[cur].bPort, &GPIO_InitStructure);
	}	
	//init TIM5 for 1ms tick interrupt
	//Update Event (Hz) = timer_clock / ((TIM_Prescaler + 1) * (TIM_Period + 1))
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	TIM_TimeBaseInitTypeDef TimBaseInit;
	TimBaseInit.TIM_Prescaler = 42000 - 1;
	TimBaseInit.TIM_CounterMode = TIM_CounterMode_Up;
	TimBaseInit.TIM_Period = 2 - 1;
	TimBaseInit.TIM_ClockDivision = 0;
	TIM_TimeBaseInit(TIM5, &TimBaseInit);
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);

	//IRQ TIM5 config
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);

	TIM_Cmd(TIM5, ENABLE);

}

///Button state machine which process each button
void Button_FSM(Buttons cur) {
	uint8_t cur_btn = GPIO_ReadInputDataBit(btns[cur].bPort,btns[cur].bPin); //read current btn state
	switch(btns[cur].bState) {
	case bWaiting:
		if(cur_btn) {
			btns[cur].bTimer = 0;
			btns[cur].bPressed = 0;
			btns[cur].bState = bDebouncing;
		}
		if(btns[cur].bClickTimer != 0) {
			if(btns[cur].bClickTimer >= CLICK_TIME) {
				btns[cur].bClickTimer = 0;
				//btns[cur].bClicks = 0;
			}
			else {
				btns[cur].bClickTimer++;
			}
		}
		break;
	case bDebouncing:
		btns[cur].bTimer++;
		if(btns[cur].bTimer >= BOUNCE_TIME) {
			if(cur_btn) {
				if(btns[cur].bClickTimer == 0) {
					btns[cur].bClicks = 0;
				}
				btns[cur].bOnPress = 1;
				btns[cur].bPressed = 1;
				btns[cur].bReleased = 0;
				btns[cur].bHolded = 0;
				btns[cur].bHoldRepeat = 1;
				btns[cur].bClickTimer = BOUNCE_TIME;
				btns[cur].bState = bClicksCounting;
			}
			else {
				btns[cur].bState = bWaiting;
			}
		}
		break;
	case bClicksCounting:
		btns[cur].bClickTimer++;
		if(cur_btn) {
			btns[cur].bTimer++;
			if(btns[cur].bClickTimer >= CLICK_TIME){
				btns[cur].bClicks += 1;
				btns[cur].bClickTimer = 0;
				btns[cur].bState = bHolding;
			}
		}
		else {
			btns[cur].bOnPress = 0;
			btns[cur].bReleased = 1;
			btns[cur].bClicks++;
			btns[cur].bState = bWaiting;
		}
		break;
	case bHolding:
		if(cur_btn) {
			btns[cur].bTimer++;
			if(btns[cur].bTimer >= HOLD_TIME){
				btns[cur].bOnHold = 1;
				btns[cur].bHolded = 1;
				btns[cur].bHoldRepeat = 1;
				btns[cur].bTimer = 0;
				btns[cur].bState = bWaitRelease;
			}
		}
		else {
			btns[cur].bOnPress = 0;
			btns[cur].bReleased = 1;
			btns[cur].bState = bWaiting;
		}
		break;
	case bWaitRelease:
		if(cur_btn) {
			btns[cur].bTimer++;
			if(btns[cur].bTimer >= HOLD_REPEAT_TIME) {
				btns[cur].bHoldRepeat = 1;
				btns[cur].bTimer = 0;
			}
		}
		else {
			btns[cur].bOnPress = 0;
			btns[cur].bOnHold = 0;
			btns[cur].bReleased = 1;
			btns[cur].bHoldRepeat = 0;
			btns[cur].bState = bWaiting;
		}
		break;
	default: break;
	}
}

///Func which process all buttons
void Buttons_IRQ(void) {
	Buttons cur;
	for(cur = 0; cur < BTN_COUNT; cur++) {
		Button_FSM(cur);
	}
}

///Is the button pressed now
uint8_t Button_onPress(Buttons cur) {
	if(btns[cur].bOnPress == 1) {
		return 1;
	}
	return 0;
}

///Was the button pressed last time
uint8_t Button_pressed(Buttons cur) {
	if(btns[cur].bPressed == 1) {
		btns[cur].bPressed = 0;
		return 1;
	}
	return 0;
}

///Is the button on hold
uint8_t Button_onHold(Buttons cur) {
	if(btns[cur].bOnHold == 1) {
		return 1;
	}
	return 0;
}

///Was the button released
uint8_t Button_released(Buttons cur) {
	if(btns[cur].bReleased == 1) {
		btns[cur].bReleased = 0;
		return 1;
	}
	return 0;
}

///Repeats button pressing over time, like PC keyboard
uint8_t Button_withRepeat(Buttons cur) {
	if(btns[cur].bHoldRepeat == 1) {
		btns[cur].bHoldRepeat = 0;
		return 1;
	}
	return 0;
}

///Returns true if count value matches the clicks
uint8_t Button_clicks(Buttons cur,uint8_t count) {
	if(btns[cur].bClicks == count) {
		btns[cur].bClickTimer = 0;
		btns[cur].bClicks = 0;
		return 1;
	}
	return 0;
}

///Was button holded
///\params once don't clear this state after reading
uint8_t Button_holded(Buttons cur, uint8_t once) {
	if(btns[cur].bHolded) {
		if(once) {
			btns[cur].bHolded = 0;
		}
		return 1;
	}
	return 0;
}

///Returns count of clicks
///\params consider_hold for using with Button_withRepeat function
uint8_t Button_getClicks(Buttons cur,uint8_t consider_hold) {
	uint8_t tmp;
	if(btns[cur].bClicks > 0 && btns[cur].bClickTimer == 0) {
		if(consider_hold) {
			if(btns[cur].bState == bHolding) {
				return 0;
			}
			else if(btns[cur].bState == bWaitRelease) {
				return btns[cur].bClicks;
			}
			else if(btns[cur].bHolded == 0 && btns[cur].bState == bWaiting) {
				tmp = btns[cur].bClicks;
				btns[cur].bClicks = 0;
				return tmp;
			}
		}
		else {
			tmp = btns[cur].bClicks;
			btns[cur].bClicks = 0;
			return tmp;
		}
	}
	return 0;
}
