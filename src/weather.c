#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "tm4c123gh6pm.h"
#include "pll.h"
#include "UART.h"
#include "ST7735.h"
#include "esp8266.h"
#include "LED.h"

char Fetch[] = "GET /data/2.5/weather?q=Lexington&APPID=6d12d5c38a00efa50b93d7ff94aab9ea HTTP/1.1\r\nHost:api.openweathermap.org\r\n\r\n";

char TempC[7];
char TMaxC[7];
char TMinC[7];
char WDescription[128];
char WLocation[128];
char WCountry[5];

int GetTempStr(char *in, char *out){
	int vali, vald;
	sscanf(in, "%*[^:]:%i.%i", &vali, &vald); 
	return sprintf(out, "%d.%d", vali-273, vald-15);
}
int GetStr(char *in, char *out){
	return sscanf(in, "%*[^:]:\"%[^\"]", out);
}

void GetWeather(void){
	ST7735_FillScreen(ST7735_BLUE);
	UART_OutString("\n\rBegin Connecting...\n\r");
	printf("Connecting to weather...\n");
	LED_RedOn();
	
	//establish TCP Connection with openweather
	if(ESP8266_MakeTCPConnection("api.openweathermap.org") == 0)
		UART_OutString("MakeTCPConnection failed");

	//2=setup but no TCP, 3=TCP connection 
  ESP8266_GetStatus();
		
	//if connection is valid:
  if(("api.openweathermap.org")){ // open socket in server
		UART_OutString("\rSuccessful Connection. Begin Fetch...\n\r");
		printf("Successful Connection. Fetching Weather...\n");
		LED_RedOff();
		LED_BlueOn();
		ESP8266_SendTCP(Fetch); // fills RXBuffer - must parse before next SendCommand()
		UART_OutString("\rCompleted Fetch. Print to Screen.\n\r");		
		
		//Fill Global Vars from RXBuff
		GetTempStr(ESP8266_ParseRXBuff("temp"), TempC);
		GetTempStr(ESP8266_ParseRXBuff("temp_min"), TMinC);
		GetTempStr(ESP8266_ParseRXBuff("temp_max"), TMaxC);
		GetStr(ESP8266_ParseRXBuff("description"), WDescription);
		GetStr(ESP8266_ParseRXBuff("name"), WLocation);
		GetStr(ESP8266_ParseRXBuff("country"), WCountry);
	}
	
  ESP8266_CloseTCPConnection(); //overwrited the Buffer
	UART_OutString("Done. Closed Connection. \n\r");
 }

 
void PrintHeader(void){
	ST7735_FillScreen(ST7735_BLUE);
	ST7735_SetCursor(0,1);
	
	printf("%s, %s\n", WLocation, WCountry);
	printf("--------------------\n");
}
void PrintWeather(void){
	ST7735_SetCursor(0,4);

	printf("Temp: %s C\n", TempC);
	printf("Min : %s C\n", TMinC);
	printf("Max : %s C\n", TMaxC);

	printf("Description:\n %s", WDescription);

	LED_BlueOff();
	LED_GreenOn();
}

	
//helper function: return length of string marked by "
uint32_t CharEndStr_Length(char * str, char c){
	uint32_t i; 
	for(i = 0; str[i] != c; i++);
	return i;
}

/*
//===================================================Custom Weather Functions=================================

//--------ESP8266_GetWeather-------- !!!
// Processes RXBuffer and prints weather data to screen (printf) 
// Cur. Temp, High/Low, and Description
// Inputs: none
// Outputs: none
// busy-wait synchronization
void ESP8266_GetWeather(void){
	//RXBuff format: "main":{"temp":281.66,"feels_like":278.11
	char *prntStr = strstr(RXBuffer, "temp");
	printf("Temp: %.6s\n", prntStr+6); //ptr math to move past 'temp":' in output
	prntStr = strstr(RXBuffer, "temp_max");
	printf("High: %.6s\n", prntStr+10);
	prntStr = strstr(RXBuffer, "temp_min");
	printf("Low: %.6s\n", prntStr+10);
	
	//more complicated because not fixed number - need to isolate substring: "[some text]" 
	prntStr = strstr(RXBuffer, "description");
	prntStr+= 14; //skip description":" (aka 14 characters) -> @first char description (skipped first ")
	printf("Description:\n %.*s\n", CharEndStr_Length(prntStr, '"'), prntStr);
	
	return;
}

//--------ESP8266_GetDateLoc_OW-------- 
// Processes RXBuffer and prints Date/Time & Location
// Inputs: none
// Outputs: none
// busy-wait synchronization
void ESP8266_GetDateLoc_OW(void){ //detailed comments on GetWeather
	//City Name
	char *prntStr = strstr(RXBuffer, "name");
	prntStr+= 7; //skip name":" (aka 7 characters) -> @first char (skip first ")
	printf("%.*s, ", CharEndStr_Length(prntStr, '"'), prntStr);
	//Country
	prntStr = strstr(RXBuffer, "country");
	prntStr+= 10; //skip country":" (aka 10 characters)
	printf("%.*s\n", CharEndStr_Length(prntStr, '"'), prntStr);
	
	//form: Date: Wed, 20 Nov 2024 18:06:29 GMT
	prntStr = strstr(RXBuffer, "Date");
	prntStr+= 6; //skip Date": (aka 6 characters)
	uint32_t i = CharEndStr_Length(prntStr, ':') - 3; //end of Date/start of Time
	printf("%.*s\n", i, prntStr);
	printf("%.*s", CharEndStr_Length(prntStr, '\n') - i, prntStr + i);
	return;
}

//============================================================================================================
*/