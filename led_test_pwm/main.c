#include "main.h"
#include "init.h"
#include "irq.h"
#include "globals.h"

void TIM2_IRQHandler() {
	if (TIM_GetITStatus(TIM2, TIM_IT_Update)) {
	    IRQ_TIM_LEDFlash();
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}

void EXTI0_IRQHandler(void) {
	if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
        IRQ_EXTI_color();
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}

void EXTI1_IRQHandler(void) {
	if (EXTI_GetITStatus(EXTI_Line1) != RESET) {
        IRQ_EXTI_intens();
		EXTI_ClearITPendingBit(EXTI_Line1);
	}
}

int main(void) {
    init_all();  
	while (1) {    
	}
}
