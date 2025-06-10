/*
 * uart.c
 *
 *  Created on: May 12, 2025
 *      Author: user
 */

#include "uart.h"
#include <stdio.h>
#include <stdarg.h>	// valist, vastart...

UART_HandleTypeDef *uart;
UART_HandleTypeDef *uart2;

#define 	rxBufferMax 100					// 버퍼의 크기
uint8_t 	rxBuffer[rxBufferMax];	// 버퍼
uint16_t	rxBufferWriteIndex;			// 쓰기 인덱스
uint16_t	rxBufferReadIndex;			// 읽기 인덱스
uint8_t		rxChar;									// 수신된 1바이트

#define 	rxBufferMax2 100					// 버퍼의 크기
uint8_t 	rxBuffer2[rxBufferMax2];	// 버퍼
uint16_t	rxBufferWriteIndex2;			// 쓰기 인덱스
uint16_t	rxBufferReadIndex2;			// 읽기 인덱스
uint8_t		rxChar2;									// 수신된 1바이트

// printf 장치 재설정
int _write(int file, char *p, int len) {
	HAL_UART_Transmit(uart, (uint8_t *)p, len, len);
	return len;
}

// uart 수신 인터럽트(문자 수신시 이곳으로 이동됨)
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	// uart1에서 수신된 인터럽트 처리
	if(huart->Instance == uart->Instance) {
		rxBuffer[rxBufferWriteIndex++] = rxChar;	// 버퍼에 1바이트 넣고 쓰기 인덱스 1증가
		rxBufferWriteIndex %= rxBufferMax;			// 쓰기 인덱스가 마지막에서 처음으로 이동
		HAL_UART_Receive_IT(uart, &rxChar, 1);	// 수신 인터럽트 활성화(재장전)
	}
	// uart2에서 수신된 인터럽트 처리
	else if(huart->Instance == uart2->Instance) {
		rxBuffer2[rxBufferWriteIndex2++] = rxChar2;	// 버퍼에 1바이트 넣고 쓰기 인덱스 1증가
		rxBufferWriteIndex2 %= rxBufferMax2;			// 쓰기 인덱스가 마지막에서 처음으로 이동
		HAL_UART_Receive_IT(uart2, &rxChar2, 1);	// 수신 인터럽트 활성화(재장전)
	}
}

// function implementation
// 함수 초기화
void initUart(UART_HandleTypeDef *huart) {
	uart = huart;	// uart 장치 핸들 저장
	HAL_UART_Receive_IT(uart, &rxChar, 1);	// 수신 인터럽트 활성화
}

void initUart2(UART_HandleTypeDef *huart) {
	uart2 = huart;	// uart 장치 핸들 저장
	HAL_UART_Receive_IT(uart2, &rxChar2, 1);	// 수신 인터럽트 활성화
}

// 수신 버퍼 초기화
void clearBuffer() {
	rxBufferWriteIndex = rxBufferReadIndex = 0;
}

char getUart() {
	char result;
	if(rxBufferWriteIndex == rxBufferReadIndex) return 0;	// 버퍼가 비어있음
	result = rxBuffer[rxBufferReadIndex++];	// 버퍼에서 1바이트 꺼내고 읽기 인덱스 1증가
	rxBufferReadIndex %= rxBufferMax;	// 읽기 인덱스가 마지막에서 처음으로 이동
	return result;
}

char getUart2() {
	char result;
	if(rxBufferWriteIndex2 == rxBufferReadIndex2) return 0;	// 버퍼가 비어있음
	result = rxBuffer2[rxBufferReadIndex2++];	// 버퍼에서 1바이트 꺼내고 읽기 인덱스 1증가
	rxBufferReadIndex2 %= rxBufferMax2;	// 읽기 인덱스가 마지막에서 처음으로 이동
	return result;
}

char *getMessage() {
	static char msg[20];
	static uint8_t	idx = 0;
	char ch;
	ch = getUart();
	if(ch != 0) {
		if(ch == '\n' || ch == '\r') {	// 줄바꿈 기호를 수신
			memset(msg, 0, 20);
			idx = 0;
		}
		else {
			msg[idx++] = ch;
		}
	}
	return msg;
}

void printf2(const char *format, ...) {
	char buffer[100];
	va_list args;
	va_start(args, format);
	// snprintf(배열, 배열크기, ...)
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);
	HAL_UART_Transmit(uart2, buffer, strlen(buffer), strlen(buffer));
}
