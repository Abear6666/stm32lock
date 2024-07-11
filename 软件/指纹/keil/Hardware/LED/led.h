#ifndef __LED_H
#define __LED_H	 
#include "sys.h"

//#define LED1 PBout(0)// PB0
//#define LED2 PBout(1)// PB1	
#define LEDPORT	GPIOB	//定义IO接口
#define LED1	GPIO_Pin_0	//定义IO接口
#define LED2	GPIO_Pin_1	//定义IO接口



void LED_Init(void);//初始化

		 				    
#endif
