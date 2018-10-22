#ifndef __MAIN_H
#define __MAIN_H	 
#include "sys.h"

#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "usart3.h"
#include "string.h" 
#include "timer.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>



u8 send_8266_cmd(u8 *cmd,u8 *ack,u16 waittime);
//u8 send_cmd(u8 *cmd);
u8* atk_8266_check_cmd(u8 *str);

void RST_8266(void);


void BUU_Init(void);
void ESP_Init(void);
void ESP8266_Init(void);

void POST_value(void);
void GET_btn(void);
void GET_value(u8 start,u8 end);
void POST_map(void);


	
		 				    
#endif
