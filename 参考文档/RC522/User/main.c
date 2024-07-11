/*         ��������      */
/* 		  2022/8/28  	 */

/*   RFID_RC522�Ž�ϵͳ   */
/*   ���ߣ� RC522ģ�飺RST��PB11��MISO��PB10�� MOSI��PB1��SCK��PB0��SDA��PA7��
			0.96��OLED:scl��PB8��sda��PB9��
		    ��Դ��������I/O��PA3��
		    180�ȶ�����ź��߽�PA1(5V��Դ����)��
		    ����1��PB12�� ����2��PB14������3��PA9������4��PA12��  */


/*	 ���幦��������
	 ����ģʽ��	�ϵ��Ĭ��Ϊ����ģʽ������2�������д��ģʽ������3������ɾ��ģʽ
	 д��ģʽ�� ��д��ģʽ�½���Ƭ������Ӧ������д���ɹ�����¼��Ŀ�Ƭ�����ظ�¼�룬��д��ģʽ�°���1���������ģʽ������3������ɾ��ģʽ
	 ɾ��ģʽ�� ��ɾ��ģʽ�°���1����2��ѡ��Ҫɾ���Ŀ�Ƭ������3��ȷ��ɾ��������4�����ص�����ģʽ��
*/

#include "stm32f10x.h"                 
#include "Delay.h"
#include "OLED.h"
#include "Flash.h"
#include "KEY.h"
#include "BUZZER.h"
#include "Servo.h"
#include "RC522.h"

uint8_t cardnumber,KeyNum,tempcard,select=0,flag_scan=1,flag_addcard=0,flag_deletecard=0;

extern uint8_t UID[4],Temp[4];
extern uint8_t UI0[4];							//��Ƭ0ID����
extern uint8_t UI1[4];							//��Ƭ1ID����
extern uint8_t UI2[4];							//��Ƭ2ID����
extern uint8_t UI3[4];							//��Ƭ3ID����

void RFID_Check(void);
void Read_Card(void);

