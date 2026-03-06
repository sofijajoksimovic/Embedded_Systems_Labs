/*
 * homework.c
 *
 *  Created on: Jan 6, 2022
 *      Author: Marko Micovic
 */

#include "homework.h"

#include "FreeRTOS.h"
#include "task.h"

#include <stdlib.h>
#include <string.h>

#include "driver_lcd.h"
#include "driver_uart.h"
#include "driver_motor.h"
#include "driver_temp.h"
#include "tim.h"
#include "timers.h"

#define TEMP_OFF 0x90
#define RAIN_OFF 0xD0
#define AZIM_OFF 0x80
#define SPEED_OFF 0xC0

volatile float rain=0;

static void homeworkTask(void *parameters)
{

	int prev_len=0;

	LCD_CommandEnqueue(LCD_INSTRUCTION, LCD_SET_DD_RAM_ADDRESS_INSTRUCTION | TEMP_OFF);
	LCD_CommandEnqueue(LCD_DATA, 0x02);
	LCD_CommandEnqueue(LCD_INSTRUCTION, LCD_SET_DD_RAM_ADDRESS_INSTRUCTION | TEMP_OFF | 0x01);
	char tempMessage[4]="Tem:";

	for(int i=0;i<4;i++){
		LCD_CommandEnqueue(LCD_DATA, tempMessage[i]);
	}

	LCD_CommandEnqueue(LCD_INSTRUCTION, LCD_SET_DD_RAM_ADDRESS_INSTRUCTION | RAIN_OFF);
	LCD_CommandEnqueue(LCD_DATA, 0x02);
	LCD_CommandEnqueue(LCD_INSTRUCTION, LCD_SET_DD_RAM_ADDRESS_INSTRUCTION | RAIN_OFF | 0x01);
	char rainMessage[4]="Kis:";

		for(int i=0;i<4;i++){
			LCD_CommandEnqueue(LCD_DATA, rainMessage[i]);
		}

	LCD_CommandEnqueue(LCD_INSTRUCTION, LCD_SET_DD_RAM_ADDRESS_INSTRUCTION | AZIM_OFF);
	LCD_CommandEnqueue(LCD_DATA, 0x02);
	LCD_CommandEnqueue(LCD_INSTRUCTION, LCD_SET_DD_RAM_ADDRESS_INSTRUCTION | AZIM_OFF | 0x01);
	char azimMessage[4]="Azi:";

		for(int i=0;i<4;i++){
			LCD_CommandEnqueue(LCD_DATA, azimMessage[i]);
		}

	LCD_CommandEnqueue(LCD_INSTRUCTION, LCD_SET_DD_RAM_ADDRESS_INSTRUCTION | SPEED_OFF);
	LCD_CommandEnqueue(LCD_DATA, 0x02);
	LCD_CommandEnqueue(LCD_INSTRUCTION, LCD_SET_DD_RAM_ADDRESS_INSTRUCTION | SPEED_OFF | 0x01);
	char speedMessage[4]="Brz:";

		for(int i=0;i<4;i++){
			LCD_CommandEnqueue(LCD_DATA, speedMessage[i]);
		}

	int prev_tmp=0;
	int prev_rain=0;
	int prev_azim=0;
	int prev_speed=0;

	while(1){
		int temp=TEMP_GetCurrentValue();
		int azim=AZIM_GetCurrentValue();
		int speed=SPEED_GetSpeed();

		char tempArr[6];
		char rainArr[6];
		char azimArr[6];
		char speedArr[6];


		if(temp!=prev_tmp){

			LCD_CommandEnqueue(LCD_INSTRUCTION, LCD_SET_DD_RAM_ADDRESS_INSTRUCTION | TEMP_OFF);
			if(temp>prev_tmp){
				LCD_CommandEnqueue(LCD_DATA, 0x02);
			}else LCD_CommandEnqueue(LCD_DATA, 0x01);

			LCD_CommandEnqueue(LCD_INSTRUCTION, LCD_SET_DD_RAM_ADDRESS_INSTRUCTION | TEMP_OFF | 0x05);

			itoa(temp, tempArr, 10);

			for(int i=0;i<strlen(tempArr);i++){
				LCD_CommandEnqueue(LCD_DATA, tempArr[i]);
			}

			prev_tmp=temp;
		}

		if(rain!=prev_rain || rain==0){
			LCD_CommandEnqueue(LCD_INSTRUCTION, LCD_SET_DD_RAM_ADDRESS_INSTRUCTION | RAIN_OFF);
			if(rain>prev_rain){
				LCD_CommandEnqueue(LCD_DATA, 0x02);
			}else LCD_CommandEnqueue(LCD_DATA, 0x01);


			LCD_CommandEnqueue(LCD_INSTRUCTION, LCD_SET_DD_RAM_ADDRESS_INSTRUCTION | RAIN_OFF | 0x05);

			itoa(rain, rainArr, 10);

			for(int i=0;i<strlen(rainArr);i++){
				LCD_CommandEnqueue(LCD_DATA, rainArr[i]);
			}
		}

		if(azim!=prev_azim){
			LCD_CommandEnqueue(LCD_INSTRUCTION, LCD_SET_DD_RAM_ADDRESS_INSTRUCTION | AZIM_OFF);
			if(azim>prev_azim){
				LCD_CommandEnqueue(LCD_DATA, 0x02);
			}else LCD_CommandEnqueue(LCD_DATA, 0x01);


			LCD_CommandEnqueue(LCD_INSTRUCTION, LCD_SET_DD_RAM_ADDRESS_INSTRUCTION | AZIM_OFF | 0x05);

			itoa(azim, azimArr, 10);

			for(int i=0;i<strlen(azimArr);i++){
				LCD_CommandEnqueue(LCD_DATA, azimArr[i]);
			}
		}

		if(speed!=prev_speed){
			LCD_CommandEnqueue(LCD_INSTRUCTION, LCD_SET_DD_RAM_ADDRESS_INSTRUCTION | SPEED_OFF);
			if(speed>prev_speed){
				LCD_CommandEnqueue(LCD_DATA, 0x02);
			}else LCD_CommandEnqueue(LCD_DATA, 0x01);


			LCD_CommandEnqueue(LCD_INSTRUCTION, LCD_SET_DD_RAM_ADDRESS_INSTRUCTION | SPEED_OFF | 0x05);

			itoa(speed, speedArr, 10);

			for(int i=0;i<strlen(speedArr);i++){
				LCD_CommandEnqueue(LCD_DATA, speedArr[i]);
			}

		}

		if(temp!=prev_tmp || azim!=prev_azim || speed!=prev_speed){

			for(int i=0;i<prev_len;i++){
				UART_AsyncTransmitCharacter('\b');
			}

			UART_AsyncTransmitCharacter('/');
			UART_AsyncTransmitCharacter('A');
			UART_AsyncTransmitCharacter('=');
			UART_AsyncTransmitString(azimArr);
			UART_AsyncTransmitCharacter('/');
			UART_AsyncTransmitCharacter('B');
			UART_AsyncTransmitCharacter('=');
			UART_AsyncTransmitString(speedArr);
			UART_AsyncTransmitCharacter('/');
			UART_AsyncTransmitCharacter('T');
			UART_AsyncTransmitCharacter('=');
			UART_AsyncTransmitString(tempArr);
			UART_AsyncTransmitCharacter('/');
			UART_AsyncTransmitCharacter('K');
			UART_AsyncTransmitCharacter('=');
			UART_AsyncTransmitString(rainArr);

			prev_len=12+strlen(azimArr)+strlen(speedArr)+strlen(tempArr)+strlen(rainArr);

			prev_speed=speed;
			prev_azim=azim;
			prev_rain=rain;
		}

		if(temp<30){
			TIM1->CCR4=0;
		}
		else if(temp<35){
			TIM1->CCR4=(65535+1)/2;
		}
		else TIM1->CCR4=65535;

		vTaskDelay(pdMS_TO_TICKS(200));
	}
}


int period_cnt=0;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	//kada kisa dostigne nivo
	if(GPIO_Pin==GPIO_PIN_14){
		rain=36000/period_cnt;
		period_cnt=0;
	}
}

void rainFunc(){
	period_cnt++;
}

void homeworkInit()
{


	TEMP_Init();
	LCD_Init();
	UART_Init();

	//HAL_TIM_Base_Start(&htim1);
	HAL_TIM_Base_Start_IT(&htim1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);

	HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_1);

	TimerHandle_t rainTimer=xTimerCreate("rainTimer", pdMS_TO_TICKS(1), pdTRUE, NULL, rainFunc);
	xTaskCreate(homeworkTask, "homeworkTask", 128, NULL, 5, NULL);

	xTimerStart(rainTimer, portMAX_DELAY);
}

