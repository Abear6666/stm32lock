/************************************************************************
							OLED����
�������ǻ���IICЭ���OLED����������һ����ά����u8 OLED_GRAM[128][8]
��������OLED����ĸ�����������Ƚ���ʾ�����ݴ���������У�Ȼ��һ��
����OLED��ʾ��

														�����ˣ���˫
														ʱ�䣺2017-1-7
************************************************************************/

#ifndef _OLED_H
#define _OLED_H
#include "sys.h"

/*OLED��ģʽ����:
	ͨ��Э��	BS0		BS1		BS2		OLED_mode
		IIC��	 0		 1		 0		 0
		4��SPI�� 0		 0		 0		 1
		6800��	 0		 0		 1
		8080��	 0     	 1		 1	
*/

#define OLED_mood 0

/***************************�˿ڶ�����*****************/

//IIC�˿ڶ��� mood == 0
#define OLED_SCL PAout(3)
#define OLED_SDA PAout(2)

//4��SPI�˿ڶ��� mood = 1
#define OLED_SPI_SDA PCout(6)	//������
#define OLED_SPI_RES PCout(8)	//��λ
#define OLED_SPI_DC PCout(11)	//DC=0,�����DC=1,������
#define OLED_SPI_VCC PDout(6)
#define OLED_SPI_SCLK PDout(7)	//ʱ����
#define OLED_SPI_CS PEout(5)	//Ƭѡ���͵�ƽ��Ч
/******************************************************/

/***************************����������*****************/

void OLED_Write_Cmd(u8 cmd);					//�������ܣ�д����
void OLED_Write_Data(u8 cmd);					//�������ܣ�д����
void OLED_WriteByte(u8 data);					//�������ܣ�SDA����һ���ֽ�
u8 OLED_Ask(void);								//�������ܣ�IICӦ��

/**********************�û�������*********************/
void OLED_Init(void);							//�������ܣ���ʼ��GPIO��OLED
void OLED_On(void);								//�������ܣ���OLED������OLED�������л���
void OLED_Off(void);							//�������ܣ��ر�OLED��ʾ
void OLED_Clear(void);							//�������ܣ�����
void OLED_Fill(u8 x1, u8 y1, u8 x2, u8 y2, u8 mode);//�������ܣ��ڶԽ�(x1,y1)��(x2,y2)��������ȫ������ȫ��

void OLED_Show_Font16X16(u8 x, u8 y, u8 str[]);//�������ܣ���ʾ��СΪ��16����16�ĺ���
void OLED_Show_Font12X16(u8 x, u8 y, u8 str[]);//�������ܣ���ʾ��СΪ��12����16�ĺ���
void OLED_Show_Font12X12(u8 x, u8 y, u8 str[]);//�������ܣ���ʾ��СΪ��12����12�ĺ���
void OLED_Show_Picture(u8 x, u8 y, u8 picture);//�������ܣ���ʾͼƬ
void OLED_Show_Str(u8 x, u8 y, const u8 *p, u8 TextSize);//�������ܣ���ʾ�ַ���
void OLED_Show_UnIntNum(u8 x, u8 y, u32 num, u8 figure, u8 size);
void OLED_Show_IntNum(u8 x, u8 y, int num,u8 figure,u8 size);//�������ܣ���ʾint����
void OLED_Show_UnFloatNum(u8 x, u8 y, double num, int precise, u8 size);
void OLED_Show_FloatNum(u8 x, u8 y, double num,int precise, u8 size);//�������ܣ���ʾfloat����
void OLED_Show_Char(u8 x, u8 y, u8 ch, u8 size);//�������ܣ���ʾ�ַ�
void OLED_SetPos(u8 x, u8 y);
void OLED_Show_Many_Chinese12X12(unsigned char x, unsigned char y, unsigned char *chr);
void OLED_Show_Many_Chinese12X12_1(unsigned char x, unsigned char y, unsigned char *chr);

/********************************************************/
#endif

