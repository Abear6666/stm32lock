#include "stm32f10x.h"
#include "rc522.h"
#include "delay.h"
#include "spi_driver.h"
#include "stm32f10x_spi.h"
#include "usart.h"
#include "Servo.h"
#include "led.h"
#include "oled.h"
#include "servo.h"
#include "Flash.h"
#include "key.h"
#include <stdio.h>
#include <string.h>
#include "onenet.h"
#include "esp8266.h"


uint8_t UI0[4]={0xFF,0xFF,0xFF,0xFF};		 	//卡0ID号
uint8_t UI1[4]={0xFF,0xFF,0xFF,0xFF};	 		//卡1ID号
uint8_t UI2[4]={0xFF,0xFF,0xFF,0xFF};			//卡2ID号
uint8_t UI3[4]={0xFF,0xFF,0xFF,0xFF};			//卡3ID号

uint8_t UID[4],Temp[4];
uint8_t tempcard;
extern u8 Fail_Num; // 门禁解锁错误次数

extern void unlock_interface(void);  //解锁界面

extern const char *devSubTopic[];
extern const char devPubTopic[];


/******************************************************************************


硬件连接
STM32F103C8T6       RC522模块
PA4（SPI1_NSS）      	SDA
PA5（SPI1_SCK）				SCK
PA6（SPI1_MISO）			MISO
PA7（SPI1_MOSI）			MOSI
PA11（RST）						RST0
PB1（IRQ）						IRQ  未定义

呼吸灯 B6
继电器 A12 

USART1_TX   GPIOA.9

USART1_RX   GPIOA.10

功能有识别读卡，解锁，防死机
读卡用的是RC522 执行机构是舵机，陪有蜂鸣器，和继电器。继电器只在舵机动作之前接通为舵机供电，其余时间舵机为断电状态
有呼吸灯的功能，在正常运行时呼吸提示作用，错误到达6次后，呼吸灯熄灭，表示机器已锁定，等待重启即可灰度
定时器看门狗会每两个小时重启复位一次。思路是定时器定时两小时，启用看门狗但是不喂狗
重要，卡录入方法！！！
将线接好后，刷卡，观察串口1有ID项有四个数字，第一项*1+第二项*2+第三项*3+第四项*4/4得到一个数字。用SWITCH语句，判断即可。


******************************************************************************/

uint8_t Card_Type1[2];  
uint8_t Card_ID[4]; 
uint8_t Card_KEY[6] = {0xff,0xff,0xff,0xff,0xff,0xff};    //{0x11,0x11,0x11,0x11,0x11,0x11};   //密码
uint8_t Card_Data[16];
uint8_t status;
uint8_t flag;
uint8_t Run_flag=1;
uint8_t Lock_flag=0;
u16 led0pwmval=0;    
u8 dir=1;
uint8_t Err_Count=0;
extern float Angle; 

uint8_t card_number_certification(uint8_t *value);

//从flash中读取各卡信息
void Read_Card(void)
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

//查询卡号发送到小程序查看
void upload_card_number(void)
{
	//1.先判断哪个卡号有存入
	
	u8 tempcard0;
	char testbuf[256] = {""};
	char str[256] ={""};
	
	if(UI0[0] == 0xFF && UI0[1] == 0xFF && UI0[2] == 0xFF && UI0[3] == 0xFF) tempcard0 = 0;	//判断系统各卡数据区是否为空，为空才能写入新卡
	else if(UI1[0] == 0xFF && UI1[1] == 0xFF && UI1[2] == 0xFF && UI1[3] == 0xFF) tempcard0 = 1;
	else if(UI2[0] == 0xFF && UI2[1] == 0xFF && UI2[2] == 0xFF && UI2[3] == 0xFF) tempcard0 = 2;
	else if(UI3[0] == 0xFF && UI3[1] == 0xFF && UI3[2] == 0xFF && UI3[3] == 0xFF) tempcard0 = 3;
	else tempcard0 = 4;
	
	if(tempcard0 == 0)
	{
	
		DEBUG_LOG("发布数据 ---无卡-- OneNet_Publish");

		OneNet_Publish(devPubTopic,"{\"Card_Msg\":0,\"value\":0}");
		DEBUG_LOG("==================================================================================");		
		ESP8266_Clear();	
	
	}else{
		sprintf(str,"card1:%d%d%d%d\\rcard2:%d%d%d%d\\rcard3:%d%d%d%d\\rcard4:%d%d%d%d",UI0[0],UI0[1],UI0[2],UI0[3],
		UI1[0],UI1[1],UI1[2],UI1[3],UI2[0],UI2[1],UI2[2],UI2[3],UI3[0],UI3[1],UI3[2],UI3[3]);
		
		sprintf(testbuf,"{\"Card_Msg\":\"%s\",\"value\":1}",str);
		//sprintf(testbuf,"{\"Card_Msg\":\"%s\"}",str);
		DEBUG_LOG("发布数据 ---%s-- OneNet_Publish",testbuf);

		OneNet_Publish(devPubTopic,testbuf);
		DEBUG_LOG("==================================================================================");		
		ESP8266_Clear();		
		DEBUG_LOG("6666");
	}

}

