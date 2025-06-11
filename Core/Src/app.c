/*
 * app.c
 *
 *  Created on: May 27, 2025
 *      Author: user
 */

#include "app.h"
#include "jsmn.h"

direction_t dir;
uint8_t			speed;

// 데이터 수신 함수
char* receiveESP() {
	static char lineBuffer[2048] = {0,};
	int pos = 0;
	signed char ch;
	// 제한시간내에 첫 문자가 도착하지 않으면 타임아웃 반환
	int timeoutReceiveWifi = HAL_GetTick() + 10000;
	do {
		ch = getUart();
		if(timeoutReceiveWifi < HAL_GetTick()) {
			return lineBuffer;
		}
	} while(ch == 0);
	timeoutReceiveWifi = HAL_GetTick() + 3000; // 타임아웃 초기화
	while(ch != '\r') { // '\r'까지 읽기
		if(ch == '\n') {
			pos = 0; // 새로운 줄의 시작
		}
		else if(ch != 0) { // 유효한 문자만 버퍼에 저장
			if(pos >= sizeof(lineBuffer) - 1) { // 버퍼 오버플로우 방지
				clearBuffer(); // Rx 버퍼 정리
				memset(lineBuffer, 0, 2048);
				return lineBuffer; // 에러 반환
			}
			lineBuffer[pos++] = ch;
		}
		if(timeoutReceiveWifi < HAL_GetTick()) { // 타임아웃 발생 시
			clearBuffer(); // Rx 버퍼 정리
			memset(lineBuffer, 0, 2048);
			return lineBuffer; // 타임아웃 반환
		}
		ch = getUart(); // 다음 문자 읽기
	}
	lineBuffer[pos] = '\0'; // 문자열 종료
	return lineBuffer; // 읽은 데이터 반환
}

// 헤더 찾기
char *receivePacketWithHeader(char *header) {
	char *rxData;
	char *result;
	rxData = receiveESP();
	if(strncmp(rxData, header, strlen(header)) == 0) {
		result = strchr(rxData, ':');
		result++;
	}
	else
		return 0;
	return result;
}

// 수신 대기 함수
bool waitForResponse(char *cmpStr) {
	char *rxData;
	int timeout;
	timeout = HAL_GetTick() + 3000;	// system timer(systick)의 값을 읽음(1ms마다 1씩 증가)
	rxData = receiveESP();
	// 일치하지 않는 위치를 리턴, 일치시 0리턴 strncmp(첫번째 문자열, 두번째 문자열, 비교 개수)
	while(strncmp(rxData, cmpStr, strlen(cmpStr)) != 0) {
		rxData = receiveESP();
		if(HAL_GetTick() > timeout) {
			return false;
		}
		HAL_Delay(1);
	}
	return true;
}

// esp를 리셋하는 함수
void resetESP() {
	printf("AT\r\n");
	HAL_Delay(50);
	clearBuffer();
	printf("at+rst\r\n");
	HAL_Delay(3000);
	int status = waitForResponse("ready");
	if(status)
		printf2("-1 receive : ready\r\n");
	else
		printf2("-1 receive : ERROR\r\n");
	clearBuffer();
	printf("ate0\r\n");		// echo(에코)를 꺼야 데이터를 보낼때마다 반향되는 문자가 없다.
	status = waitForResponse("OK");
	if(status)
		printf2("0 receive : OK\r\n");
	else
		printf2("0 receive : ERROR\r\n");
	clearBuffer();
}

// ap에 접속하는 함수
void connectAP() {
	// 초기 테스트
	printf("AT\r\n");
	int status = waitForResponse("OK");
	if(status)
		printf2("1 receive : OK\r\n");
	else
		printf2("1 receive : ERROR\r\n");
	clearBuffer();
	// WIFI 설정
	printf("AT+CWINIT=1\r\n");
	status = waitForResponse("OK");
	if(status)
		printf2("2 receive : OK\r\n");
	else
		printf2("2 receive : ERROR\r\n");
	clearBuffer();
	// station mode
	printf("AT+CWMODE=1\r\n");
	status = waitForResponse("OK");
	if(status)
		printf2("3 receive : OK\r\n");
	else
		printf2("3 receive : ERROR\r\n");
	clearBuffer();
	// connection ap
	printf("AT+CWJAP=\"iot_AI\",\"iotiotiot\"\r\n");
	status = waitForResponse("OK");
	if(status)
		printf2("4 receive : OK\r\n");
	else
		printf2("4 receive : ERROR\r\n");
	HAL_Delay(50);
	clearBuffer();
}

