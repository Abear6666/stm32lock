#include "stm32f10x.h"                  // Device header
#include "Delay.h"

void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 |GPIO_Pin_5|GPIO_Pin_2;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}


uint8_t Key_Scan(void)
{
	uint8_t keynum;
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)==0)
	{
		Delay_ms(20);
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)==0)
		{
			while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)==0);
			keynum=1;
		}
	}
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7)==0)
	{
		Delay_ms(20);
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7)==0)
		{
			while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7)==0);
			keynum=2;
		}
	}
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)==0)
	{
		Delay_ms(20);
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)==0)
		{
			while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)==0);
			keynum=3;
		}
	}
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)==0)
	{
		Delay_ms(20);
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)==0)
		{
			while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)==0);
			keynum=4;
		}
	}
	return keynum;
}