void RC522_Rm_card(void)
{
	
	//1.读取有几张卡
//	u8 tempcard0;
	u8 line = 5;
	u8 Num;
	
 	
	OLED_Clear();
	OLED_ShowString(line,2,"->"); // 5 6 7 8 
	OLED_ShowString(5,4,"   Card 1 ");
	OLED_ShowString(6,4,"   Card 2 ");
	OLED_ShowString(7,4,"   Card 3 ");
	OLED_ShowString(8,4,"   Card 4 ");		
	
	while(1)
	{
		Num = Key_GetNum();
		
		if(Num == 1)
		{
				OLED_Clear();
				if(line <= 5)
				{
					line = 8;
				}else{
					line --;
				}
				OLED_Clear();
				OLED_ShowString(line,2,"->"); // 5 6 7 8 
				OLED_ShowString(5,4,"   Card 1 ");
				OLED_ShowString(6,4,"   Card 2 ");
				OLED_ShowString(7,4,"   Card 3 ");
				OLED_ShowString(8,4,"   Card 4 ");		
			
		
		}else if(Num == 2)
		{
				if(line >= 8)
				{
					line = 5;
				}else{
					line ++;
				}
		
				OLED_Clear();
				OLED_ShowString(line,2,"->"); // 5 6 7 8 
				OLED_ShowString(5,4,"   Card 1 ");
				OLED_ShowString(6,4,"   Card 2 ");
				OLED_ShowString(7,4,"   Card 3 ");
				OLED_ShowString(8,4,"   Card 4 ");		
		
		}else if(Num == 3)
		{
			if(line==5)	//删除卡1
			{
				OLED_Clear();
				OLED_ShowString(2,1,"Clear Card 1 OK ");
				delay_ms(1500);
				FLASH_Clear(FLASH_ADDR1);	
				UI0[0]=0xFF;
				UI0[1]=0xFF;
				UI0[2]=0xFF;
				UI0[3]=0xFF;
				delay_ms(1500);
				OLED_Clear();
				return;
			}

			
			if(line==6)
			{
				OLED_Clear();
				OLED_ShowString(2,1,"Clear Card 2 OK ");
				delay_ms(1500);
				FLASH_Clear(FLASH_ADDR2);
				UI1[0]=0xFF;
				UI1[1]=0xFF;
				UI1[2]=0xFF;
				UI1[3]=0xFF;
		
				OLED_Clear();
				return;
			}
		
			if(line==7)
			{
				OLED_Clear();
				OLED_ShowString(2,1,"Clear Card 3 OK ");
				delay_ms(1500);
				FLASH_Clear(FLASH_ADDR3);
				UI2[0]=0xFF;
				UI2[1]=0xFF;
				UI2[2]=0xFF;
				UI2[3]=0xFF;
				OLED_Clear();
				return;				
			}

			
			if(line==8)
			{
				OLED_Clear();
				OLED_ShowString(2,1,"Clear Card 4 OK ");
				delay_ms(1500);
				FLASH_Clear(FLASH_ADDR4);
				UI3[0]=0xFF;
				UI3[1]=0xFF;
				UI3[2]=0xFF;
				UI3[3]=0xFF;
			
				OLED_Clear();
				return;				
			}		
			

		}		
			
 	
	}



}

