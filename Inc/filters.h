#ifndef __FILTERS_H
#define __FILTERS_H
#include <stdint.h>
#include "arm_math.h"

#define FIR_FILTER_LEN 10

typedef struct{
	float32_t *buff;
	float32_t * kernel;
	float32_t out;
	uint32_t buffIdx;
	uint32_t kernelLen;

}fir_filter;

void firFilterInit(fir_filter * fir, float32_t *fltrKernel, uint32_t kernelLen);
float32_t firFilterUpdate(fir_filter * fir, float32_t currSample);
#endif /*__FILTERS_H*/
