/*
 * statFns.c
 *
 *  Created on: Oct 29, 2025
 *      Author: hadya
 */

#include "statFns.h"

/*
float32_t g_meanValue;
float32_t g_varianceValue;
float32_t g_stdDevValue;
float32_t g_stdDevValueCMSIS;
*/
float32_t signalMean(float32_t * sigSrcArr, uint32_t sigLength){
	float32_t _mean = 0.0;
	for(uint32_t i = 0; i < sigLength; i++){
		_mean +=sigSrcArr[i];
	}
	_mean = _mean/(float32_t)sigLength;
	return _mean;

}


float32_t signalVariance(float32_t * sigSrcArr, float32_t sigMean, uint32_t sigLength){
	float32_t _variance = 0.0;
	for(uint32_t i = 0; i < sigLength; i++){
		_variance +=powf((sigSrcArr[i]-sigMean), 2);
	}
	_variance = _variance/(float32_t)(sigLength-1);
	return _variance;

}
float32_t signalStdDeviation(float32_t sigVariance){
	float32_t stdDev = sqrt(sigVariance);
	return stdDev;
}