// NTP server에 접속하는 함수
void connectNTPServer() {
	printf("AT\r\n");
	HAL_Delay(50);
	clearBuffer();
	// NTP 서버에 대한 설정
	printf("AT+CIPSNTPCFG=1,900,\"time.google.com\"\r\n");
	HAL_Delay(50);
	int status = waitForResponse("OK");
	if(status)
		printf2("5 receive : OK\r\n");
	else
		printf2("5 receive : ERROR\r\n");
	HAL_Delay(50);//
	status = waitForResponse("+TIME_UPDATED");
	if(status)
		printf2("6 receive : TIME_UPDATED\r\n");
	else
		printf2("6 receive : ERROR\r\n");
	printf("AT\r\n");
	HAL_Delay(50);
	clearBuffer();
	// 서버에서 시간 가져오기
	printf("AT+CIPSNTPTIME?\r\n");
	HAL_Delay(50);//
	printf2("%s\r\n", receivePacketWithHeader("+CIPSNTPTIME"));
}

void connectServer() {
	// 서버 접속
	printf("AT\r\n");
	HAL_Delay(50);
	clearBuffer();
	printf("AT+CIPMUX=1\r\n");	// 다중 접속 모드 활성, 이후 소켓 번호로 할당됨
	HAL_Delay(50);
	clearBuffer();
	printf("AT+CIPSTART=0,\"TCP\",\"192.168.0.51\",5555\r\n");	// 소켓 0번 접속
	int status = waitForResponse("OK");
	if(status)
		printf2("7 receive : OK\r\n");
	else
		printf2("7 receive : ERROR\r\n");
	printf("AT+CIPSTART=1,\"TCP\",\"192.168.0.51\",7777\r\n");	// 소켓 1번 접속
	HAL_Delay(50);
	clearBuffer();
}

void closeServer() {
	// 서버 종료
	printf("AT+CIPCLOSE\r\n");
	int status = waitForResponse("OK");
	if(status)
		printf2("8 receive : OK\r\n");
	else
		printf2("8 receive : ERROR\r\n");
}

void startServer() {
	printf("AT\r\n");
	HAL_Delay(50);
	clearBuffer();
	printf("AT+CIPMUX=1\r\n");	// 다중 접속 모드 활성, 이후 소켓 번호로 할당됨
	HAL_Delay(50);
	clearBuffer();
	printf("AT+CIPSERVER=1,5000\r\n");
	HAL_Delay(50);
	clearBuffer();
	// get ip address
	printf("AT+CIPSTA?\r\n");
	HAL_Delay(50);
	printf2("%s\r\n", receivePacketWithHeader("+CIPSTA:ip"));
	HAL_Delay(50);
	clearBuffer();
}

void sendServer(uint8_t sNo, char *msg) {
	clearBuffer();
	int sendDataLength = strlen(msg);
	printf("AT+CIPSEND=%d,%d\r\n", sNo, sendDataLength);
	int status = waitForResponse("OK");
	if(status) {
		while(getUart() != '>');	// 입력 프롬프트 대기
		printf("%s", msg);
	}
	status = waitForResponse("SEND OK");
	if(status)
		printf2("%d : receive : SEND OK\r\n", sNo);
	else
		printf2("%d : receive : ERROR\r\n", sNo);
}

void receiveServer() {
	char rxData = receiveESP();
	printf2(receivePacketWithHeader("+IPD"));
}

// MQTT 서버 접속
void connectMQTT() {
	printf("AT\r\n");
	HAL_Delay(10);
	clearBuffer();
	printf("AT+CIPMUX=1\r\n");	// 다중 접속 모드 활성, 이후 소켓 번호로 할당됨
	HAL_Delay(50);
	clearBuffer();
	// MQTT Server 설정
	printf("AT+MQTTUSERCFG=0,1,\"MY_TOPIC\",\"\",\"\",0,0,\"\"\r\n");
	HAL_Delay(10);
	bool status = waitForResponse("OK");
	if(status)
		printf2("success mqtt config\r\n");
	else
		printf2("failed mqtt config\r\n");
	// MQTT 접속
	printf("AT+MQTTCONN=0,\"192.168.0.51\",1883,1\r\n");
	HAL_Delay(10);
	status = waitForResponse("+MQTTCONNECTED");
	if(status)
		printf2("success mqtt connect\r\n");
	else
		printf2("failed matt connect\r\n");
	clearBuffer();
	// Subscribe 등록 at+mqttsub=0,"topic",0
	printf("AT+MQTTSUB=0,\"MY_TOPIC\",0\r\n");
	HAL_Delay(10);
	status = waitForResponse("OK");
	if(status) printf2("success subscribe\r\n");
	else printf2("failed subscribe\r\n");
}

// MQTT Subscribe
// +MQTTSUB:0,"MY_TOPIC",5,hello
char *receiveMQTT() {
	char *receiveMsg;
	receiveMsg = receivePacketWithHeader("+MQTTSUB");
	if(strncmp(&receiveMsg[3], "MY_TOPIC", 8) != 0) return 0;
	return &receiveMsg[2];
}

