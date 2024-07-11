#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Servo.h"
#include "Key.h"
#include "LED.h"
#include "BEEP.h"
#include "USART.h"
#include "RC522.h"
#include "as608.h"
#include "as608_use.h"
#include "esp8266.h"
#include "onenet.h"
#include "Flash.h"
#include "DHT11.H"
#include "timer.h"
#include "servo.h"
uint8_t KeyNum;
float Angle;
extern SysPara AS608Para;							/*ָ��ģ��AS608����*/

extern u16 ValidN;										/*ģ������Чָ�Ƹ���*/

u8 Line = 5;  //��ť�����л�������
extern u16 ValidN;											/*ģ������Чָ�Ƹ���*/
u8 lock = 0; //������״̬ 
u8 ESP8266_INIT_OK;//wifi��ʼ���ɹ���ı�־


u8 interface_logo = 1; //����ҳ����ʾ��־
u8 Led_Status; //�Ʊ�־λ
u8 Power_Status; //�Ž���Դ��־λ
u8 Beep_Status; //������������־λ

u8 Fail_Num; // �Ž������������

u8 upload_card_number_flag; //���Ϳ��Ÿ�С�����־
u8 upload_fin_number_flag;//����ָ�Ƹ�����С�����־


char oledBuf[128];

char PUB_BUF[256];//�ϴ����ݵ�buf
const char *devSubTopic[] = {"/myhome/sub"};
const char devPubTopic[] = "/myhome/pub";

u8 humidityH;	  //ʪ����������
u8 humidityL;	  //ʪ��С������
u8 temperatureH;   //�¶���������
u8 temperatureL;   //�¶�С������


void unlock_interface(void);  //������̬��ʾ����
void main_interface(void);  //����������
void main1_interface(void);  //������������� �ڶ�ҳ

void Init_ALL(void)
{
	u8 ensure;
	OLED_Init();
	Servo_Init();
	Key_Init();
	LED_Init();
	BEEP_Init();
	DHT11_Init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	USART1_Init(115200);

	RC522_IO_Init();//RC522��ʼ��
	PcdReset();  //��λRC522
	PcdAntennaOff();  //�ر�����
	delay_ms(100);
	PcdAntennaOn();  //��������
	
	Read_Card(); //��ȡ������Ϣ
	
	//��ʼ���̵�������
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	GPIO_SetBits(GPIOA,GPIO_Pin_8);
	Power_Status = 1;

	BEEP_ON();
	delay_ms(500);
	BEEP_OFF();
	DEBUG_LOG("��ʼ����\r\n");
	
	USART2_Init(115200);
	DEBUG_LOG("UART2��ʼ��			[OK]");
	
	DEBUG_LOG("��ʼ��ESP8266 WIFIģ��...");


	if(!ESP8266_INIT_OK){
		OLED_Clear();
		sprintf(oledBuf,"Waiting For");
		OLED_ShowString(1,4,oledBuf);
		sprintf(oledBuf,"WiFi");
		OLED_ShowString(2,8,oledBuf);
		sprintf(oledBuf,"Connection");
		OLED_ShowString(3,4,oledBuf);
				
		delay_ms(500);
		OLED_ShowChar(4,12,'.');
		delay_ms(500);
		OLED_ShowChar(4,13,'.');
		delay_ms(500);
		OLED_ShowChar(4,14,'.');
		delay_ms(500);
		OLED_ShowChar(4,15,'.');
	}
	
	ESP8266_Init();					//��ʼ��ESP8266	

	while(OneNet_DevLink()){//����OneNET
				delay_ms(500);
	}	

	OneNet_Subscribe(devSubTopic,1);//����MQTT
	OneNet_Publish(devPubTopic,"mqtt test send msg");
	OLED_Clear();

	
	USART3_Init(57600);	/*��ʼ������3,������ָ��ģ��ͨѶ*/
	PS_StaGPIO_Init();					/*��ʼ��FR��״̬����*/


	while(PS_HandShake(&AS608Addr))			/*��AS608ģ������*/
	{
		delay_ms(400);
		DEBUG_LOG("δ��⵽ģ��\r\n");
		delay_ms(1000);
		DEBUG_LOG("������������ģ��\r\n"); 
	}
	DEBUG_LOG("��ָ��ģ�����ֳɹ���r\n");
	DEBUG_LOG("ͨѶ�ɹ�\r\n");
	DEBUG_LOG("������:%d   ��ַ:%x\r\n",57600,AS608Addr);		/*��ӡ��Ϣ*/
	ensure=PS_ValidTempleteNum(&ValidN);										/*����ָ�Ƹ���*/
	if(ensure!=0x00)
		ShowErrMessage(ensure);								/*��ʾȷ���������Ϣ*/
	ensure=PS_ReadSysPara(&AS608Para);  		/*������ */
	if(ensure==0x00)
	{
		DEBUG_LOG("������:%d     �Աȵȼ�: %d",AS608Para.PS_max-ValidN,AS608Para.PS_level);
	}
	else
		ShowErrMessage(ensure);		
	
	TIM3_Int_Init(2499,7199);
	

}	

