#include "adc.h"


#define GIOAEN	  		(1U << 0) // to enable clock for GPIO A
#define ADC1EN    		(1U << 8)	// enable ADC clock
#define CR2_ADCON 		(1U << 0) //enable ADC conversion
#define CR2_CONT  	    (1U << 1) //enable continuous conversion
#define CR2_STARTCONV   (1U << 30) //enable continuous conversion
#define SR_EOC 		    (1U << 1) //end of conversion bit

void pa1AdcInit(){
	/*configure ADC GPIO Pin */
	/*enable clock access to GPIOA*/
	RCC->AHB1ENR |= GIOAEN;
	/*set PA1 mode to analog mode */
	GPIOA->MODER |=  (3U << 2);
	/*Configure ADC module*/
	/* enable clock access to adc module*/
	RCC->APB2ENR |= ADC1EN;
	/*Set start of conversion sequence*/
	ADC1->SQR3 = 1u;
	/*set conversion sequence length(specify which channels to use for conversion)*/
	ADC1->SQR1 = 0; //single conversion
	/*enable adc module*/
	ADC1->CR2 |= CR2_ADCON;
}

void startAdcConversion(){
	/*enable continuous adc conversion*/
	ADC1->CR2 |= CR2_CONT;
	/*start adc conversion*/
	ADC1->CR2 |= CR2_STARTCONV;

}

uint32_t adcRead(){
	/*wait for conversion end*/
	while(!(ADC1->SR & SR_EOC)){}
	/*Read converted result*/
	return (ADC1->DR);
}


rx_dataType RX_FIFO[RXFIFO_SIZE];
volatile rx_dataType *  rxPutPt;
volatile rx_dataType *  rxGetPt;

/*Initialize the FIFO*/

void rxFifoInit(){
	/*Reset FIFO*/
	rxPutPt = rxGetPt = &RX_FIFO[0];

}

/*Put data into RX FIFO*/
uint8_t rxFifoPut(rx_dataType data){
	rx_dataType volatile * rxNextPutPt;
	rxNextPutPt = rxPutPt+1;
	/*Check if at the end of Fifo and wrap around*/
	if (rxNextPutPt == &RX_FIFO[RXFIFO_SIZE]){
		rxNextPutPt = &RX_FIFO[0];
	}

	if(rxNextPutPt == rxGetPt){
		return (RXFIFO_FAIL);
	}else{
		/*put data in FIFO*/
		*(rxPutPt) = data;
		rxPutPt = rxNextPutPt;
		return (RXFIFO_OK);

	}
}

/*Get data from FIFO*/

uint8_t rxFifoGet(rx_dataType * dataPt){
	/*Check if FiFo is Empty*/
	if(rxPutPt == rxGetPt){
		/*Fifo Empty*/
		return (RXFIFO_FAIL);
	}

	/*Get data from FIFO*/
	*dataPt = *(rxGetPt++);

	/*Check if at the end of Fifo and wrap around*/
	if (rxGetPt == &RX_FIFO[RXFIFO_SIZE]){
		rxGetPt = &RX_FIFO[0];
	}

	return (RXFIFO_OK);
}
