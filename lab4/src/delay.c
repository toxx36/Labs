#include "delay.h"

void delay(uint32_t time) {
	for (; time > 0; time--)
		;
}