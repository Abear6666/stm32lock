#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//按键驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////  
								    
//按键初始化函数
void KEY_Init(void) //IO初始化
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOE,ENABLE);//使能PORTA,PORTB,PORTC时钟

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12|GPIO_Pin_13 |GPIO_Pin_5 |GPIO_Pin_3 ;//KEY0-KEY2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB12,13
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3;//KEY0-KEY2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //设置成上拉输入
 	GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIOB12,13

}
//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，KEY0按下
//2，KEY1按下
//3，KEY2按下 
//4，KEY3按下 WK_UP
//注意此函数有响应优先级,KEY0>KEY1>KEY2>KEY3!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按		  
	if(key_up&&(KEY0==0||KEY1==0||KEY2==0||KEY3==0))
	{
		delay_ms(10);//去抖动 
		key_up=0;
		if(KEY0==0)return KEY0_PRES;
		else if(KEY1==0)return KEY1_PRES;
		else if(KEY2==0)return KEY2_PRES;
		else if(KEY3==0)return KEY3_PRES;
	}else if(KEY0==1&&KEY1==1&&KEY2==1&&KEY3==1)key_up=1; 	    
 	return 0;// 无按键按下
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
/*------------------------------矩阵键盘扫描--------------------------------------------* 
	功能： 扫描矩阵键盘，并返回键值 * 参数： * 输入： 无 
* 返回： 有键按下返回该键值 * 无键按下时则返回0 
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
