int main(void)
{
	unsigned char *dataPtr = NULL;//������ƽ̨������
	unsigned short timeCount = 0;	//���ͼ������
	Init_ALL();//����ģ���豸��ʼ��

	while (1)
	{

		//���ݻط���С����

		if(timeCount % 60 == 0)//1500ms / 25 = 60  1.5��ִ��һ��
		{

			if(upload_card_number_flag)
			{
				upload_card_number(); //�ط��洢�Ŀ������ݸ�С����
				upload_card_number_flag = 0;
			
			}else if(upload_fin_number_flag)
			{
			
				upload_fin_number();//�ط�ָ�Ƹ������ݸ�С����
				upload_fin_number_flag = 0;
				
			
			}else{
			
			/********** ��ȡLED0��״̬ **************/
			Led_Status = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_12);
				/********** ��ʪ�ȴ�������ȡ����**************/
			DHT11_Read_Data(&humidityH,&humidityL,&temperatureH,&temperatureL);
			DEBUG_LOG(" | ʪ�ȣ�%d.%d C | �¶ȣ�%d.%d %%  ",humidityH,humidityL,temperatureH,temperatureL);
			
			DEBUG_LOG("==================================================================================");
			DEBUG_LOG("�������� ----- OneNet_Publish");
			sprintf(PUB_BUF,"{\"Hum\":%d.%d,\"Temp\":%d.%d,\"Door\":%d,\"Led\":%d,\"Beep\":%d,\"Power\":%d}",
				humidityH,humidityL,temperatureH,temperatureL,lock,Led_Status?0:1,Beep_Status,Power_Status);
			OneNet_Publish(devPubTopic, PUB_BUF);
			DEBUG_LOG("==================================================================================");
			ESP8266_Clear();			
			
			}
									
		}	
		
		if(lock)//����״̬
		{
			KeyNum = Key_GetNum();//ɨ�谴��
			
		if (KeyNum == 1)
		{
			
			OLED_Clear();
			if(Line <= 5)
			{
				Line = 8;
				interface_logo = 1;
				
			}else{
				Line --;
			}
			if(interface_logo == 1)
			{
					main_interface();	//�������������	��һҳ
			
			}else{
					main1_interface();	//�������������	�ڶ�ҳ
			}
			
		}else if(KeyNum == 2)
		{
			
			OLED_Clear();
			if(Line >= 8)
			{
				Line = 5;
				interface_logo = 2;
			}else{
				Line ++;
			}
			
			
			if(interface_logo == 1)
			{
					main_interface();	//�������������	��һҳ
			
			}else{
					main1_interface();	//�������������	�ڶ�ҳ
			}
		}else if(KeyNum == 3) //ȷ�ϰ���
		{
			
			if(interface_logo == 1) //�����һҳ��ѡ��ȷ��
			{
				if(Line == 5) //�ƹ�任
				{
								
					LED1_Turn();
		
				}			

				if(Line == 6) //���ָ��
				{
									
					Add_FR(ValidN++);
				
		
				}				
				
							
				if(Line == 7) //ɾ��ָ��
				{
					Del_FR(--ValidN);
						
													
				}

				if(Line == 8)
				{
					close_door();
					OLED_Clear();
					lock = 0;
					continue;
									
				}	

					main_interface();
			
			
			}else if(interface_logo == 2)
			{//�ڶ�ҳѡ��ȷ��
				if(Line == 5) //���ӿ���
				{
								
					RC522_Add_card();
		
				}			

				if(Line == 6) //ɾ������
				{
					
				
					RC522_Rm_card();
		
				}				
				
							
				if(Line == 7) //�򿪾���
				{

					if(Beep_Status == 1)
					{
						Beep_Status = 0;
					}else{
					
						Beep_Status = 1;
					}
													
				}

				if(Line == 8) //���ص�Դ
				{
					if (GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_8) == 0)
					{
						GPIO_SetBits(GPIOA, GPIO_Pin_8);
						Power_Status = 1;
					}
					else
					{
						GPIO_ResetBits(GPIOA, GPIO_Pin_8);
							Power_Status = 0;
					}
									
				}	
					main1_interface();		//�������������	�ڶ�ҳ		
			
			
			}
		
		}			
					
				
		}
		else
		{//����״̬
			OLED_WriteCN(16,1,0,6);    //��ʾ->�����Ž�ϵͳ
			OLED_WriteCN(56,4,1,1);		 //��ʾ��

			sprintf(oledBuf,"T:%d.%d  H:%d.%d%%",temperatureH,temperatureL,humidityH,humidityL);
			OLED_ShowString(8,1,oledBuf);//8*16 ��ABC��			
			RC522_Get_card();//��⵽��ˢ�� ����ʶ����֤����
			Check_FR(); //��⵽��ָ�� ����ʶ����֤����			
					
		}
		
		if(Fail_Num >=5) //�Ž�ʶ���������ﵽ5��
		{
			Beep_Status= 1;	
			DEBUG_LOG("�������� --Warning--- OneNet_Publish");

			OneNet_Publish(devPubTopic, "{\"Warning\":1}");
			DEBUG_LOG("==================================================================================");

			ESP8266_Clear();
			Fail_Num = 0;
		}
	
		
		dataPtr = ESP8266_GetIPD(3); //��ȡƽ̨���ص�����
		if(dataPtr != NULL)
			OneNet_RevPro(dataPtr);//ƽ̨�������ݼ��
		delay_ms(10);
		timeCount++;
	
	}
}


