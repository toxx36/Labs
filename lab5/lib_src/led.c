#include "led.h"
#include "globals.h"

static uint16_t intens;
static uint16_t rgb_curr[3];

void LED_init(void) {
	rgb_curr[0] = 0;
	rgb_curr[1] = 0;
	rgb_curr[2] = 0;
	LED_set_intensity(INTENSITY_STEP_COUNT);
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
	TIM_SetCompare1(TIM1, LED_calc_color(rgb));
	TIM_SetCompare2(TIM1, LED_calc_color(rgb+1));
	TIM_SetCompare3(TIM1, LED_calc_color(rgb+2));
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