int main(void)
{
	Servo_Init();
	Buzzer_Init();
	Key_Init();
	OLED_Init();
	RFID_Init();
	
	Servo_SetAngle(0);
	Read_Card();		//�ϵ���flash�ж�����¼��Ŀ�����
	
	while(1)
	{	
		KeyNum = Key_Scan();
		
		OLED_ShowString(1,1,"  Welcome Home! ");
		OLED_ShowString(2,1,"                ");
		
		if(KeyNum==1)	//����1������Ѱ��ģʽ���ϵ��ʼ��ΪѰ��ģʽ
		{
			flag_scan=1;
			flag_addcard=0;
			flag_deletecard=0;
		}
		if(KeyNum==2)	//����2������д��ģʽ
		{
			flag_scan=0;
			flag_addcard=1;
			flag_deletecard=0;
		}
		if(KeyNum==3)	//����3������ɾ��ģʽ
		{
			flag_scan=0;
			flag_addcard=0;
			flag_deletecard=1;
		}
		
		if(flag_scan==1)
		{
			RFID_Check();
		}
		
		while(flag_addcard==1)	//д��ģʽ
		{
			KeyNum = Key_Scan();
			if(KeyNum==1)		//д��ģʽ�а���1������Ѱ��ģʽ
			{
				flag_scan=1;
				flag_addcard=0;
				flag_deletecard=0;
			}
			if(KeyNum==3)		//д��ģʽ�а���3������ɾ��ģʽ
			{
				flag_scan=0;
				flag_addcard=0;
				flag_deletecard=1;
			}
			
			OLED_ShowString(1,1,"   Add Card:    ");
			
			if (PcdRequest(REQ_ALL, Temp) == MI_OK)
			{
				if (PcdAnticoll(UID) == MI_OK)
				{
					if(UI0[0] == 0xFF && UI0[1] == 0xFF && UI0[2] == 0xFF && UI0[3] == 0xFF) tempcard = 0;	//�ж�ϵͳ�����������Ƿ�Ϊ�գ�Ϊ�ղ���д���¿�
					else if(UI1[0] == 0xFF && UI1[1] == 0xFF && UI1[2] == 0xFF && UI1[3] == 0xFF) tempcard = 1;
					else if(UI2[0] == 0xFF && UI2[1] == 0xFF && UI2[2] == 0xFF && UI2[3] == 0xFF) tempcard = 2;
					else if(UI3[0] == 0xFF && UI3[1] == 0xFF && UI3[2] == 0xFF && UI3[3] == 0xFF) tempcard = 3;
					else tempcard = 4;
					
					if(UID[0]==UI0[0] && UID[1]==UI0[1] && UID[2]==UI0[2] && UID[3]==UI0[3])	//�ж��¿��Ƿ��Ѿ�¼��
					{
						tempcard = 5;
					}
					if(UID[0]==UI1[0] && UID[1]==UI1[1] && UID[2]==UI1[2] && UID[3]==UI1[3])
					{
						tempcard = 5;
					}
					if(UID[0]==UI2[0] && UID[1]==UI2[1] && UID[2]==UI2[2] && UID[3]==UI2[3])
					{
						tempcard = 5;
					}
					if(UID[0]==UI3[0] && UID[1]==UI3[1] && UID[2]==UI3[2] && UID[3]==UI3[3])
					{
						tempcard = 5;
					}
					
					switch(tempcard)
					{
						case 0:
						{
							UI0[0] = UID[0];	//���¿�����д��UI0[]����
							UI0[1] = UID[1];
							UI0[2] = UID[2];
							UI0[3] = UID[3];
							FLASH_W(FLASH_ADDR1,UI0[0],UI0[1],UI0[2],UI0[3]);	//���¿����ݴ���flash
							OLED_ShowString(1,1,"  Add Card 1 OK ");	//д���ɹ�����������һ��
							Buzzer1();
							WaitCardOff();	//�ȴ���Ƭ����
						}break;
						case 1:
						{
							UI1[0] = UID[0];
							UI1[1] = UID[1];
							UI1[2] = UID[2];
							UI1[3] = UID[3];
							FLASH_W(FLASH_ADDR2,UI1[0],UI1[1],UI1[2],UI1[3]);
							OLED_ShowString(1,1,"  Add Card 2 OK ");
							Buzzer1();
							WaitCardOff();
						}break;
						case 2:
						{
							UI2[0] = UID[0];
							UI2[1] = UID[1];
							UI2[2] = UID[2];
							UI2[3] = UID[3];
							FLASH_W(FLASH_ADDR3,UI2[0],UI2[1],UI2[2],UI2[3]);
							OLED_ShowString(1,1,"  Add Card 3 OK ");
							Buzzer1();
							WaitCardOff();
						}break;
						case 3:
						{
							UI3[0] = UID[0];
							UI3[1] = UID[1];
							UI3[2] = UID[2];
							UI3[3] = UID[3];
							FLASH_W(FLASH_ADDR4,UI3[0],UI3[1],UI3[2],UI3[3]);
							OLED_ShowString(1,1,"  Add Card 4 OK ");
							Buzzer1();
							WaitCardOff();
						}break;
						case 4:
						{	
							OLED_ShowString(1,1,"   NO memory    ");	//��4���濨������Ѵ��뿨Ƭ����ʾ�����ݿռ䣬��������������
							Buzzer_Alarm();
							WaitCardOff();
						}
						case 5:
						{	
							OLED_ShowString(1,1," Existing Card! ");	//���¿���¼�룬��������������
							Buzzer_Alarm();
							WaitCardOff();
						}
						default:break;
					 }
				}
			}
		}
		
		while(flag_deletecard==1)	//ɾ��ģʽ
		{
			OLED_ShowString(1,1,"  Delete Card:  ");
			KeyNum = Key_Scan();
			
			if(KeyNum==4)	//����4���˳�ɾ��ģʽ������Ѱ��ģʽ
			{
				flag_scan=1;
				flag_addcard=0;
				flag_deletecard=0;
			}
			
			
			if(KeyNum==1)	//��ɾ��ģʽ�°���1����ѡ��Ҫɾ���Ŀ�Ƭ����Ƭ��������
			{
				select++;
				if(select>=4 || select<1)select=0;
			}
			if(KeyNum==2)	//��ɾ��ģʽ�°���2����ѡ��Ҫɾ���Ŀ�Ƭ����Ƭ���м���
			{
				select--;
				if(select>=4 || select<1)select=0;
			}
			
			
			switch (select)
			{
				case 0:
				{
					OLED_ShowString(2,1," Delete Card 1 ?");
					if(KeyNum==3)	//��ɾ��ģʽ�°���3����ɾ����Ӧ�Ŀ�Ƭ
					{
						FLASH_Clear(FLASH_ADDR1);	
						UI0[0]=0xFF;
						UI0[1]=0xFF;
						UI0[2]=0xFF;
						UI0[3]=0xFF;
						OLED_ShowString(2,1,"Clear Card 1 OK ");
						Buzzer1();	//ɾ���ɹ����������һ��
						Delay_ms(1500);
					}
				}break;
				case 1:
				{
					OLED_ShowString(2,1," Delete Card 2 ?");
					if(KeyNum==3)
					{
						FLASH_Clear(FLASH_ADDR2);
						UI1[0]=0xFF;
						UI1[1]=0xFF;
						UI1[2]=0xFF;
						UI1[3]=0xFF;
						OLED_ShowString(2,1,"Clear Card 2 OK ");
						Buzzer1();
						Delay_ms(1500);
					}
				}break;
				case 2:
				{
					OLED_ShowString(2,1," Delete Card 3 ?");
					if(KeyNum==3)
					{
						FLASH_Clear(FLASH_ADDR3);
						UI2[0]=0xFF;
						UI2[1]=0xFF;
						UI2[2]=0xFF;
						UI2[3]=0xFF;
						OLED_ShowString(2,1,"Clear Card 3 OK ");
						Buzzer1();
						Delay_ms(1500);;
					}
				}break;
				case 3:
				{
					OLED_ShowString(2,1," Delete Card 4 ?");
					if(KeyNum==3)
					{
						FLASH_Clear(FLASH_ADDR4);
						UI3[0]=0xFF;
						UI3[1]=0xFF;
						UI3[2]=0xFF;
						UI3[3]=0xFF;
						OLED_ShowString(2,1,"Clear Card 4 OK ");
						Buzzer1();
						Delay_ms(1500);
					}
				}break;
				default:break;
			}
		}
	}
}