void RC522_Add_card(void)
{
	OLED_Clear();
	OLED_WriteCN(5,2,6,5);		
	while(1)
	{
		
		if(MI_OK==PcdRequest(0x52, Card_Type1))  //寻卡函数，如果成功返回MI_OK  打印多次卡号
		{
				OLED_Clear();
				OLED_WriteCN(6,2,5,5);	
				delay_ms(300);
				OLED_ShowChar(6,12,'.');
				delay_ms(300);
				OLED_ShowChar(6,13,'.');
				delay_ms(300);
				OLED_ShowChar(6,14,'.');
				delay_ms(300);
				OLED_ShowChar(6,15,'.');
			break;
		}
	
	}
			
			

			uint16_t cardType = (Card_Type1[0]<<8)|Card_Type1[1];//读不同卡的类型
			DEBUG_LOG("卡类型：(0x%04X)\r\n",cardType);  //"Card Type(0x%04X):"
			
			switch(cardType)
				{
					case 0x4400:
							DEBUG_LOG("Mifare UltraLight\r\n");
							break;
					case 0x0400:
							DEBUG_LOG("Mifare One(S50)\r\n");
							break;
					case 0x0200:
							DEBUG_LOG("Mifare One(S70)\r\n");
							break;
					case 0x0800:
							DEBUG_LOG("Mifare Pro(X)\r\n");
							break;
					case 0x4403:
							DEBUG_LOG("Mifare DESFire\r\n");
							break;
					default:
							DEBUG_LOG("Unknown Card\r\n");
							break;
			}
			status = PcdAnticoll(Card_ID);//防冲撞 如果成功返回MI_OK
			if(status != MI_OK)
				{
					DEBUG_LOG("Anticoll Error\r\n");
				}else
				{
					DEBUG_LOG("Serial Number:%d %d %d %d\r\n",Card_ID[0],Card_ID[1],Card_ID[2],Card_ID[3]);
					
					DEBUG_LOG("card0 Number:%d %d %d %d\r\n",UI0[0],UI0[1],UI0[2],UI0[3]);
					DEBUG_LOG("card0 Number:%d %d %d %d\r\n",UI1[0],UI1[1],UI1[2],UI1[3]);
					DEBUG_LOG("card0 Number:%d %d %d %d\r\n",UI2[0],UI2[1],UI2[2],UI2[3]);
					DEBUG_LOG("card0 Number:%d %d %d %d\r\n",UI3[0],UI3[1],UI3[2],UI3[3]);

					if(UI0[0] == 0xFF && UI0[1] == 0xFF && UI0[2] == 0xFF && UI0[3] == 0xFF) tempcard = 0;	//判断系统各卡数据区是否为空，为空才能写入新卡
					else if(UI1[0] == 0xFF && UI1[1] == 0xFF && UI1[2] == 0xFF && UI1[3] == 0xFF) tempcard = 1;
					else if(UI2[0] == 0xFF && UI2[1] == 0xFF && UI2[2] == 0xFF && UI2[3] == 0xFF) tempcard = 2;
					else if(UI3[0] == 0xFF && UI3[1] == 0xFF && UI3[2] == 0xFF && UI3[3] == 0xFF) tempcard = 3;
					else tempcard = 4;
					
					if(Card_ID[0]==UI0[0] && Card_ID[1]==UI0[1] && Card_ID[2]==UI0[2] && Card_ID[3]==UI0[3])	//判断新卡是否已经录入
					{
						tempcard = 5;
					}
					if(Card_ID[0]==UI1[0] && Card_ID[1]==UI1[1] && Card_ID[2]==UI1[2] && Card_ID[3]==UI1[3])
					{
						tempcard = 5;
					}
					if(Card_ID[0]==UI2[0] && Card_ID[1]==UI2[1] && Card_ID[2]==UI2[2] && Card_ID[3]==UI2[3])
					{
						tempcard = 5;
					}
					if(Card_ID[0]==UI3[0] && Card_ID[1]==UI3[1] && Card_ID[2]==UI3[2] && Card_ID[3]==UI3[3])
					{
						tempcard = 5;
					}
					
					DEBUG_LOG("----%d",tempcard);
					OLED_Clear();
					
					switch(tempcard)
					{
						
						case 0:
						{
							UI0[0] = Card_ID[0];	//将新卡数据写入UI0[]数组
							UI0[1] = Card_ID[1];
							UI0[2] = Card_ID[2];
							UI0[3] = Card_ID[3];
							FLASH_W(FLASH_ADDR1,UI0[0],UI0[1],UI0[2],UI0[3]);	//将新卡数据存入flash
							OLED_ShowString(1,1,"  Add Card 1 OK ");	//写卡成功，蜂鸣器响一声
							DEBUG_LOG("Add Card 1 OK");
							delay_ms(1000);
							
							//WaitCardOff();	//等待卡片移走
						}break;
						case 1:
						{
							UI1[0] = Card_ID[0];
							UI1[1] = Card_ID[1];
							UI1[2] = Card_ID[2];
							UI1[3] = Card_ID[3];
							FLASH_W(FLASH_ADDR2,UI1[0],UI1[1],UI1[2],UI1[3]);
							OLED_ShowString(1,1,"  Add Card 2 OK ");
							DEBUG_LOG("Add Card 2 OK");
							delay_ms(1000);
							//WaitCardOff();
						}break;
						case 2:
						{
							UI2[0] = Card_ID[0];
							UI2[1] = Card_ID[1];
							UI2[2] = Card_ID[2];
							UI2[3] = Card_ID[3];
							FLASH_W(FLASH_ADDR3,UI2[0],UI2[1],UI2[2],UI2[3]);
							OLED_ShowString(1,1,"  Add Card 3 OK ");
							DEBUG_LOG("Add Card 3 OK");
							delay_ms(1000);
							//WaitCardOff();
						}break;
						case 3:
						{
							UI3[0] = Card_ID[0];
							UI3[1] = Card_ID[1];
							UI3[2] = Card_ID[2];
							UI3[3] = Card_ID[3];
							FLASH_W(FLASH_ADDR4,UI3[0],UI3[1],UI3[2],UI3[3]);
							OLED_ShowString(1,1,"  Add Card 4 OK ");
							DEBUG_LOG("Add Card 4 OK");
							delay_ms(1000);
							//WaitCardOff();
						}break;
						case 4:
						{	
							OLED_ShowString(1,1,"   NO memory    ");	//若4个存卡数组均已存入卡片则显示无数据空间，蜂鸣器发出警报
							DEBUG_LOG("NO memory");
							delay_ms(1000);
							//WaitCardOff();
						}
						case 5:
						{	
							OLED_ShowString(1,1," Existing Card! ");	//若新卡已录入，蜂鸣器发出警报
							DEBUG_LOG(" Existing Card!");
							delay_ms(1000);

						

						}break;
						default:break;
					 }
					
					


				}
			
			status = PcdSelect(Card_ID);  //选卡 如果成功返回MI_OK
			if(status != MI_OK){
					DEBUG_LOG("Select Card Error\r\n");
			}
			else{
				DEBUG_LOG("Select Card OK\r\n");
				
			}
			
			
			status = PcdHalt();  //卡片进入休眠状态
			if(status != MI_OK){
					DEBUG_LOG("PcdHalt Error\r\n");			
			}
			else
			{
					DEBUG_LOG("PcdHalt OK\r\n");	
			}

			


		DEBUG_LOG("cesss444");

	//delay_ms(500);
	//PcdAntennaOff();  //关闭天线

}




