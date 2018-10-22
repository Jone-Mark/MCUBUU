/*
WEbsocket��http://47.106.209.211:8888/
*/
#include "main.h"
#include "dht11.h"

//������ӵ�BUU������Ϊ1
//������ӵ���ͨ·����������Ϊ2�������޸��˺�����
#define WIFISSID 1

//BUUУ԰��
char *buussid="BUU";  //buuwifi����
char *buupasswd="";   //buu��������Ϊ��
char *buu_user="2015080331133"; //buu�˺�
char *buu_pass="730232";    //buu����

//��ͨ·����
char *ssid="iot 921";  //wifi����
char *passwd="iot123456";   //����

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
  float longitude=0;//��γ��
	float latitude=0;	
	
  unsigned int  get_btn_time = 0,LED_time=0;	
	u16 send_map_time = 0,send_value_time = 0;
 
  u8 i=0;
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 //���ڳ�ʼ��Ϊ115200
  usart3_init(115200); //��ʼ������3 
	
 	LED_Init();			     //LED�˿ڳ�ʼ��
	KEY_Init();          //��ʼ���밴�����ӵ�Ӳ���ӿ�
	
	
	#if(WIFISSID==1)
	BUU_Init();          //BUUWIFI��ʼ��
	#elif(WIFISSID==1)
	ESP8266_Init();      //��ʼ��ESP8266���
	#endif	
	
	TIM3_Int_Init(49,35999); //72MHz��36000��Ƶ-500us��50����ֵ�����ж�����Ϊ500us * 50 = 25ms
	//#define random(x) (rand()%x)���������
  DHT11_Init();
 	while(1)
	{	

//		for(i=0;i<250;i++)
//		{
//			DHT11_Read_Data(&temperature,&humidity);	//��ȡ��ʪ��ֵ100��ƽ��ֵ
//			delay_ms(200);
//			temperature_l=temperature+temperature_l;
//		}
		
//		if(timInfo.timer3Out - LED_time >= 40)	//1000msһ��(25ms�ж�)LED��˸��ϵͳ����
//		{
//			LED_time = timInfo.timer3Out;
//      LED1=!LED1;		
//		}
	
//    if(timInfo.timer3Out - get_btn_time >= 30)	//50msһ��(25ms�ж�)ȥget��ť
//		{
//			get_btn_time = timInfo.timer3Out;
//			GET_btn(); 
//		}
			
//		if(timInfo.timer3Out - send_value_time >= 50)	//50000msһ��(25ms�ж�)�ϴ���ֵ
//		{
//			send_value_time = timInfo.timer3Out;

//			for(i=0;i<250;i++)
//			{
//				DHT11_Read_Data(&temperature,&humidity);	//��ȡ��ʪ��ֵ100��ƽ��ֵ
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
		
//		if(timInfo.timer3Out - send_map_time >= 4000)		//10000msһ��(25ms�ж�)�ϴ���ͼ�㲻��һ��
//		{
//			longitude=116.426724;
//	    latitude=39.99066;	
//			send_map_time = timInfo.timer3Out;
//			
//			sprintf((char *)DATA2,"long=%lf&lat=%lf",longitude,latitude);
//			POST_map();
//		}
		
		//������ȡ��ͼ���ϴ������API�ĵ������޸ĺ���
	}	 
} 
 
