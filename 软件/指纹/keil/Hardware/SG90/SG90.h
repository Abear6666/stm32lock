#ifndef __SG90_H
#define __SG90_H	 
#include "sys.h"
#include "delay.h"

#define SE_PORT	GPIOA	//����IO�ӿ�
#define SE_OUT	GPIO_Pin_15	//����IO�ӿ�


void SG90_Init(u16 arr,u16 psc);//SG90�����ʼ�� ��ʱ����ʼ��
//void SG90_angle(u8 a);//����Ƕ�����    //����û�ã�ֱ���õĶ�ʱ����pwm
void steer();	 				    
#endif