void RC522_Get_card(void)
{
								char buf[25] = {""};
//	while(Run_flag)
//	{
	
		if(MI_OK==PcdRequest(0x52, Card_Type1))  //寻卡函数，如果成功返回MI_OK  打印多次卡号
		{
			
				OLED_Clear();
				OLED_WriteCN(6,2,5,5);	
				delay_ms(500);
				OLED_ShowChar(6,12,'.');
				delay_ms(500);
				OLED_ShowChar(6,13,'.');
				delay_ms(500);
				OLED_ShowChar(6,14,'.');
				delay_ms(500);
				OLED_ShowChar(6,15,'.');
			

			uint16_t cardType = (Card_Type1[0]<<8)|Card_Type1[1];//读不同卡的类型
			DEBUG_LOG("卡类型：(0x%04X)\r\n",cardType);  //"Card Type(0x%04X):"
			
			switch(cardType)
				{
					case 0x4400:
							DEBUG_LOG("Mifare UltraLight\r\n");
							break;
					case 0x0400:
							DEBUG_LOG("Mifare One(S50)\r\n");
							break;
					case 0x0200:
							DEBUG_LOG("Mifare One(S70)\r\n");
							break;
					case 0x0800:
							DEBUG_LOG("Mifare Pro(X)\r\n");
							break;
					case 0x4403:
							DEBUG_LOG("Mifare DESFire\r\n");
							break;
					default:
							DEBUG_LOG("Unknown Card\r\n");
							break;
			}
			status = PcdAnticoll(Card_ID);//防冲撞 如果成功返回MI_OK
			if(status != MI_OK)
				{
					DEBUG_LOG("Anticoll Error\r\n");
				}else
				{
					DEBUG_LOG("Serial Number:%d %d %d %d\r\n",Card_ID[0],Card_ID[1],Card_ID[2],Card_ID[3]);
						flag=card_number_certification(Card_ID);
					
					
						DEBUG_LOG("flag=%d \r\n",flag);
						if(flag != 0)//解锁
						{
							
							unlock_interface();
										
						
						}else{
						
							Fail_Num++; // 门禁解锁错误次数
		
							//判断标志 才进行错误次数增加减少
							sprintf(buf,"fail num: %d",Fail_Num);
							OLED_ShowString(2,1,buf);
								delay_ms(500);
								OLED_Clear();
						}

				}
			
			status = PcdSelect(Card_ID);  //选卡 如果成功返回MI_OK
			if(status != MI_OK){
					DEBUG_LOG("Select Card Error\r\n");
			}
			else{
				DEBUG_LOG("Select Card OK\r\n");
				
			}
			
			
			status = PcdHalt();  //卡片进入休眠状态
			if(status != MI_OK){
					DEBUG_LOG("PcdHalt Error\r\n");			
			}
			else
			{
					DEBUG_LOG("PcdHalt OK\r\n");	
			}
			
		}

	//}

	//delay_ms(500);
	//PcdAntennaOff();  //关闭天线

}



