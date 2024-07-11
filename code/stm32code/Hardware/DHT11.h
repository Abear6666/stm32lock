#ifndef __DHT11_H
#define __DHT11_H	 
#include "sys.h"
 
//IO��������
#define DHT11_IO_IN()  {GPIOA->CRL&=0XFFFFFFF0;GPIOA->CRL|=8;}//PA0
#define DHT11_IO_OUT() {GPIOA->CRL&=0XFFFFFFF0;GPIOA->CRL|=3;}
////IO��������											   
#define	DHT11_DQ_OUT PAout(8) //���ݶ˿�	PA0������ 
#define	DHT11_DQ_IN  PAin(8)  //���ݶ˿�	PA0�뷽��


u8 DHT11_Init(void);//��ʼ��DHT11
u8 DHT11_Read_Data(u8 *humiH,u8 *humiL,u8 *tempH,u8 *tempL);//��ȡ��ʪ��
u8 DHT11_Read_Byte(void);//����һ���ֽ�
u8 DHT11_Read_Bit(void);//����һ��λ
u8 DHT11_Check(void);//����Ƿ����DHT11
void DHT11_Rst(void);//��λDHT11 

		 				    
#endif
