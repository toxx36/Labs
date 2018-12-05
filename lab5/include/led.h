#ifndef LED_H
#define LED_H

#ifndef GLOBALS_H
#include "globals.h"
#endif

#define MAX_INTENSITY TIM_PERIOD
#define INTENSITY_STEP_COUNT 5
#define MIN_INTENSITY 1

void LED_init(void);
void LED_change_color(uint16_t*, int8_t*);
void LED_change_intensity(int32_t*,int8_t*);
uint16_t LED_calc_color(uint16_t*, int32_t*);
void LED_set_color(uint16_t*,int32_t*);

#endif