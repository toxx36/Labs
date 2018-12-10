#ifndef LED_H
#define LED_H

#ifndef GLOBALS_H
#include "globals.h"
#endif

#define MAX_INTENSITY TIM_PERIOD
#define INTENSITY_STEP_COUNT 5
#define MIN_INTENSITY 1

void LED_init(void);
void LED_change_color(uint16_t *intensity, int8_t *direction);
void LED_change_intensity(int32_t *intensity, int8_t *direction);
uint16_t LED_calc_color(uint16_t *value, int32_t *intensity);
void LED_set_color_raw(uint16_t *color, int32_t *intensity);
void LED_set_color_HEX(uint32_t color, int32_t intensity);
void LED_set_color_RGB(uint16_t *color, int32_t intensity);
void LED_8bit_conversion(uint32_t *color);

#endif