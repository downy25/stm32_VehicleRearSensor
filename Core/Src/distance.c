/*
 * distance.c
 *
 *  Created on: Dec 3, 2024
 *      Author: wjdek
 */
#include "distance.h"

//장치선언
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim11;

// 전역변수
uint32_t ic_val1 = 0; //rising 위치 저장
uint32_t ic_val2 = 0; //falling 위치 저장
uint32_t difference = 0; // falling - rising
uint8_t is_first_captured = 0; //현재 캡쳐 단계
uint8_t distance = 0; //계산된 거리(cm)

void initDistance(){
	HAL_TIM_IC_Start(&htim3, TIM_CHANNEL_2);
	HAL_TIM_Base_Start(&htim11); //base는 기본기능만 사용
}

// us delay
void delayUs(uint16_t t){
	htim11.Instance->CNT = 0;        // 카운터 초기화
	while(htim11.Instance->CNT < t); // up카운터의 값이 t와 같으면 종료
}

// 인터럽트 서비스 루틴(isr)
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){
	//인터럽트가 발생된 타이머의 채널 확인
	if(htim->Channel  == HAL_TIM_ACTIVE_CHANNEL_2){
		//현재 캡쳐 단계
		if(is_first_captured == 0){
			ic_val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
			//다음 인터럽트 발생을 위한 설정
			is_first_captured = 1;
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_FALLING);
		}
		// 현재 캡처 하강 단계
		else if(is_first_captured == 1){
			// 현재 카운터의 값을 저장
			ic_val2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);

			// 거리 계산
			if(ic_val2 > ic_val1){
				difference = ic_val2 - ic_val1;
			}
			else{
				difference = 65535 - ic_val1 + ic_val2;
			}
			distance = difference * 0.034 / 2;

			//다음 인터럽트 발생을 위한 설정
			is_first_captured = 0;
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_2,TIM_INPUTCHANNELPOLARITY_RISING);
			htim->Instance->CNT = 0;
			// 인터럽트 중지
			__HAL_TIM_DISABLE_IT(&htim3,TIM_IT_CC2);
		}
	}
}


// 거리를 측정하여 반환
uint32_t getDistance(){
	//트리거 핀 동작
	HAL_GPIO_WritePin(Trigger_GPIO_Port,Trigger_Pin, GPIO_PIN_SET); //1<<6
	delayUs(10); //delay를 10us만큼 주는 함수 생성
	HAL_GPIO_WritePin(Trigger_GPIO_Port,Trigger_Pin, GPIO_PIN_RESET);
	// 인터럽트 설정
	__HAL_TIM_ENABLE_IT(&htim3,TIM_IT_CC2);
	return distance;
}