uint8_t card_number_certification(uint8_t *value)
{

	DEBUG_LOG("%d %d %d %d",UI0[0],UI0[1],UI0[2],UI0[3]);
	int flag=0;	
	if(value[0]==UI0[0]&&value[1]==UI0[1]&&value[2]==UI0[2]&&value[3]==UI0[3])
	{
		flag=1;
	}
	else if(value[0]==UI1[0]&&value[1]==UI1[1]&&value[2]==UI1[2]&&value[3]==UI1[3])
	{
		flag=2;
	}
	else if(value[0]==UI2[0]&&value[1]==UI2[1]&&value[2]==UI2[2]&&value[3]==UI2[3])
	{
		flag=3;
	}
	else if(value[0]==UI3[0]&&value[1]==UI3[1]&&value[2]==UI3[2]&&value[3]==UI3[3])
	{
		flag=4;
	}
	else flag = 0;

	
	return flag;
}

void RC522_IO_Init(void) 
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);   //开启AFIO时钟
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE); //关闭JTAG因为要使用PB3和4
	
	//GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStruct);	
	
	SPI_Configuration(SPI1); 
}


//#define MAXRLEN 18
                              
/////////////////////////////////////////////////////////////////////
//功    能：寻卡
//参数说明: req_code[IN]:寻卡方式
//                0x52 = 寻感应区内所有符合14443A标准的卡
//                0x26 = 寻未进入休眠状态的卡
//          pTagType[OUT]：卡片类型代码
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdRequest(unsigned char req_code,unsigned char *pTagType)
{
   char status;  
   unsigned int  unLen;
   unsigned char ucComMF522Buf[MAXRLEN]; 

   ClearBitMask(Status2Reg,0x08); //清RC522寄存位
   WriteRawRC(BitFramingReg,0x07); //写RC623寄存器
   SetBitMask(TxControlReg,0x03); //置RC522寄存位
//   
   ucComMF522Buf[0] = req_code;

   status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);
//   UART_send_byte(status);
   if ((status == MI_OK) && (unLen == 0x10))
   {    
       *pTagType     = ucComMF522Buf[0];
       *(pTagType+1) = ucComMF522Buf[1];
   }
   else
   {   status = MI_ERR;   }
   
   return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：防冲撞