void unlock_interface(void)  //��������
{
//��������
	if(Power_Status == 1) // ��Դ�������ܽ���
	{
			lock = 1;
	}else{
	
			lock = 0;
			OLED_Clear();
			OLED_ShowString(2,5,"fail:notpower!");
			delay_ms(1000);
		OLED_Clear();
		return;
	}
	
	OLED_Clear();
	LED1_ON();
	BEEP_ON();
	delay_ms(200);
	OLED_WriteCN(32,1,2,3);		 //��ʾ�ѽ���
	OLED_WriteCN(56,4,1,1);		 //��ʾ��
	LED1_OFF();
	BEEP_OFF();	
	delay_ms(600);
	open_door();	
	OLED_WriteCN(56,4,3,1);		 //��ʾ����
	delay_ms(800);	
	main_interface();	//�������������	
	

	Fail_Num = 0; //�����������
	Beep_Status = 0; //ȡ������
}
 
void main_interface(void)  //�������������
{
	OLED_Clear();
	OLED_ShowString(Line,2,"->"); // 5 6 7 8 
	OLED_WriteCN(32,0,7,4);
	OLED_WriteCN(32,2,8,4);
	OLED_WriteCN(32,4,9,4);
	OLED_WriteCN(32,6,10,4);	
}

void main1_interface(void)  //������������� �ڶ�ҳ
{
	OLED_Clear();
	OLED_ShowString(Line,2,"->"); // 5 6 7 8 
	OLED_WriteCN(32,0,22,4);
	OLED_WriteCN(32,2,23,4);
	OLED_WriteCN(32,4,24,4);
	OLED_WriteCN(32,6,25,4);	
}