//������������������ȡ�����
void RFID_Check()									
{
	cardnumber = Rc522Test();	//��ȡ�����
	if(cardnumber == 0)			//���Ϊ0����ʾ����Ƭ���󡱣�ϵͳ��û�����ſ�
	{
		OLED_ShowString(1,1,"   Error card   ");
		Buzzer_Alarm();		//��������������
		WaitCardOff();		//�ȴ���Ƭ�ƿ�
	}
	else if(cardnumber==1||cardnumber==2||cardnumber==3||cardnumber == 4)			//��������Ϊ1-4��˵����ϵͳ�е�4�ſ�
	{	
		OLED_ShowString(1,1,"The CardID is:  ");
		OLED_ShowNum(1,15,cardnumber,2);
		Buzzer2();			//������������
		Servo_SetAngle(90);	//�����ת90��ά��1.5��
		Delay_ms(1500);
		Servo_SetAngle(0);
		WaitCardOff();		//�ȴ���Ƭ�ƿ�
	}	
}

//��flash�ж�ȡ������Ϣ
void Read_Card()
{
	UI0[0]=FLASH_R(FLASH_ADDR1);
	UI0[1]=FLASH_R(FLASH_ADDR1+2);
	UI0[2]=FLASH_R(FLASH_ADDR1+4);
	UI0[3]=FLASH_R(FLASH_ADDR1+6);
	
	UI1[0]=FLASH_R(FLASH_ADDR2);
	UI1[1]=FLASH_R(FLASH_ADDR2+2);
	UI1[2]=FLASH_R(FLASH_ADDR2+4);
	UI1[3]=FLASH_R(FLASH_ADDR2+6);
	
	UI2[0]=FLASH_R(FLASH_ADDR3);
	UI2[1]=FLASH_R(FLASH_ADDR3+2);
	UI2[2]=FLASH_R(FLASH_ADDR3+4);
	UI2[3]=FLASH_R(FLASH_ADDR3+6);
	
	UI3[0]=FLASH_R(FLASH_ADDR4);
	UI3[1]=FLASH_R(FLASH_ADDR4+2);
	UI3[2]=FLASH_R(FLASH_ADDR4+4);
	UI3[3]=FLASH_R(FLASH_ADDR4+6);
}
