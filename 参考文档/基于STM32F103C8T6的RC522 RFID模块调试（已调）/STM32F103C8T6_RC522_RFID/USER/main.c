#include "usart.h"
#include "stm32f10x_spi.h"
#include "RC522.h"
#include "delay.h"
#include "string.h"
#include "spi_driver.h"
#include "stdio.h"

/******************************************************************************
大海电子专营各种电子模块，欢迎光临

硬件连接
STM32F103C8T6       RC522模块
PA4（SPI1_NSS）      	SDA
PA5（SPI1_SCK）				SCK
PA6（SPI1_MISO）			MISO
PA7（SPI1_MOSI）			MOSI
PA11（RST）						RST
PB1（IRQ）						IRQ  未定义


USART1_TX   GPIOA.9
USART1_RX   GPIOA.10


备注：以下代码非原创，只是经过修改适配成STM32F103C8T6
若侵权，请告知。
此代码是融合了战舰开发板程序+网上RFID RC522(Author: wdluo)模块进行适配的。
******************************************************************************/

uint8_t Card_Type1[2];  
uint8_t Card_ID[4]; 
uint8_t Card_KEY[6] = {0xff,0xff,0xff,0xff,0xff,0xff};    //{0x11,0x11,0x11,0x11,0x11,0x11};   //密码
uint8_t Card_Data[16];
uint8_t status;

int main(void)
{
	uint8_t i;
	Card_Type1[0]=0x04;
	Card_Type1[1]=0x00;
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 //串口初始化为115200	
 	
 	printf("\r\n***************************** 大海电子串口测试 *****************************\r\n");
 
	delay_init();
	RC522_IO_Init();
	PcdReset();  //复位RC522
	PcdAntennaOff();  //关闭天线
	delay_ms(100);
	PcdAntennaOn();  //开启天线
	
	printf("\r\n***************************** 大海电子串口测试 *****************************\r\n");
	while(1)
	{
		delay_ms(10);
		if(MI_OK==PcdRequest(0x52, Card_Type1))  //寻卡函数，如果成功返回MI_OK  打印多次卡号
//		if(MI_OK==PcdRequest(0x26, Card_Type1))  //寻卡函数，如果成功返回MI_OK   打印1次卡号
		{
			uint16_t cardType = (Card_Type1[0]<<8)|Card_Type1[1];//读不同卡的类型
			printf("卡类型：(0x%04X)\r\n",cardType);  //"Card Type(0x%04X):"
			switch(cardType){
			case 0x4400:
					printf("Mifare UltraLight\r\n");
					break;
			case 0x0400:
					printf("Mifare One(S50)\r\n");
					break;
			case 0x0200:
					printf("Mifare One(S70)\r\n");
					break;
			case 0x0800:
					printf("Mifare Pro(X)\r\n");
					break;
			case 0x4403:
					printf("Mifare DESFire\r\n");
					break;
			default:
					printf("Unknown Card\r\n");
					break;
			}
			
			status = PcdAnticoll(Card_ID);//防冲撞 如果成功返回MI_OK
			if(status != MI_OK){
					printf("Anticoll Error\r\n");
			}else{
					printf("Serial Number:%02X%02X%02X%02X\r\n",Card_ID[0],Card_ID[1],Card_ID[2],Card_ID[3]);
			}
			
			status = PcdSelect(Card_ID);  //选卡 如果成功返回MI_OK
			if(status != MI_OK){
					printf("Select Card Error\r\n");
			}
			else
				printf("Select Card OK\r\n");
			
//			status = PcdAuthState(PICC_AUTHENT1A,5,Card_KEY,Card_ID);//验证卡密码 如果成功返回MI_OK
//			if(status != MI_OK){
//					printf("Auth State Error\r\n");
//					continue;
//			}
//			
//			memset(Card_ID,1,4);  //函数用于为变量开辟空间，或将变量都赋值为一个值
//			memset(Card_Data,1,16);
//			Card_Data[0]=0xaa;
//			status = PcdWrite(5,Card_Data);                   //写入0XAA,0X01,0X01……选择块地址写入数据，成功则返回MI_OK
//			if(status != MI_OK){
//					printf("Card Write Error\r\n");
//					continue;
//			}
//			memset(Card_Data,0,16);  //清零
//			delay_us(8);
//			
//			
//			status = PcdRead(5,Card_Data);                    //再一次把它读取出来16字节的卡片数据，选择块地址读出数据，成功则返回MI_OK
//			if(status != MI_OK){
//					printf("Card Read Error\r\n");
//					continue;
//			}else{
//				for(i=0;i<16;i++){
//					printf("%02X ",Card_Data[i]);
//				}
//				printf("\r\n");
//			}
//			
//			memset(Card_Data,2,16);
//			Card_Data[0]=0xbb;
//			delay_us(8);
//			status = PcdWrite(5,Card_Data);                   //写入0Xbb,0X02,0X02……
//			if(status != MI_OK){
//					printf("Card Write Error\r\n");
//					continue;
//			}
//			delay_us(8);
//			
//			status = PcdRead(5,Card_Data);                    //再一次把它读取出来16字节的卡片数据
//			if(status != MI_OK){
//					printf("Card Read Error\r\n");
//					continue;
//			}else{
//				for(i=0;i<16;i++){
//					printf("%02X ",Card_Data[i]);
//				}
//				printf("\r\n");
//			}
			
			//memset(Card_Data,0,16);
		//	PcdHalt();  //卡片进入休眠状态
			
			status = PcdHalt();  //卡片进入休眠状态
			if(status != MI_OK){
					printf("PcdHalt Error\r\n");			
			}
			else
			{
					printf("PcdHalt OK\r\n");	
			}
		}
	}
}


