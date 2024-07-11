#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"

//#define KEY1 PAin(0)// PA0
//#define KEY2 PAin(1)// PA1

#define KEYPORT	GPIOA	//定义IO接口组
#define KEY1	GPIO_Pin_0	//定义IO接口
#define KEY2	GPIO_Pin_1	//定义IO接口


void KEY_Init(void);//初始化

		 				    
#endif
