/************************************************************************
							OLED驱动
本驱动是基于IIC协议的OLED驱动，定义一个二维数组u8 OLED_GRAM[128][8]
来存整个OLED点阵的各个点的亮灭。先将显示的数据存入该数组中，然后一起
传到OLED显示。

														制作人：胡双
														时间：2017-1-7
************************************************************************/

#ifndef _OLED_H
#define _OLED_H
#include "sys.h"

/*OLED的模式设置:
	通信协议	BS0		BS1		BS2		OLED_mode
		IIC：	 0		 1		 0		 0
		4线SPI： 0		 0		 0		 1
		6800：	 0		 0		 1
		8080：	 0     	 1		 1	
*/

#define OLED_mood 0

/***************************端口定义区*****************/

//IIC端口定义 mood == 0
#define OLED_SCL PAout(3)
#define OLED_SDA PAout(2)

//4线SPI端口定义 mood = 1
#define OLED_SPI_SDA PCout(6)	//数据线
#define OLED_SPI_RES PCout(8)	//复位
#define OLED_SPI_DC PCout(11)	//DC=0,传命令，DC=1,传数据
#define OLED_SPI_VCC PDout(6)
#define OLED_SPI_SCLK PDout(7)	//时钟线
#define OLED_SPI_CS PEout(5)	//片选，低电平有效
/******************************************************/

/***************************函数声明区*****************/

void OLED_Write_Cmd(u8 cmd);					//函数功能：写命令
void OLED_Write_Data(u8 cmd);					//函数功能：写数据
void OLED_WriteByte(u8 data);					//函数功能：SDA传输一个字节
u8 OLED_Ask(void);								//函数功能：IIC应答

/**********************用户函数区*********************/
void OLED_Init(void);							//函数功能：初始化GPIO、OLED
void OLED_On(void);								//函数功能：打开OLED，即将OLED从休眠中唤醒
void OLED_Off(void);							//函数功能：关闭OLED显示
void OLED_Clear(void);							//函数功能：清屏
void OLED_Fill(u8 x1, u8 y1, u8 x2, u8 y2, u8 mode);//函数功能：在对角(x1,y1)、(x2,y2)的区域内全亮或者全灭

void OLED_Show_Font16X16(u8 x, u8 y, u8 str[]);//函数功能：显示大小为宽16，高16的汉字
void OLED_Show_Font12X16(u8 x, u8 y, u8 str[]);//函数功能：显示大小为宽12，高16的汉字
void OLED_Show_Font12X12(u8 x, u8 y, u8 str[]);//函数功能：显示大小为宽12，高12的汉字
void OLED_Show_Picture(u8 x, u8 y, u8 picture);//函数功能：显示图片
void OLED_Show_Str(u8 x, u8 y, const u8 *p, u8 TextSize);//函数功能：显示字符串
void OLED_Show_UnIntNum(u8 x, u8 y, u32 num, u8 figure, u8 size);
void OLED_Show_IntNum(u8 x, u8 y, int num,u8 figure,u8 size);//函数功能：显示int数字
void OLED_Show_UnFloatNum(u8 x, u8 y, double num, int precise, u8 size);
void OLED_Show_FloatNum(u8 x, u8 y, double num,int precise, u8 size);//函数功能：显示float数字
void OLED_Show_Char(u8 x, u8 y, u8 ch, u8 size);//函数功能：显示字符
void OLED_SetPos(u8 x, u8 y);
void OLED_Show_Many_Chinese12X12(unsigned char x, unsigned char y, unsigned char *chr);
void OLED_Show_Many_Chinese12X12_1(unsigned char x, unsigned char y, unsigned char *chr);

/********************************************************/
#endif

