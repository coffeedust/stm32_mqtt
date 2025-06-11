/*
 * app.h
 *
 *  Created on: May 27, 2025
 *      Author: user
 */

#ifndef INC_APP_H_
#define INC_APP_H_

#include "main.h"

extern UART_HandleTypeDef	huart1;
extern UART_HandleTypeDef	huart2;
extern ADC_HandleTypeDef	hadc1;
extern TIM_HandleTypeDef	htim1;

void app();

#endif /* INC_APP_H_ */
