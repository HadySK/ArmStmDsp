

//symbol to use cm4 math lib
#define ARM_MATH_CM4
//enable fpu for CMSIS
#define __FPU_PRESENT
#include <stdio.h>

#define STM32F446xx
#include "stm32f4xx.h"
#include "stm32f446xx.h"
#include "signals.h"
#include "uart.h"
#include "systick.h"
#include "arm_math.h"
#include "arm_sorting.h"
#include "miscfn.h"
#include "clkcfg.h"
#include "adc.h"
#include "filters.h"

extern float _5hz_signal[HZ_5_SIG_LEN];
extern float32_t inputSignal_f32_1kHz_15kHz[KHZ_15_SIG_LEN];
float g_insigSample;
extern float32_t  impulse_response[IMP_RSP_LEN];
float g_impRspSample;
float32_t  outputSigArr[HZ_10_100_500HZ_SIGLEN+ LPF_70HZ_IMP_RESP_LEN -1];
//float32_t  outputSigRunSum[KHZ_15_SIG_LEN];
uint32_t timeBefore, timeAfter, timeTaken;
float32_t timeS, timeMs;
float32_t timeS_CMSIS, timeMs_CMSIS;


extern float32_t _640_points_ecg_[ECG_SIG_LEN];

arm_rfft_fast_instance_f32 fftHandler;
#define FFTLENGTH (512)

extern float32_t IpSig_10hz_100hz_500hz[HZ_10_100_500HZ_SIGLEN];
extern float32_t Lpf_70Hz_impulseResponse[LPF_70HZ_IMP_RESP_LEN];
extern float32_t Hpf_400hz_impulseResponse[HPF_400HZ_IMP_RESP_LEN];
volatile float32_t  reX2[HZ_10_100_500HZ_SIGLEN/2]; //real part of DFT is half the length of i/p signal
volatile float32_t  imX2[HZ_10_100_500HZ_SIGLEN/2]; //imaginary part of DFT is half the length of i/p signal

volatile float32_t  outputArrDFTCMSIS2[HZ_10_100_500HZ_SIGLEN];

float32_t magnitude[HZ_10_100_500HZ_SIGLEN/2 + 1];

volatile float32_t  outputArrMvg[KHZ_15_SIG_LEN];
volatile uint32_t sensorValue;
volatile uint32_t filteredSensorVal;

extern float LP_1HZ_2HZ_IMPULSE_RESPONSE[IMP_RSP_LENGTH];
fir_filter lpfFir;

