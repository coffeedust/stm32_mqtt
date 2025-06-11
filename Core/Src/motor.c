/*
 * motor.c
 *
 *  Created on: Jun 11, 2025
 *      Author: user
 */

#include "motor.h"

uint8_t stepLUT[] = {
		0b1010,
		0b0110,
		0b0101,
		0b1001
};

void outStep(uint8_t step) {
	HAL_GPIO_WritePin(MA1_GPIO_Port, MA1_Pin, stepLUT[step] >> 0 & 1);
	HAL_GPIO_WritePin(MA2_GPIO_Port, MA2_Pin, stepLUT[step] >> 1 & 1);
	HAL_GPIO_WritePin(MB1_GPIO_Port, MB1_Pin, stepLUT[step] >> 2 & 1);
	HAL_GPIO_WritePin(MB2_GPIO_Port, MB2_Pin, stepLUT[step] >> 3 & 1);
}

// 모터의 동작을 제어. 호출 주기에 따라서 속도가 변화
void runStep(direction_t dir) {
	static uint8_t cur_step;	// 0 ~ 255;
	switch(dir) {
		case dir_none:
			break;
		case dir_cw:
			cur_step++;
			cur_step %= 4;
			break;
		case dir_ccw:
			cur_step--;
			if(cur_step == 255) cur_step = 3;
			break;
	}
	outStep(cur_step);
}
