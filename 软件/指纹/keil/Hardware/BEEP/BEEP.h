#ifndef _BEEP_H	
#define _BEEP_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
#define BEEP PBout(7)// PE5	
#define LED0 PBout(14)// �̵�������PB14
void BEEP_Init(void);//��ʼ��
void BEEP_ON( );   //��������


void LED_Init(void);//�̵�����ʼ��
void JDQ_init(void);//�̵�������
		 				    
		 				    
#endif