//参数说明: pSnr[OUT]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////  
char PcdAnticoll(unsigned char *pSnr)
{
    char status;
    unsigned char i,snr_check=0;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 
    

    ClearBitMask(Status2Reg,0x08);
    WriteRawRC(BitFramingReg,0x00);
    ClearBitMask(CollReg,0x80);
 
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x20;

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);

    if (status == MI_OK)
    {
    	 for (i=0; i<4; i++)
         {   
             *(pSnr+i)  = ucComMF522Buf[i];
             snr_check ^= ucComMF522Buf[i];
         }
         if (snr_check != ucComMF522Buf[i])
         {   status = MI_ERR;    }
    }
    
    SetBitMask(CollReg,0x80);
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：选定卡片
//参数说明: pSnr[IN]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdSelect(unsigned char *pSnr)
{
    char status;
    unsigned char i;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x70;
    ucComMF522Buf[6] = 0;
    for (i=0; i<4; i++)
    {
    	ucComMF522Buf[i+2] = *(pSnr+i);
    	ucComMF522Buf[6]  ^= *(pSnr+i);
    }
    CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);
  
    ClearBitMask(Status2Reg,0x08);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);
    
    if ((status == MI_OK) && (unLen == 0x18))
    {   status = MI_OK;  }
    else
    {   status = MI_ERR;    }

    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：验证卡片密码
//参数说明: auth_mode[IN]: 密码验证模式
//                 0x60 = 验证A密钥
//                 0x61 = 验证B密钥 
//          addr[IN]：块地址
//          pKey[IN]：密码
//          pSnr[IN]：卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////                   
char PcdAuthState(unsigned char auth_mode,unsigned char addr,unsigned char *pKey,unsigned char *pSnr)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = auth_mode;
    ucComMF522Buf[1] = addr;
    for (i=0; i<6; i++)
    {    ucComMF522Buf[i+2] = *(pKey+i);   }
    for (i=0; i<6; i++)
    {    ucComMF522Buf[i+8] = *(pSnr+i);   }
    
    status = PcdComMF522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
    if ((status != MI_OK) || (!(ReadRawRC(Status2Reg) & 0x08)))
    {   status = MI_ERR;   }
    
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：读取M1卡一块数据
//参数说明: addr[IN]：块地址
//          p [OUT]：读出的数据，16字节
//返    回: 成功返回MI_OK
///////////////////////////////////////////////////////////////////// 
char PcdRead(unsigned char addr,unsigned char *pData)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_READ;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
   
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    if ((status == MI_OK) && (unLen == 0x90))
    {
        for (i=0; i<16; i++)
        {    *(pData+i) = ucComMF522Buf[i];   }
    }
    else
    {   status = MI_ERR;   }
    
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：写数据到M1卡一块
//参数说明: addr[IN]：块地址
//          p [IN]：写入的数据，16字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////                 
char PcdWrite(unsigned char addr,unsigned char *pData)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_WRITE;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
        
    if (status == MI_OK)
    {
        for (i=0; i<16; i++)
        {    ucComMF522Buf[i] = *(pData+i);   }
        CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);

        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&unLen);
        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   status = MI_ERR;   }
    }
    
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：扣款和充值
//参数说明: dd_mode[IN]：命令字
//               0xC0 = 扣款
//               0xC1 = 充值
//          addr[IN]：钱包地址
//          pValue[IN]：4字节增(减)值，低位在前
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////                
char PcdValue(unsigned char dd_mode,unsigned char addr,unsigned char *pValue)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = dd_mode;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
        
    if (status == MI_OK)
    {
        for (i=0; i<16; i++)
        {    ucComMF522Buf[i] = *(pValue+i);   }
        CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
        unLen = 0;
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
        if (status != MI_ERR)
        {    status = MI_OK;    }
    }
    
    if (status == MI_OK)
    {
        ucComMF522Buf[0] = PICC_TRANSFER;
        ucComMF522Buf[1] = addr;
        CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]); 
   
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   status = MI_ERR;   }
    }
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：备份钱包
//参数说明: sourceaddr[IN]：源地址
//          goaladdr[IN]：目标地址
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdBakValue(unsigned char sourceaddr, unsigned char goaladdr)
{
    char status;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_RESTORE;
    ucComMF522Buf[1] = sourceaddr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
    
    if (status == MI_OK)
    {
        ucComMF522Buf[0] = 0;
        ucComMF522Buf[1] = 0;
        ucComMF522Buf[2] = 0;
        ucComMF522Buf[3] = 0;
        CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
 
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
        if (status != MI_ERR)
        {    status = MI_OK;    }
    }
    
    if (status != MI_OK)
    {    return MI_ERR;   }
    
    ucComMF522Buf[0] = PICC_TRANSFER;
    ucComMF522Buf[1] = goaladdr;

    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }

    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：命令卡片进入休眠状态
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdHalt(void)
{
    //char status;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_HALT;
    ucComMF522Buf[1] = 0;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    //status = 
	  PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    return MI_OK;
}

