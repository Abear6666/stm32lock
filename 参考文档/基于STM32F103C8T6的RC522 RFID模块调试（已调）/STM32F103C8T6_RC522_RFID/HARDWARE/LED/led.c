#include "led.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   

//��ʼ��PB5��PE5Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
// RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PB�˿�ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOE, ENABLE);	 //ʹ��PB�˿�ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);   //����AFIOʱ��
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE); //�ر�JTAG��ΪҪʹ��PB3��4
	
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_14 | GPIO_Pin_3;				 //LED0-->PB.14 beep->PB4 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.14
 

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;				 //LED0-->PB.14 beep->PB4 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.14
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				 //LED0-->PB.14 beep->PB4 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOE, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.14
	
//	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_3;				 //LED0-->PB.14 beep->PB4 �˿�����
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //�������
// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;		 //IO���ٶ�Ϊ50MHz
// GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.14
	
//	//beep ���ų�ʼ��
//	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				 //LED0-->PB.14 beep->PB4 �˿�����
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
// GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.14
 GPIO_SetBits(GPIOB,GPIO_Pin_14);						 //PB.4 ���1
//	GPIO_SetBits(GPIOB,GPIO_Pin_5);						 //PB.4 ���1
	
	GPIO_ResetBits(GPIOE,GPIO_Pin_0);						 //PB.4 ���1
	GPIO_ResetBits(GPIOB,GPIO_Pin_4);						 //PB.14 �����
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);						 //PB.4 ���1
	GPIO_ResetBits(GPIOA,GPIO_Pin_0);						 //PB.14 �����

}
 
