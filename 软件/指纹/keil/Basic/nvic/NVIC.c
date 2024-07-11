/*2022-10-16�����жϰ�������*/
#include "NVIC.h"
#include "delay.h"
#include "OLED_I2C.h"
u8 INT_MARK1;
u8 INT_MARK2;

void Infrared_INT_INIT(void){
	GPIO_InitTypeDef  GPIO_InitStructure; //����GPIO�ĳ�ʼ��ö�ٽṹ	
	NVIC_InitTypeDef NVIC_InitStruct;
	EXTI_InitTypeDef EXTI_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); //����GPIOʱ�� ����Ҫ�븴��ʱ��һͬ������
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);           //peripheral��Χ��,���ø���ʱ��	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4; //ѡ��˿ںţ�0~15��all��                        
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //ѡ��IO�ӿڹ�����ʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //����IO�ӿ��ٶȣ�2/10/50MHz��    
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; //ѡ��˿ںţ�0~15��all��                        
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //ѡ��IO�ӿڹ�����ʽ   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //����IO�ӿ��ٶȣ�2/10/50MHz��     
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
//��һ���ж� PA0��
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0);  //ѡ���ӦGPIO�ܽ���Ϊ�ⲿ�ж���·
	EXTI_InitStruct.EXTI_Line=EXTI_Line0;
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;
	EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Falling;  //�����ش���
	
	EXTI_Init(&EXTI_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel=EXTI0_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=3;
	NVIC_Init(&NVIC_InitStruct);


//��һ���ж� PA1��
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource1);  //ѡ���ӦGPIO�ܽ���Ϊ�ⲿ�ж���·
	EXTI_InitStruct.EXTI_Line=EXTI_Line1;
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;
	EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Falling;  //�����ش���
	
	EXTI_Init(&EXTI_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel=EXTI1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=3;
	NVIC_Init(&NVIC_InitStruct);
	
//��һ���ж� PA4��
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource4);  //ѡ���ӦGPIO�ܽ���Ϊ�ⲿ�ж���·
	EXTI_InitStruct.EXTI_Line=EXTI_Line4;
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;
	EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Falling;  //�����ش���
	
	EXTI_Init(&EXTI_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel=EXTI4_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=2;
	NVIC_Init(&NVIC_InitStruct);
}
//��stm32��˵������ԽС�����ȼ�Խ��






