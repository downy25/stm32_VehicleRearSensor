/*
 * app.c
 *
 *  Created on: Nov 25, 2024
 *      Author: wjdek
 */
#include "app.h"

extern UART_HandleTypeDef huart2;

int cycleDistance =0;
int cycleSound = 0;

// 1ms의 주기로 호출됨
void SystickCallback(){
	if(cycleDistance > 0) cycleDistance--;
	if(cycleSound > 0) cycleSound--;
}



void app(){
	uint8_t distance;
	uint8_t periodSound = 0;
	int periodOn = 0;
	//uart init
	initUart(&huart2);
	//buzzer init
	initSound();
	//timer 시작
	initDistance();
	while(1){
		// measure distnace
		if(cycleDistance == 0){
			//100ms마다 들어온다
			cycleDistance = 100;
			distance = getDistance();
			printf("distance = %d\n",distance);
			// judgement distance
			if(distance > 20) periodOn = 1000 / 2;
			else if((distance <= 20) && (distance > 15)) periodOn = 800 / 2;
			else if((distance <= 15) && (distance > 10)) periodOn = 600 / 2;
			else if((distance <= 10) && (distance > 5)) periodOn = 400 / 2;
			else if((distance <= 5))periodOn = 0;

		}
		//generate sound
		if(cycleSound == 0){
			cycleSound = periodOn;
			if(periodOn == 0){
				startSound();
				cycleSound = 0;
			}
			else{
				if(periodSound == 1) startSound();
				else if(periodSound == 0) stopSound();
				periodSound ^= 1;
			}
		}
	}
}

//내가 작성한 코드

/*void app(){
	uint16_t delay=0;
	static uint16_t count=0;
	//uart 장치 초기화
	initUart(&huart2);
	//pwm시작
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	//timer 시작
	HAL_TIM_IC_Start(&htim3, TIM_CHANNEL_2);
	HAL_TIM_Base_Start(&htim11); //base는 기본기능만 사용
	while(1){
		setSound(500); //500hz음 발생
		printf("distance = %d\n",getDistance());
		if(getDistance()>=20)delay=1000;
		else if(getDistance()>=15&&getDistance()<20)delay=800;
		else if(getDistance()>=10&&getDistance()<15)delay=600;
		else if(getDistance()>=5&&getDistance()<10)delay=400;
		else delay=0;
		HAL_Delay(delay);
		stopSound();
		HAL_Delay(delay);

	}
}*/

//HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET); //GPIO_PIN_RESET == 0
//HAL_Delay(1000);
//HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);  //GPIO_PIN_SET == 1
//HAL_Delay(1000);
