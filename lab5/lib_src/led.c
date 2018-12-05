#include "led.h"
#include "globals.h"

volatile int8_t cycle;

void LED_init(void) {
	cycle = 0;
}

void LED_change_color(uint16_t *rgb, int8_t *direction) {
	uint8_t current_color = (1+cycle/2-cycle%2)%3;
	int8_t move = *direction * (!(cycle%2)-cycle%2);
	rgb[current_color]+=move;
	if((rgb[current_color]==MAX_INTENSITY && move>0) || (rgb[current_color]==0 && move<0))
	{
		cycle += *direction;
		if(cycle>5) cycle = 0;
		else if(cycle<0) cycle = 5;
	}
}

void LED_change_intensity(int32_t *intensity, int8_t *direction) {
	*intensity += *direction;
	if(*intensity > INTENSITY_STEP_COUNT) *intensity = MIN_INTENSITY;
	else if(*intensity < MIN_INTENSITY) *intensity = INTENSITY_STEP_COUNT;
}

uint16_t LED_calc_color(uint16_t *value, int32_t *intensity) {
	return ((*value == MAX_INTENSITY) && (*intensity == INTENSITY_STEP_COUNT)) ? MAX_INTENSITY :
			( (( (uint32_t)(*value) * (*value) / INTENSITY_STEP_COUNT) * (*intensity)) / MAX_INTENSITY);
}

void LED_set_color(uint16_t *rgb,int32_t *intensity) {		
	TIM_SetCompare1(TIM1, LED_calc_color(rgb,intensity));
	TIM_SetCompare2(TIM1, LED_calc_color(rgb+1,intensity));
	TIM_SetCompare3(TIM1, LED_calc_color(rgb+2,intensity));
}
