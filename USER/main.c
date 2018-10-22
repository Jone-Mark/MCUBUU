/*
WEbsocket：http://47.106.209.211:8888/
*/
#include "main.h"
#include "dht11.h"

//如果连接的BUU，设置为1
//如果连接的普通路由器，设置为2，并且修改账号密码
#define WIFISSID 1

//BUU校园网
char *buussid="BUU";  //buuwifi名称
char *buupasswd="";   //buu无线密码为空
char *buu_user="2015080331133"; //buu账号
char *buu_pass="730232";    //buu密码

//普通路由器
char *ssid="iot 921";  //wifi名称
char *passwd="iot123456";   //密码

extern  u16 flag;

u8 DATA1[]="";
u8 DATA2[]="";
u8 BUUDATA[]="";

u8 temperature=0;	
u8 humidity=0;
float temperature_l=0;
float temp=0;
int main(void)
{		
  float longitude=0;//经纬度
	float latitude=0;	
	
  unsigned int  get_btn_time = 0,LED_time=0;	
	u16 send_map_time = 0,send_value_time = 0;
 
  u8 i=0;
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 //串口初始化为115200
  usart3_init(115200); //初始化串口3 
	
 	LED_Init();			     //LED端口初始化
	KEY_Init();          //初始化与按键连接的硬件接口
	
	
	#if(WIFISSID==1)
	BUU_Init();          //BUUWIFI初始化
	#elif(WIFISSID==1)
	ESP8266_Init();      //初始化ESP8266相关
	#endif	
	
	TIM3_Int_Init(49,35999); //72MHz，36000分频-500us，50重载值。则中断周期为500us * 50 = 25ms
	//#define random(x) (rand()%x)随机数函数
  DHT11_Init();
 	while(1)
	{	

//		for(i=0;i<250;i++)
//		{
//			DHT11_Read_Data(&temperature,&humidity);	//读取温湿度值100次平均值
//			delay_ms(200);
//			temperature_l=temperature+temperature_l;
//		}
		
//		if(timInfo.timer3Out - LED_time >= 40)	//1000ms一次(25ms中断)LED闪烁，系统运行
//		{
//			LED_time = timInfo.timer3Out;
//      LED1=!LED1;		
//		}
	
//    if(timInfo.timer3Out - get_btn_time >= 30)	//50ms一次(25ms中断)去get按钮
//		{
//			get_btn_time = timInfo.timer3Out;
//			GET_btn(); 
//		}
			
//		if(timInfo.timer3Out - send_value_time >= 50)	//50000ms一次(25ms中断)上传数值
//		{
//			send_value_time = timInfo.timer3Out;

//			for(i=0;i<250;i++)
//			{
//				DHT11_Read_Data(&temperature,&humidity);	//读取温湿度值100次平均值
//				delay_ms(300);
//				temperature_l=temperature+temperature_l;
//			}
//		
//		  temp=temperature_l/251;
//		  temperature_l=0;	

//			sprintf((char *)DATA1,"id=%d&value=%.2lf\r\n",0,temp);
//			printf((char *)DATA1);
//			POST_value();
//		}
		
//		if(timInfo.timer3Out - send_map_time >= 4000)		//10000ms一次(25ms中断)上传地图点不能一样
//		{
//			longitude=116.426724;
//	    latitude=39.99066;	
//			send_map_time = timInfo.timer3Out;
//			
//			sprintf((char *)DATA2,"long=%lf&lat=%lf",longitude,latitude);
//			POST_map();
//		}
		
		//天气获取和图像上传请根据API文档自行修改函数
	}	 
} 
 
u8 send_8266_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	u16 len;
	u16 t; 
	USART3_RX_STA=0;
	u3_printf("%s\r\n",cmd);	//发送命令
				
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			if(USART3_RX_STA&0X8000)//接收到期待的应答结果
			{
				len=USART3_RX_STA&0x3fff;//得到此次接收到的数据长度
				for(t=0;t<len;t++)
				{
					USART_SendData(USART1, USART3_RX_BUF[t]);//向串口1发送数据
					if(USART3_RX_BUF[t]=='b' && USART3_RX_BUF[t+1]=='t' &&USART3_RX_BUF[t+2]=='n')
					{
						if(USART3_RX_BUF[t+4]=='1')
							LED0=0;    //LED0开
						else if(USART3_RX_BUF[t+4]=='0')
							LED0=1;    //LED0开	
					}
					while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
				}
				if(atk_8266_check_cmd(ack))
				{
					printf("ack:%s\r\n",(u8*)ack);
					break;//得到有效数据 
				}
        memset(USART3_RX_BUF,0,sizeof(USART3_RX_BUF));
        USART3_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
} 

