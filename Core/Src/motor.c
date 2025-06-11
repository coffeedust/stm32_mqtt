/*
 * motor.c
 *
 *  Created on: Jun 11, 2025
 *      Author: user
 */

#include "motor.h"

TIM_HandleTypeDef	*motor_handle;
uint32_t					motor_channel;

// 모터 초기화
void initMotor(TIM_HandleTypeDef	*handle, uint32_t ch) {
	motor_handle = handle;
	motor_channel = ch;
	HAL_TIM_PWM_Start(motor_handle, ch);
}

// 모터 동작 (방향, 속도)
void dirMotor(direction_t direction) {
	// 모터의 회전 방향 설정
	switch(direction) {
		case dir_none:
			HAL_GPIO_WritePin(MOTOR1_GPIO_Port, MOTOR1_Pin, 0);
			HAL_GPIO_WritePin(MOTOR2_GPIO_Port, MOTOR2_Pin, 0);
			break;
		case dir_cw:
			HAL_GPIO_WritePin(MOTOR1_GPIO_Port, MOTOR1_Pin, 1);
			HAL_GPIO_WritePin(MOTOR2_GPIO_Port, MOTOR2_Pin, 0);
			break;
		case dir_ccw:
			HAL_GPIO_WritePin(MOTOR1_GPIO_Port, MOTOR1_Pin, 0);
			HAL_GPIO_WritePin(MOTOR2_GPIO_Port, MOTOR2_Pin, 1);
			break;
	}
}

void speedMotor(uint8_t speed_duty) {
	// 모터의 속도 설정 (CCR / ARR * 100 = DUTY, CCR = ARR * DUTY / 100 )
	switch(motor_channel) {
		case TIM_CHANNEL_1:
			motor_handle->Instance->CCR1 = motor_handle->Instance->ARR * speed_duty / 100;
			break;
		case TIM_CHANNEL_2:
			motor_handle->Instance->CCR2 = motor_handle->Instance->ARR * speed_duty / 100;
			break;
		case TIM_CHANNEL_3:
			motor_handle->Instance->CCR3 = motor_handle->Instance->ARR * speed_duty / 100;
			break;
		case TIM_CHANNEL_4:
			motor_handle->Instance->CCR4 = motor_handle->Instance->ARR * speed_duty / 100;
			break;
	}
	// ARR, CRR의 값의 변동이 있으면 CNT는 항상 0으로 초기화 함
	motor_handle->Instance->CNT = 0;
}

// 모터 정지
void stopMotor() {
	switch(motor_channel) {
		case TIM_CHANNEL_1:
			motor_handle->Instance->CCR1 = 0;
			break;
		case TIM_CHANNEL_2:
			motor_handle->Instance->CCR2 = 0;
			break;
		case TIM_CHANNEL_3:
			motor_handle->Instance->CCR3 = 0;
			break;
		case TIM_CHANNEL_4:
			motor_handle->Instance->CCR4 = 0;
			break;
	}
}
