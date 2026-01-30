#include "systick.h"

#define CTRL_ENABLE (1U << 0)
#define CTRL_CLKSRC (1U << 2)


void systickCounterInit(){
	/*Disable SysTick before config*/
	SysTick->CTRL = 0u;
	/*Load SysTick reload value register with max value*/
	SysTick->LOAD =0x00FFFFFFu;
	/*Clear SysTick current value register*/
	SysTick->VAL = 0x0u;

	/*Select Internal clock src*/
	SysTick->CTRL |= CTRL_CLKSRC;
	/*Enable SysTick*/
	SysTick->CTRL |= CTRL_ENABLE;
}