u8 send_8266_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	u16 len;
	u16 t; 
	USART3_RX_STA=0;
	u3_printf("%s\r\n",cmd);	//��������
				
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
		while(--waittime)	//�ȴ�����ʱ
		{
			delay_ms(10);
			if(USART3_RX_STA&0X8000)//���յ��ڴ���Ӧ����
			{
				len=USART3_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
				for(t=0;t<len;t++)
				{
					USART_SendData(USART1, USART3_RX_BUF[t]);//�򴮿�1��������
					if(USART3_RX_BUF[t]=='b' && USART3_RX_BUF[t+1]=='t' &&USART3_RX_BUF[t+2]=='n')
					{
						if(USART3_RX_BUF[t+4]=='1')
							LED0=0;    //LED0��
						else if(USART3_RX_BUF[t+4]=='0')
							LED0=1;    //LED0��	
					}
					while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
				}
				if(atk_8266_check_cmd(ack))
				{
					printf("ack:%s\r\n",(u8*)ack);
					break;//�õ���Ч���� 
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
	if(USART3_RX_STA&0X8000)		//���յ�һ��������
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//��ӽ�����
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
	
	while(send_8266_cmd((u8 *)"AT",(u8 *)"OK",200))//���WIFIģ���Ƿ�����
	{
		LED0=!LED0;//�����
		delay_ms(800);
		printf("WIFIģ��δ��ȷ����\r\n");	
	} 
	LED0=1;
	printf("WIFIģ��������\r\n");	
	
	send_8266_cmd((u8 *)"AT+CWMODE=3",(u8 *)"OK",200);  //����wifiӦ��ģʽ
	
	send_8266_cmd((u8 *)"AT+RST",(u8 *)"OK",1000);  //����
	sprintf((char *)cwjap,"AT+CWJAP=\"%s\",\"%s\"",ssid,passwd);
	printf((const char *)cwjap);
	send_8266_cmd(cwjap,(u8 *)"OK",1000);  //��������·����
	LED1=0;
	
  send_8266_cmd((u8 *)"AT+CIPMODE=1",(u8 *)"OK",200);  //����͸��ģʽ
	send_8266_cmd((u8 *)"AT+CIPSTART=\"TCP\",\"47.106.209.211\",8888",(u8 *)"OK",200);//����TCP
	
	send_8266_cmd((u8 *)"AT+CIPSEND",(u8 *)"OK",200);  //��������
}

u8 BUU_TCP_DATA[300];
void BUU_Init(void)
{
	u8 size=0; 
	RST_8266();
	
	while(send_8266_cmd((u8 *)"AT",(u8 *)"OK",500))//���WIFIģ���Ƿ�����
	{
		LED0=!LED0;//�����
		delay_ms(800);
		printf("WIFIģ��δ��ȷ����\r\n");	
	} 
	
	LED0=1;
	printf("WIFIģ��������\r\n");	
	
	send_8266_cmd((u8 *)"AT+CWMODE=3",(u8 *)"OK",200);  //����wifiӦ��ģʽ
	send_8266_cmd((u8 *)"AT+RST",(u8 *)"OK",1000);  //����
	sprintf((char *)cwjap,"AT+CWJAP=\"%s\",\"%s\"",buussid,buupasswd);
	//printf((const char *)cwjap);
	send_8266_cmd(cwjap,(u8 *)"OK",1000);  //��������·����
	LED1=0;

	send_8266_cmd((u8 *)"AT+CIPSTART=\"TCP\",\"10.11.7.40\",80",(u8 *)"OK",200);//����TCP
	send_8266_cmd((u8 *)"AT+CIPMODE=1",(u8 *)"OK",200);  //����͸��ģʽ
	send_8266_cmd((u8 *)"AT+CIPSEND",(u8 *)"OK",200);  //��������
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
  send_8266_cmd(BUU_TCP_DATA,(u8 *)"OK",500);//����TCP
	memset(BUU_TCP_DATA,0,sizeof(BUU_TCP_DATA));
	delay_ms(50);
	u3_printf("+++");        //�˳�͸��ģʽ
	delay_ms(50);
	u3_printf("+++");        //�˳�͸��ģʽ
	while(send_8266_cmd((u8 *)"AT",(u8 *)"OK",500))//���WIFIģ���Ƿ�����
	{
		LED0=!LED0;//�����
		delay_ms(800);
		printf("δ�˳�͸��ģʽ\r\n");	
		u3_printf("+++");        //�˳�͸��ģʽ
	} 
	printf((char *)"�˳�͸��ģʽ\r\n\r\n");
	send_8266_cmd((u8 *)"AT+CIPCLOSE",(u8 *)"OK",200);  //����wifiӦ��ģʽ
	
	//////////����tok������
	send_8266_cmd((u8 *)"AT+CIPSTART=\"TCP\",\"47.106.209.211\",8888",(u8 *)"OK",200);//����TCP
	send_8266_cmd((u8 *)"AT+CIPMODE=1",(u8 *)"OK",200);  //����͸��ģʽ
	send_8266_cmd((u8 *)"AT+CIPSEND",(u8 *)"OK",200);  //��������
}