/////////////////////////////////////////////////////////////////////
//用MF522计算CRC16函数
/////////////////////////////////////////////////////////////////////
void CalulateCRC(unsigned char *pIndata,unsigned char len,unsigned char *pOutData)
{
    unsigned char i,n;
    ClearBitMask(DivIrqReg,0x04);
    WriteRawRC(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);
    for (i=0; i<len; i++)
    {   WriteRawRC(FIFODataReg, *(pIndata+i));   }
    WriteRawRC(CommandReg, PCD_CALCCRC);
    i = 0xFF;
    do 
    {
        n = ReadRawRC(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04));
    pOutData[0] = ReadRawRC(CRCResultRegL);
    pOutData[1] = ReadRawRC(CRCResultRegM);
}

/////////////////////////////////////////////////////////////////////
//功    能：复位RC522
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdReset(void)
{
    RC522_RESET_SET();     //RST522_1;
    delay_us(10);  //_NOP();
    RC522_RESET_RESET();   //RST522_0;
    delay_ms(60);  //_NOP();_NOP();
    RC522_RESET_SET();     //RST522_1;RST522_1;
    delay_us(500);  //_NOP();_NOP();
    WriteRawRC(CommandReg,PCD_RESETPHASE);
    delay_ms(2);  //_NOP();_NOP();
  
  WriteRawRC(ModeReg,0x3D);            //?Mifare???,CRC???0x6363
  WriteRawRC(TReloadRegL,30);         //?30?????           
  WriteRawRC(TReloadRegH,0);          
  WriteRawRC(TModeReg,0x8D);
  WriteRawRC(TPrescalerReg,0x3E);
  WriteRawRC(TxAutoReg,0x40);
  
  ClearBitMask(TestPinEnReg, 0x80);//off MX and DTRQ out
  WriteRawRC(TxAutoReg,0x40);
   
  return MI_OK;
}

/////////////////////////////////////////////////////////////////////
//功    能：读RC632寄存器
//参数说明：Address[IN]:寄存器地址
//返    回：读出的值
/////////////////////////////////////////////////////////////////////
unsigned char ReadRawRC(unsigned char Address)
{
	unsigned char ucAddr;
	unsigned char ucResult=0;
	ucAddr = ((Address<<1)&0x7E)|0x80;
	delay_ms(1);
	RC522_ENABLE;
	SPI_WriteNBytes(SPI1,&ucAddr,1);  //向总线写多个数据
	SPI_ReadNBytes(SPI1,&ucResult,1);  //向总线读多个数据
	RC522_DISABLE;
	return ucResult;
}

/////////////////////////////////////////////////////////////////////
//功    能：写RC632寄存器
//参数说明：Address[IN]:寄存器地址
//          value[IN]:写入的值
/////////////////////////////////////////////////////////////////////
void WriteRawRC(unsigned char Address, unsigned char value)
{  
	unsigned char ucAddr;
	uint8_t write_buffer[2]={0};
	ucAddr = ((Address<<1)&0x7E);
	write_buffer[0] = ucAddr;
	write_buffer[1] = value;
	delay_ms(1);
	RC522_ENABLE;
	SPI_WriteNBytes(SPI1,write_buffer,2);
	RC522_DISABLE;
}

/////////////////////////////////////////////////////////////////////
//功    能：置RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:置位值
/////////////////////////////////////////////////////////////////////
void SetBitMask(unsigned char reg,unsigned char mask)  
{
    char tmp = 0x0;
    tmp = ReadRawRC(reg);  //读RC632寄存器
    WriteRawRC(reg,tmp | mask);  // set bit mask
}

/////////////////////////////////////////////////////////////////////
//功    能：清RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:清位值
/////////////////////////////////////////////////////////////////////
void ClearBitMask(unsigned char reg,unsigned char mask)  
{
    char tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg, tmp & ~mask);  // clear bit mask
} 

