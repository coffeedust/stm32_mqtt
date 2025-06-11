/*
 * motor.h
 *
 *  Created on: Jun 11, 2025
 *      Author: user
 */

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

#include "main.h"

// 특정 상수값을 원하는 문자로 치환
typedef enum {
	dir_none = 0,
	dir_cw,
	dir_ccw
} direction_t;

// 함수 프로토타입 선언 (변수 이름 생략 가능)
void initMotor(TIM_HandleTypeDef *, uint32_t);
void dirMotor(direction_t);
void speedMotor(uint8_t);
void stopMotor();

#endif /* INC_MOTOR_H_ */
