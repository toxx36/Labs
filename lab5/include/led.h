#ifndef LED_H
#define LED_H

#ifndef GLOBALS_H
#include "globals.h"
#endif

#define USING_TIMER TIM1
#define MAX_INTENSITY TIM_PERIOD
#define INTENSITY_STEP_COUNT 5
#define MIN_INTENSITY 1

void LED_init_val(void);
void LED_init_periph(void);
uint16_t LED_calc_color(uint16_t *value);
void LED_set_color_raw(uint16_t *rgb);
void LED_set_intensity(uint16_t intensity);
void LED_set_color_RGB(uint8_t *rgb);
void LED_set_color_HEX(uint32_t hex);

#endif