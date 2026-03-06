/*
 * driver_temp.c
 *
 *  Created on: Jan 6, 2022
 *      Author: Marko Micovic
 */

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "adc.h"
#include "tim.h"
#include "gpio.h"

// MAX VOLTAGE = 5V
#define MAX_VOLTAGE 5.0
// ADC RESOLUTION = 2^12 = 4096 (12-bit resolution)
#define RESOLUTION 4096.0

static TaskHandle_t TEMP_TaskHandle;
static QueueHandle_t TEMP_MailboxHandle;
static QueueHandle_t AZIM_MailboxHandle;

static float voltageThresholds[16] =
{ 0.52635, 0.8136, 1.0611, 1.36555, 1.74565, 1.9979, 2.4099, 2.88215, 3.3071,
		3.7009, 3.95155, 4.24015, 4.4637, 4.56805, 4.6344, 5 };
static float windBearings[16] =
{ 270, 315, 292.5, 0, 337.5, 225, 247.5, 45, 22.5, 180, 202.5, 135, 157.5, 90,
		67.5, 112.5 };
static float voltage2WindBearing(float voltage)
{
	for (uint32_t i = 0; i < 15; i++)
	{
		if (voltage < voltageThresholds[i])
		{
			return windBearings[i];
		}
	}
	return windBearings[15];
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	BaseType_t woken = pdFALSE;
	vTaskNotifyGiveFromISR(TEMP_TaskHandle, &woken);
	portYIELD_FROM_ISR(woken);
}

static void TEMP_Task(void *parameter)
{
	while (1)
	{
		//TEMP
		HAL_ADC_Start_IT(&hadc1);
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		float value = HAL_ADC_GetValue(&hadc1); // get raw conversion result
		value *= MAX_VOLTAGE / RESOLUTION; // translate into the voltage value
		value *= 100; // translate into temperature value (problem specific)

		xQueueOverwrite(TEMP_MailboxHandle, &value);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
		vTaskDelay(pdMS_TO_TICKS(200));

		//AZIM
		HAL_ADC_Start_IT(&hadc1);
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		value = HAL_ADC_GetValue(&hadc1); // get raw conversion result
		value *= MAX_VOLTAGE / RESOLUTION; // translate into the voltage value
		value = voltage2WindBearing(value);

		xQueueOverwrite(AZIM_MailboxHandle, &value);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
		vTaskDelay(pdMS_TO_TICKS(200));
	}
}

float TEMP_GetCurrentValue()
{
	float result = 0.0;
	xQueuePeek(TEMP_MailboxHandle, &result, portMAX_DELAY);
	return result;
}

float AZIM_GetCurrentValue(){
	float result=0.0;
	xQueuePeek(AZIM_MailboxHandle, &result, portMAX_DELAY);
	return result;
}
float speed;
extern volatile int overflow;
float prev_ticks=0;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance==TIM1){
		float ticks= TIM1->CCR1+overflow*65536-prev_ticks;
		overflow=0;
		prev_ticks=TIM1->CCR1;
		speed=8000000/ticks*2.4;
	}
}
float SPEED_GetSpeed(){
	return speed;
}
void TEMP_Init()
{

	TEMP_MailboxHandle = xQueueCreate(1, sizeof(float));
	AZIM_MailboxHandle = xQueueCreate(1, sizeof(float));
	xTaskCreate(TEMP_Task, "TEMP_Task", 64, NULL, 2, &TEMP_TaskHandle);
}
