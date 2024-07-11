#include "BEEP.h"
#include "delay.h"

void BEEP_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PB,PE�˿�ʱ��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;				 //LED0-->PB.5 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
 GPIO_SetBits(GPIOB,GPIO_Pin_15);	
}
 
void BEEP_ON( ){                        //��������һ��
	GPIO_ResetBits(GPIOB,GPIO_Pin_15);	
	DelayMs(100);
	GPIO_SetBits(GPIOB,GPIO_Pin_15);
	
}
//�̵�����ʼ��
//void LED_Init(void)
//{
// 
// GPIO_InitTypeDef  GPIO_InitStructure;
// 	
// RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PB,PE�˿�ʱ��
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;				 //LED0-->PB.5 �˿�����
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
// GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
//// GPIO_ResetBits(GPIOB,GPIO_Pin_14);			
//GPIO_SetBits(GPIOB,GPIO_Pin_14);						
//	DelayMs(1000);
//	GPIO_ResetBits(GPIOB,GPIO_Pin_14);			
//}
//�̵�������
void JDQ_init(void)
{
	GPIO_SetBits(GPIOB,GPIO_Pin_14);						
	DelayMs(1000);
	GPIO_ResetBits(GPIOB,GPIO_Pin_14);		
}