/////////////////////////////////////////////////////////////////////
//功    能：通过RC522和ISO14443卡通讯
//参数说明：Command[IN]:RC522命令字
//          pIn [IN]:通过RC522发送到卡片的数据
//          InLenByte[IN]:发送数据的字节长度
//          pOut [OUT]:接收到的卡片返回数据
//          *pOutLenBit[OUT]:返回数据的位长度
/////////////////////////////////////////////////////////////////////
char PcdComMF522(unsigned char Command, 
                 unsigned char *pInData, 
                 unsigned char InLenByte,
                 unsigned char *pOutData, 
                 unsigned int  *pOutLenBit)
{
    char status = MI_ERR;
    unsigned char irqEn   = 0x00;
    unsigned char waitFor = 0x00;
    unsigned char lastBits;
    unsigned char n;
    unsigned int i;
    switch (Command)
    {
       case PCD_AUTHENT:
          irqEn   = 0x12;
          waitFor = 0x10;
          break;
       case PCD_TRANSCEIVE:
          irqEn   = 0x77;
          waitFor = 0x30;
          break;
       default:
         break;
    }
   
    WriteRawRC(ComIEnReg,irqEn|0x80);
    ClearBitMask(ComIrqReg,0x80);
    WriteRawRC(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);
    
    for (i=0; i<InLenByte; i++)
    {   WriteRawRC(FIFODataReg, pInData[i]);    }
    WriteRawRC(CommandReg, Command);
   
    
    if (Command == PCD_TRANSCEIVE)
    {    SetBitMask(BitFramingReg,0x80);  }
    
    i = 800 ; //600;//????????,??M1???????25ms
    do 
    {
         n = ReadRawRC(ComIrqReg);
         i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitFor));
    ClearBitMask(BitFramingReg,0x80);
	      
    if (i!=0)
    {    
         if(!(ReadRawRC(ErrorReg)&0x1B))
         {
             status = MI_OK;
             if (n & irqEn & 0x01)
             {   status = MI_NOTAGERR;   }
             if (Command == PCD_TRANSCEIVE)
             {
               	n = ReadRawRC(FIFOLevelReg);
              	lastBits = ReadRawRC(ControlReg) & 0x07;
                if (lastBits)
                {   *pOutLenBit = (n-1)*8 + lastBits;   }
                else
                {   *pOutLenBit = n*8;   }
                if (n == 0)
                {   n = 1;    }
                if (n > MAXRLEN)
                {   n = MAXRLEN;   }
                for (i=0; i<n; i++)
                {   pOutData[i] = ReadRawRC(FIFODataReg);    }
            }
         }
         else
         {   status = MI_ERR;   }
        
   }
   

   SetBitMask(ControlReg,0x80);           // stop timer now
   WriteRawRC(CommandReg,PCD_IDLE); 
   return status;
}


/////////////////////////////////////////////////////////////////////
//开启天线  
//每次启动或关闭天险发射之间应至少有1ms的间隔
/////////////////////////////////////////////////////////////////////
void PcdAntennaOn(void)
{
    unsigned char i;
    i = ReadRawRC(TxControlReg);
    if (!(i & 0x03))
    {
        SetBitMask(TxControlReg, 0x03);
    }
}


/////////////////////////////////////////////////////////////////////
//关闭天线
/////////////////////////////////////////////////////////////////////
void PcdAntennaOff(void)
{
    ClearBitMask(TxControlReg, 0x03);
}

void RC522_Config(unsigned char Card_Type)
{
	   ClearBitMask(Status2Reg,0x08);
     WriteRawRC(ModeReg,0x3D);//3F
     WriteRawRC(RxSelReg,0x86);//84
     WriteRawRC(RFCfgReg,0x7F);   //4F
   	 WriteRawRC(TReloadRegL,30);//tmoLength);// TReloadVal = 'h6a =tmoLength(dec) 
	   WriteRawRC(TReloadRegH,0);
     WriteRawRC(TModeReg,0x8D);
	   WriteRawRC(TPrescalerReg,0x3E);
//	   WriteRawRC(TxAutoReg,0x40);//???
	   delay_ms(5);//delay_10ms(1);
	
	
     PcdAntennaOn();
}





//等待卡离开
void WaitCardOff(void)
{
	char          status;
  unsigned char	TagType[2];
 
	while(1)
	{
		status = PcdRequest(REQ_ALL, TagType);
		if(status)
		{
			status = PcdRequest(REQ_ALL, TagType);
			if(status)
			{
				status = PcdRequest(REQ_ALL, TagType);
				if(status)
				{
					return;
				}
			}
		}
		delay_ms(10);
	}
}
 







