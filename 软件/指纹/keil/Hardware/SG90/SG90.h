#ifndef __SG90_H
#define __SG90_H	 
#include "sys.h"
#include "delay.h"

#define SE_PORT	GPIOA	//定义IO接口
#define SE_OUT	GPIO_Pin_15	//定义IO接口


void SG90_Init(u16 arr,u16 psc);//SG90舵机初始化 定时器初始化
//void SG90_angle(u8 a);//舵机角度设置    //现在没用，直接用的定时器调pwm
void steer();	 				    
#endif
