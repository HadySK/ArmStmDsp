#include <stdint.h>

#ifndef __SYSTICK_H
#define __SYSTICK_H

#ifndef STM32F446xx
#define STM32F446xx
#include "stm32f4xx.h"
#endif
//default systick 16mhz
//#define CLOCKSPEED_HZ 16000000.0f

// systick should be HCLK/8, HCLK changed to 160mhz
//and we changed clksource to external so it shouldbe HCLK no divide over 8
#define CLOCKSPEED_HZ 160000000.0f
void systickCounterInit();
uint32_t GetSysTickClockFreq(void);
#endif /*__SYSTICK_H*/