void convTesting();
int main(void)
{
	/*Enable FPU*/
	SCB->CPACR |= ((3U << 20) | (3U << 22));
	/*Setclock tree*/
	clk100MhzCfg();
	/*init uart*/
	uart2_Tx_Init();

	/*Initialize SysTick Counter*/
	systickCounterInit();
	//init adc
	pa1AdcInit();
	//start conversion
	startAdcConversion();

	/*Init FIFO*/
	rxFifoInit();

	uint8_t rxFifoPut(rx_dataType data);
	uint8_t rxFifoGet(rx_dataType * dataPt);
	/*Initialize FIR filter*/
	firFilterInit(&lpfFir, LP_1HZ_2HZ_IMPULSE_RESPONSE, IMP_RSP_LENGTH);
	//init fft handler
	arm_rfft_fast_init_f32(&fftHandler, FFTLENGTH);
	/*CMSIS DSP API*/

	arm_rfft_fast_f32	(	&fftHandler,
	(float32_t * )	IpSig_10hz_100hz_500hz,
	(float32_t * )	outputArrDFTCMSIS2,
	0
	);

	//getDftOpMagnitudeCMSIS(outputArrDFTCMSIS2, (HZ_10_100_500HZ_SIGLEN/2));
	arm_cmplx_mag_f32(outputArrDFTCMSIS2, magnitude, HZ_10_100_500HZ_SIGLEN/2);
	serialPlotReX((float32_t  *)magnitude,(uint32_t) (HZ_10_100_500HZ_SIGLEN/2));

	movingAverage( inputSignal_f32_1kHz_15kHz,  outputArrMvg,
			KHZ_15_SIG_LEN, MAFLTR_PTS);
	//serialPlotReXCMSIS(outputArrDFTCMSIS2, (HZ_10_100_500HZ_SIGLEN/2));

	//serialPlotSignal((float32_t  *) outputSigArr,
			//	(uint32_t)  (HZ_10_100_500HZ_SIGLEN+ LPF_70HZ_IMP_RESP_LEN -1));
    /* Loop forever */
 	while(1){
 		/*serialPlotDFTIDFT((float32_t  *)inputSignal_f32_1kHz_15kHz,(uint32_t)   KHZ_15_SIG_LEN,
 			(float32_t  *)outputArrMvg,(uint32_t)   KHZ_15_SIG_LEN);*/
 		//serialPlotReXCMSIS(outputArrDFTCMSIS2, (HZ_10_100_500HZ_SIGLEN/2));

 		sensorValue = adcRead();
 		filteredSensorVal = firFilterUpdate(&lpfFir, sensorValue);
 		printf("%d,", (int)sensorValue);
 		printf("%d\n\r", (int)filteredSensorVal);
 		delayFn(100000);

 	}

}
void convTesting(){

	float32_t  reX[ECG_SIG_LEN/2];
	float32_t  imX[ECG_SIG_LEN/2];

	float32_t  outputSigArrECG[ECG_SIG_LEN];
	float32_t  outputSigArrDFT[ECG_SIG_LEN];
	/*Manual API*/
	timeBefore = SysTick->VAL;
	convSig((float32_t  *)inputSignal_f32_1kHz_15kHz,
			(float32_t  *)outputSigArr,
			(float32_t  *)impulse_response,
			(uint32_t)   KHZ_15_SIG_LEN,
			(uint32_t)  IMP_RSP_LEN);

	timeAfter = SysTick->VAL;
		if (timeAfter >= timeBefore) {
				timeTaken =  timeAfter - timeBefore;
			} else {
				timeTaken = timeBefore - timeAfter;
			}
	timeS =  timeTaken / CLOCKSPEED_HZ;
	timeMs= timeS * 1000.0f;
	 /*CMSIS DSP API*/
	timeBefore = SysTick->VAL;
	 arm_conv_f32((float32_t  *)inputSignal_f32_1kHz_15kHz,
							(uint32_t)   KHZ_15_SIG_LEN,
							(float32_t  *)impulse_response,
							(uint32_t)  IMP_RSP_LEN,
							(float32_t  *)outputSigArr);
	timeAfter = SysTick->VAL;
	if (timeAfter >= timeBefore) {
			timeTaken =  timeAfter - timeBefore;
		} else {
			timeTaken = timeBefore - timeAfter;
		}
	timeS_CMSIS =  timeTaken / CLOCKSPEED_HZ;
	timeMs_CMSIS= timeS_CMSIS * 1000.0f;


	uint32_t outputSigLength = KHZ_15_SIG_LEN+ IMP_RSP_LEN -1;

	timeBefore = SysTick->VAL;
	//compute signal DFT
	calcSigDFT((float32_t  *)_640_points_ecg_,
			 (float32_t  *)reX,
			 (float32_t  *)imX,
			 (uint32_t)   ECG_SIG_LEN);


	//computer signal IDFT
	 calcSigIDFT((float32_t  *)outputSigArrECG,
			 (float32_t  *)reX,
			 (float32_t  *)imX,
			 (uint32_t)   ECG_SIG_LEN);


	 timeAfter = SysTick->VAL;
	 		if (timeAfter >= timeBefore) {
	 				timeTaken =  timeAfter - timeBefore;
	 			} else {
	 				timeTaken = timeBefore - timeAfter;
	 			}
	 	timeS =  timeTaken / CLOCKSPEED_HZ;
	 	timeMs= timeS * 1000.0f;
	 	serialPlotDFTIDFT((float32_t  *)_640_points_ecg_,(uint32_t)   ECG_SIG_LEN,
	 	 				(float32_t  *)outputSigArrECG,(uint32_t)   ECG_SIG_LEN);

	 	//init fft handler
	 	arm_rfft_fast_init_f32(&fftHandler, FFTLENGTH);
	 	 /*CMSIS DSP API*/
	 	timeBefore = SysTick->VAL;

	 	arm_rfft_fast_f32	(	&fftHandler,
	 	(float32_t * )	_640_points_ecg_,
	 	(float32_t * )	outputSigArrDFT,
	 	0
	 	);

	 	arm_rfft_fast_f32	(	&fftHandler,
	 		 	(float32_t * )	outputSigArrDFT,
	 		 	(float32_t * )	outputSigArrECG,
	 	1
	 	);
	 timeAfter = SysTick->VAL;
	 	if (timeAfter >= timeBefore) {
	 			timeTaken =  timeAfter - timeBefore;
	 		} else {
	 			timeTaken = timeBefore - timeAfter;
	 		}
	 	timeS_CMSIS =  timeTaken / CLOCKSPEED_HZ;
	 	timeMs_CMSIS= timeS_CMSIS * 1000.0f;
	//getDftOpMagnitude((float32_t  *)reX,(uint32_t)   ECG_SIG_LEN);
	//serialPlotReX((float32_t  *)reX,(uint32_t)   ECG_SIG_LEN);

	 	/* arm_conv_f32((float32_t  *)IpSig_10hz_100hz_500hz,
	 								(uint32_t)   HZ_10_100_500HZ_SIGLEN,
	 								(float32_t  *)Hpf_400hz_impulseResponse,
	 								(uint32_t)  HPF_400HZ_IMP_RESP_LEN,
	 								(float32_t  *)outputSigArr);
	 		//compute signal DFT
	 		calcSigDFT((float32_t  *)IpSig_10hz_100hz_500hz,
	 				  (float32_t  *)reX2,
	 				  (float32_t  *)imX2,
	 				  (uint32_t)   HZ_10_100_500HZ_SIGLEN);

	 		getDftOpMagnitude((float32_t  *)reX2,(uint32_t) (HZ_10_100_500HZ_SIGLEN/2));
	 		//arm_cmplx_mag_f32(reX2, imX2, HZ_10_100_500HZ_SIGLEN/2);
	 		serialPlotReX((float32_t  *)reX2,(uint32_t) (HZ_10_100_500HZ_SIGLEN/2));*/
}