u8* atk_8266_check_cmd(u8 *str)
{
	char *strx=0;
	if(USART3_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//添加结束符
		strx=strstr((const char*)USART3_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}

void RST_8266(void)
{
	ESP8266_HIGH;	
	ESP8266_LOW;
	delay_ms(1000);
  ESP8266_HIGH;	
}

u8 cwjap[]="";
void ESP8266_Init(void)
{
	RST_8266();
	
	while(send_8266_cmd((u8 *)"AT",(u8 *)"OK",200))//检查WIFI模块是否在线
	{
		LED0=!LED0;//红灯闪
		delay_ms(800);
		printf("WIFI模块未正确连接\r\n");	
	} 
	LED0=1;
	printf("WIFI模块已连接\r\n");	
	
	send_8266_cmd((u8 *)"AT+CWMODE=3",(u8 *)"OK",200);  //设置wifi应用模式
	
	send_8266_cmd((u8 *)"AT+RST",(u8 *)"OK",1000);  //重启
	sprintf((char *)cwjap,"AT+CWJAP=\"%s\",\"%s\"",ssid,passwd);
	printf((const char *)cwjap);
	send_8266_cmd(cwjap,(u8 *)"OK",1000);  //连接无线路由器
	LED1=0;
	
  send_8266_cmd((u8 *)"AT+CIPMODE=1",(u8 *)"OK",200);  //进入透传模式
	send_8266_cmd((u8 *)"AT+CIPSTART=\"TCP\",\"47.106.209.211\",8888",(u8 *)"OK",200);//连接TCP
	
	send_8266_cmd((u8 *)"AT+CIPSEND",(u8 *)"OK",200);  //发送命令
}

u8 BUU_TCP_DATA[300];
void BUU_Init(void)
{
	u8 size=0; 
	RST_8266();
	
	while(send_8266_cmd((u8 *)"AT",(u8 *)"OK",500))//检查WIFI模块是否在线
	{
		LED0=!LED0;//红灯闪
		delay_ms(800);
		printf("WIFI模块未正确连接\r\n");	
	} 
	
	LED0=1;
	printf("WIFI模块已连接\r\n");	
	
	send_8266_cmd((u8 *)"AT+CWMODE=3",(u8 *)"OK",200);  //设置wifi应用模式
	send_8266_cmd((u8 *)"AT+RST",(u8 *)"OK",1000);  //重启
	sprintf((char *)cwjap,"AT+CWJAP=\"%s\",\"%s\"",buussid,buupasswd);
	//printf((const char *)cwjap);
	send_8266_cmd(cwjap,(u8 *)"OK",1000);  //连接无线路由器
	LED1=0;

	send_8266_cmd((u8 *)"AT+CIPSTART=\"TCP\",\"10.11.7.40\",80",(u8 *)"OK",200);//连接TCP
	send_8266_cmd((u8 *)"AT+CIPMODE=1",(u8 *)"OK",200);  //进入透传模式
	send_8266_cmd((u8 *)"AT+CIPSEND",(u8 *)"OK",200);  //发送命令
//	
	sprintf((char *)BUUDATA,"0MKKey=123&DDDDD=%s&upass=%s",buu_user,buu_pass);
	size=strlen((char *)BUUDATA);
//	printf(BUUDATA);
	sprintf((char *)BUU_TCP_DATA,"POST http://10.11.7.40/ HTTP/1.1\r\n"
			                         "Host: 10.11.7.40\r\n"
			                         "Connection: keep-alive\r\n"
					                     "Content-Length: %d\r\n"
					                     "Content-Type: application/x-www-form-urlencoded\r\n"
					                     "\r\n"
					                     "%s",size,BUUDATA);
	
  printf((char *)BUU_TCP_DATA);
  send_8266_cmd(BUU_TCP_DATA,(u8 *)"OK",500);//连接TCP
	memset(BUU_TCP_DATA,0,sizeof(BUU_TCP_DATA));
	delay_ms(50);
	u3_printf("+++");        //退出透传模式
	delay_ms(50);
	u3_printf("+++");        //退出透传模式
	while(send_8266_cmd((u8 *)"AT",(u8 *)"OK",500))//检查WIFI模块是否在线
	{
		LED0=!LED0;//红灯闪
		delay_ms(800);
		printf("未退出透传模式\r\n");	
		u3_printf("+++");        //退出透传模式
	} 
	printf((char *)"退出透传模式\r\n\r\n");
	send_8266_cmd((u8 *)"AT+CIPCLOSE",(u8 *)"OK",200);  //设置wifi应用模式
	
	//////////连接tok服务器
	send_8266_cmd((u8 *)"AT+CIPSTART=\"TCP\",\"47.106.209.211\",8888",(u8 *)"OK",200);//连接TCP
	send_8266_cmd((u8 *)"AT+CIPMODE=1",(u8 *)"OK",200);  //进入透传模式
	send_8266_cmd((u8 *)"AT+CIPSEND",(u8 *)"OK",200);  //发送命令
}