int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
  if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
      strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
    return 0;
  }
  return -1;
}

void json_parse(char *inStr) {
	jsmn_parser p;
	jsmntok_t		t[128];
	jsmn_init(&p);
	int result = jsmn_parse(&p, inStr, strlen(inStr), t, 128);
	// {"KEY":"VAR"}
	if(result < 1) {
		printf2("json format error\r\n");
		return;
	}
	for(int i = 1; i < result; i++) {
		if(jsoneq(inStr, &t[i], "HEADER") == 0) {
			// %.s 문자열의 길이를 지정하여 출력
			printf2("HEADER = %.*s\r\n", t[i+1].end - t[i+1].start, inStr + t[i+1].start);
			i++;
		}
		else if(jsoneq(inStr, &t[i], "CONTENT") == 0) {
			// %.s 문자열의 길이를 지정하여 출력
			printf2("CONTENT = %.*s\r\n", t[i+1].end - t[i+1].start, inStr + t[i+1].start);
			i++;
		}
		else if(jsoneq(inStr, &t[i], "ITEM1") == 0) {
			// %.s 문자열의 길이를 지정하여 출력
			printf2("ITEM1 = %.*s\r\n", t[i+1].end - t[i+1].start, inStr + t[i+1].start);
			i++;
		}
		else if(jsoneq(inStr, &t[i], "ITEM2") == 0) {
			// %.s 문자열의 길이를 지정하여 출력
			printf2("ITEM2 = %.*s\r\n", t[i+1].end - t[i+1].start, inStr + t[i+1].start);
			i++;
		}
		else if(jsoneq(inStr, &t[i], "IS_RUN") == 0) {
			// %.s 문자열의 길이를 지정하여 출력
			printf2("IS_RUN = %.*s\r\n", t[i+1].end - t[i+1].start, inStr + t[i+1].start);
			if(strncmp(inStr + t[i+1].start, "NONE", 4) == 0) {
				dir = dir_none;
			}
			else if(strncmp(inStr + t[i+1].start, "CW", 2) == 0) {
				dir = dir_cw;
			}
			else if(strncmp(inStr + t[i+1].start, "CCW", 3) == 0) {
				dir = dir_ccw;
			}
			i++;
		}
		else if(jsoneq(inStr, &t[i], "SPEED") == 0) {
			// %.s 문자열의 길이를 지정하여 출력
			printf2("SPEED = %.*s\r\n", t[i+1].end - t[i+1].start, inStr + t[i+1].start);
			char tmp[5];
			strncpy(tmp, inStr + t[i+1].start, t[i+1].end - t[i+1].start);
			speed = atoi(tmp);
			i++;
		}
	}
}

// 1ms 주기로 호출됨
void SystickCallback() {
	static uint8_t cur_speed;
	if(cur_speed > 0) cur_speed--;
	else {
		cur_speed = speed;
		runStep(dir);
	}
}

void app() {
	// uart 초기화
	initUart(&huart1);	// to ESP32
	initUart2(&huart2);	// to PC by USB
	// ESP 초기화
	resetESP();
	// AP 접속
	connectAP();
	// MQTT 접속
	connectMQTT();
	// 모터 초기화
	speed = 255;
	dir = dir_none;
	while(1) {
		static uint32_t cycle_sub = 0, cycle_pub = 0;
		static uint8_t seq_num = 0;
		// publish
		if(HAL_GetTick() > cycle_pub) {
			cycle_pub = HAL_GetTick() + 1000;
			// ad conversion
			HAL_ADC_Start(&hadc1);
			// wait for complete
			HAL_ADC_PollForConversion(&hadc1, 10);
			// get adc value
			uint16_t adcValue = HAL_ADC_GetValue(&hadc1);
			// stop adc
			HAL_ADC_Stop(&hadc1);
			// publish data
			char sendMsg[500];
			sprintf(sendMsg, "{\"TYPE\":\"TEMP\","
					"\"SEQ\":%d,"
					"\"STATUS\":[{"
					"\"TEMP1\":%2.1f,"
					"\"TEMP2\":-1}]}", seq_num++, lookupTemperature(adcValue));
			printf("AT+MQTTPUBRAW=0,\"MY_TOPIC\",%d,0,0\r\n", strlen(sendMsg));
			bool status = waitForResponse("OK");
			if(status) {
				while(getUart() != '>');	// 문자열 입력 프롬프트 대기
				printf(sendMsg);
				printf2("published\r\n");
			}
		}
		// subscribe
		if(HAL_GetTick() > cycle_sub) {
			cycle_sub = HAL_GetTick() + 10;
			char *msg = receiveMQTT();
			if(msg[0] != 0) {
				printf2("%s\r\n", &msg[14]);
				// json parse
				json_parse(&msg[14]);
			}
		}
	}
}
