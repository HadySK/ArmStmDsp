#ifndef __ADC_H
#define __ADC_H

#include <stdint.h>

#ifndef STM32F446xx
#define STM32F446xx
#endif
#include "stm32f4xx.h"



#define RXFIFO_SIZE (300u)
#define RXFIFO_FAIL (0u)
#define RXFIFO_OK   (1u)

typedef uint32_t rx_dataType;


void pa1AdcInit();
void startAdcConversion();
uint32_t adcRead();

void rxFifoInit();
uint8_t rxFifoPut(rx_dataType data);
uint8_t rxFifoGet(rx_dataType * dataPt);
#endif
