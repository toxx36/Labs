#ifndef BUTTONS_H
#define BUTTONS_H

#include <stm32f4xx.h>

///Time in ms
#define BOUNCE_TIME 40
#define HOLD_TIME 800
#define HOLD_REPEAT_TIME 250
#define CLICK_TIME 300

//Count and names of buttons
#define BTN_COUNT 1
typedef enum {bBoard} Buttons;

void ButtonInit(void);
void Buttons_IRQ(void);
uint8_t Button_onPress(Buttons cur);
uint8_t Button_pressed(Buttons cur);
uint8_t Button_onHold(Buttons cur);
uint8_t Button_holded(Buttons cur, uint8_t once);
uint8_t Button_released(Buttons cur);
uint8_t Button_withRepeat(Buttons cur);
uint8_t Button_clicks(Buttons cur,uint8_t count);
uint8_t Button_getClicks(Buttons cur,uint8_t consider_hold);

#endif
