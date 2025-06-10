/*
 * uart.h
 *
 *  Created on: May 12, 2025
 *      Author: user
 */

#ifndef INC_UART_H_
#define INC_UART_H_

#include "main.h"

// definition prototype function
void initUart(UART_HandleTypeDef *huart);
char getUart();
void initUart2(UART_HandleTypeDef *huart);
char getUart2();
void clearBuffer();
char *getMessage();
void printf2(const char *format, ...);

#endif /* INC_UART_H_ */
