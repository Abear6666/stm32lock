#ifndef __NVIC_H
#define __NVIC_H	 
#include "sys.h"
#define  INFREAD10 GPIO_ReadInputDataBit(GPIOA,INFRE10) //读取端口电平
#define  INFREAD11 GPIO_ReadInputDataBit(GPIOA,INFRE11) //读取端口电平

extern u8 INT_MARK;//中断标志位
extern u8 INT_MARK1;
extern u8 INT_MARK2;
#define INFRE10 GPIO_Pin_10;
#define INFRE11 GPIO_Pin_4;

void Infrared_INT_INIT (void);

#endif
