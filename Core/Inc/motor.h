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
void runStep(direction_t dir);

#endif /* INC_MOTOR_H_ */
