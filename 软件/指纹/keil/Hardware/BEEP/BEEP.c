#include "BEEP.h"
#include "delay.h"

void BEEP_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PB,PE端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;				 //LED0-->PB.5 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
 GPIO_SetBits(GPIOB,GPIO_Pin_15);	
}
 
void BEEP_ON( ){                        //蜂鸣器响一声
	GPIO_ResetBits(GPIOB,GPIO_Pin_15);	
	DelayMs(100);
	GPIO_SetBits(GPIOB,GPIO_Pin_15);
	
}
//继电器初始化
//void LED_Init(void)
//{
// 
// GPIO_InitTypeDef  GPIO_InitStructure;
// 	
// RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PB,PE端口时钟
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;				 //LED0-->PB.5 端口配置
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
// GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
//// GPIO_ResetBits(GPIOB,GPIO_Pin_14);			
//GPIO_SetBits(GPIOB,GPIO_Pin_14);						
//	DelayMs(1000);
//	GPIO_ResetBits(GPIOB,GPIO_Pin_14);			
//}
//继电器工作
void JDQ_init(void)
{
	GPIO_SetBits(GPIOB,GPIO_Pin_14);						
	DelayMs(1000);
	GPIO_ResetBits(GPIOB,GPIO_Pin_14);		
}