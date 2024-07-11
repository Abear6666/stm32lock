#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//������������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////  
								    
//������ʼ������
void KEY_Init(void) //IO��ʼ��
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOE,ENABLE);//ʹ��PORTA,PORTB,PORTCʱ��

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12|GPIO_Pin_13 |GPIO_Pin_5 |GPIO_Pin_3 ;//KEY0-KEY2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB12,13
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3;//KEY0-KEY2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //���ó���������
 	GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��GPIOB12,13

}
//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//0��û���κΰ�������
//1��KEY0����
//2��KEY1����
//3��KEY2���� 
//4��KEY3���� WK_UP
//ע��˺�������Ӧ���ȼ�,KEY0>KEY1>KEY2>KEY3!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//�������ɿ���־
	if(mode)key_up=1;  //֧������		  
	if(key_up&&(KEY0==0||KEY1==0||KEY2==0||KEY3==0))
	{
		delay_ms(10);//ȥ���� 
		key_up=0;
		if(KEY0==0)return KEY0_PRES;
		else if(KEY1==0)return KEY1_PRES;
		else if(KEY2==0)return KEY2_PRES;
		else if(KEY3==0)return KEY3_PRES;
	}else if(KEY0==1&&KEY1==1&&KEY2==1&&KEY3==1)key_up=1; 	    
 	return 0;// �ް�������
}






void KeyBoard_Init() 
	{	
		GPIO_InitTypeDef GPIO_InitStructure; 
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3; 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
		GPIO_Init(GPIOC, &GPIO_InitStructure); 
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7; 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 
		GPIO_Init(GPIOC, &GPIO_InitStructure); 
		GPIO_SetBits(GPIOC, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3); 
		GPIO_ResetBits(GPIOC, GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7); 
	} 
/*------------------------------�������ɨ��--------------------------------------------* 
	���ܣ� ɨ�������̣������ؼ�ֵ * ������ * ���룺 �� 
* ���أ� �м����·��ظü�ֵ * �޼�����ʱ�򷵻�0 
--------------------------------------------------------------------------------------*/ 
u8 Read_KeyValue(void) 
	{ 

		u8 KeyValue=0; 

		if((GPIO_ReadInputData(GPIOC)&0xff)!=0x0f) 
		{
			delay_ms(10); 
			if((GPIO_ReadInputData(GPIOC)&0xff)!=0x0f) 
			{ 
				GPIO_SetBits(GPIOC, GPIO_Pin_0); 
				GPIO_ResetBits(GPIOC, GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3); 
				switch(GPIO_ReadInputData(GPIOC)&0xff) 
					{					
					case 0x11: KeyValue = 1; break; 
					case 0x21: KeyValue = 5; break; 
					case 0x41: KeyValue = 9; break; 
					case 0x81: KeyValue = 13;break; 
					} 
			GPIO_SetBits(GPIOC, GPIO_Pin_1); 
			GPIO_ResetBits(GPIOC, GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_3); 
			switch(GPIO_ReadInputData(GPIOC)&0xff) 
				{ 
				case 0x12: KeyValue = 2; break; 
				case 0x22: KeyValue = 6; break; 
				case 0x42: KeyValue = 10;break; 
				case 0x82: KeyValue = 14;break; 
				} 
			GPIO_SetBits(GPIOC, GPIO_Pin_2); 
			GPIO_ResetBits(GPIOC, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3); 
			switch(GPIO_ReadInputData(GPIOC)&0xff) 
				{ 
				case 0x14: KeyValue = 3; break; 
				case 0x24: KeyValue = 7; break; 
				case 0x44: KeyValue = 11;break; 
				case 0x84: KeyValue = 15;break; 
				} 
		GPIO_SetBits(GPIOC, GPIO_Pin_3); 
		GPIO_ResetBits(GPIOC, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2); 
			switch(GPIO_ReadInputData(GPIOC)&0xff) 
				{ 
				case 0x18: KeyValue = 4; break; 
				case 0x28: KeyValue = 8; break; 
				case 0x48: KeyValue = 12;break; 
				case 0x88: KeyValue = 16;break; 
				} 
		GPIO_SetBits(GPIOC, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3); 
		GPIO_ResetBits(GPIOC, GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7); 
		while((GPIO_ReadInputData(GPIOC)&0xff)!=0x0f); 
				return KeyValue;
			} 
		} 
	return 0; 
	}
























