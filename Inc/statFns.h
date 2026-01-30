/*
 * statFns.h
 *
 *  Created on: Oct 29, 2025
 *      Author: hadya
 */

#ifndef STATFNS_H_
#define STATFNS_H_

#include "arm_math.h"

float32_t signalMean(float32_t * sigSrcArr, uint32_t sigLength);
float32_t signalVariance(float32_t * sigSrcArr, float32_t sigMean, uint32_t sigLength);
float32_t signalStdDeviation(float32_t sigVariance);


#endif /* STATFNS_H_ */
