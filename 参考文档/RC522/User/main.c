/*         风轻云舒      */
/* 		  2022/8/28  	 */

/*   RFID_RC522门禁系统   */
/*   接线： RC522模块：RST接PB11；MISO接PB10； MOSI接PB1；SCK接PB0；SDA接PA7；
			0.96寸OLED:scl接PB8，sda接PB9；
		    有源蜂鸣器：I/O接PA3；
		    180度舵机：信号线接PA1(5V电源驱动)；
		    按键1：PB12； 按键2：PB14；按键3：PA9；按键4：PA12；  */


/*	 具体功能描述：
	 读卡模式：	上电后默认为读卡模式，按下2键后进入写卡模式，按下3键进入删卡模式
	 写卡模式： 在写卡模式下将卡片靠近感应区即可写卡成功，已录入的卡片不能重复录入，在写卡模式下按下1键进入读卡模式，按下3键进入删卡模式
	 删卡模式： 在删卡模式下按下1键或2键选择要删除的卡片，按下3键确认删除，按下4键返回到读卡模式。
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
extern uint8_t UI0[4];							//卡片0ID数组
extern uint8_t UI1[4];							//卡片1ID数组
extern uint8_t UI2[4];							//卡片2ID数组
extern uint8_t UI3[4];							//卡片3ID数组

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
	Read_Card();		//上电后从flash中读出已录入的卡数据
	
	while(1)
	{	
		KeyNum = Key_Scan();
		
		OLED_ShowString(1,1,"  Welcome Home! ");
		OLED_ShowString(2,1,"                ");
		
		if(KeyNum==1)	//按下1键进入寻卡模式，上电初始化为寻卡模式
		{
			flag_scan=1;
			flag_addcard=0;
			flag_deletecard=0;
		}
		if(KeyNum==2)	//按下2键进入写卡模式
		{
			flag_scan=0;
			flag_addcard=1;
			flag_deletecard=0;
		}
		if(KeyNum==3)	//按下3键进入删卡模式
		{
			flag_scan=0;
			flag_addcard=0;
			flag_deletecard=1;
		}
		
		if(flag_scan==1)
		{
			RFID_Check();
		}
		
		while(flag_addcard==1)	//写卡模式
		{
			KeyNum = Key_Scan();
			if(KeyNum==1)		//写卡模式中按下1键进入寻卡模式
			{
				flag_scan=1;
				flag_addcard=0;
				flag_deletecard=0;
			}
			if(KeyNum==3)		//写卡模式中按下3键进入删卡模式
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
					if(UI0[0] == 0xFF && UI0[1] == 0xFF && UI0[2] == 0xFF && UI0[3] == 0xFF) tempcard = 0;	//判断系统各卡数据区是否为空，为空才能写入新卡
					else if(UI1[0] == 0xFF && UI1[1] == 0xFF && UI1[2] == 0xFF && UI1[3] == 0xFF) tempcard = 1;
					else if(UI2[0] == 0xFF && UI2[1] == 0xFF && UI2[2] == 0xFF && UI2[3] == 0xFF) tempcard = 2;
					else if(UI3[0] == 0xFF && UI3[1] == 0xFF && UI3[2] == 0xFF && UI3[3] == 0xFF) tempcard = 3;
					else tempcard = 4;
					
					if(UID[0]==UI0[0] && UID[1]==UI0[1] && UID[2]==UI0[2] && UID[3]==UI0[3])	//判断新卡是否已经录入
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
							UI0[0] = UID[0];	//将新卡数据写入UI0[]数组
							UI0[1] = UID[1];
							UI0[2] = UID[2];
							UI0[3] = UID[3];
							FLASH_W(FLASH_ADDR1,UI0[0],UI0[1],UI0[2],UI0[3]);	//将新卡数据存入flash
							OLED_ShowString(1,1,"  Add Card 1 OK ");	//写卡成功，蜂鸣器响一声
							Buzzer1();
							WaitCardOff();	//等待卡片移走
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
							OLED_ShowString(1,1,"   NO memory    ");	//若4个存卡数组均已存入卡片则显示无数据空间，蜂鸣器发出警报
							Buzzer_Alarm();
							WaitCardOff();
						}
						case 5:
						{	
							OLED_ShowString(1,1," Existing Card! ");	//若新卡已录入，蜂鸣器发出警报
							Buzzer_Alarm();
							WaitCardOff();
						}
						default:break;
					 }
				}
			}
		}
		
		while(flag_deletecard==1)	//删卡模式
		{
			OLED_ShowString(1,1,"  Delete Card:  ");
			KeyNum = Key_Scan();
			
			if(KeyNum==4)	//按下4键退出删卡模式，进入寻卡模式
			{
				flag_scan=1;
				flag_addcard=0;
				flag_deletecard=0;
			}
			
			
			if(KeyNum==1)	//在删卡模式下按下1键，选择要删除的卡片，卡片序列增加
			{
				select++;
				if(select>=4 || select<1)select=0;
			}
			if(KeyNum==2)	//在删卡模式下按下2键，选择要删除的卡片，卡片序列减少
			{
				select--;
				if(select>=4 || select<1)select=0;
			}
			
			
			switch (select)
			{
				case 0:
				{
					OLED_ShowString(2,1," Delete Card 1 ?");
					if(KeyNum==3)	//在删卡模式下按下3键，删除对应的卡片
					{
						FLASH_Clear(FLASH_ADDR1);	
						UI0[0]=0xFF;
						UI0[1]=0xFF;
						UI0[2]=0xFF;
						UI0[3]=0xFF;
						OLED_ShowString(2,1,"Clear Card 1 OK ");
						Buzzer1();	//删除成功后蜂鸣器响一声
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


//读卡函数，读卡并获取卡编号
void RFID_Check()									
{
	cardnumber = Rc522Test();	//获取卡编号
	if(cardnumber == 0)			//如果为0，表示“卡片错误”，系统中没有这张卡
	{
		OLED_ShowString(1,1,"   Error card   ");
		Buzzer_Alarm();		//蜂鸣器发出警报
		WaitCardOff();		//等待卡片移开
	}
	else if(cardnumber==1||cardnumber==2||cardnumber==3||cardnumber == 4)			//如果卡编号为1-4，说明是系统中的4张卡
	{	
		OLED_ShowString(1,1,"The CardID is:  ");
		OLED_ShowNum(1,15,cardnumber,2);
		Buzzer2();			//蜂鸣器响两声
		Servo_SetAngle(90);	//舵机旋转90度维持1.5秒
		Delay_ms(1500);
		Servo_SetAngle(0);
		WaitCardOff();		//等待卡片移开
	}	
}

//从flash中读取各卡信息
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
