#ifndef __NVIC_H
#define __NVIC_H	 
#include "sys.h"
#define  INFREAD10 GPIO_ReadInputDataBit(GPIOA,INFRE10) //��ȡ�˿ڵ�ƽ
#define  INFREAD11 GPIO_ReadInputDataBit(GPIOA,INFRE11) //��ȡ�˿ڵ�ƽ

extern u8 INT_MARK;//�жϱ�־λ
extern u8 INT_MARK1;
extern u8 INT_MARK2;
#define INFRE10 GPIO_Pin_10;
#define INFRE11 GPIO_Pin_4;

void Infrared_INT_INIT (void);

#endif
