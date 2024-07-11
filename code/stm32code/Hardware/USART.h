#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"

#define USART_DEBUG		USART1		//调试打印所使用的串口组

void USART1_Init(unsigned int baud);
void DEBUG_LOG(char *fmt,...);
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...);
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len);




#define USART3_MAX_RECV_LEN		400					//最大接收缓存字节数
#define USART3_MAX_SEND_LEN		400					//最大发送缓存字节数
#define USART3_RX_EN 			1					//0,不接收;1,接收.

extern u8  USART3_RX_BUF[USART3_MAX_RECV_LEN]; 		//接收缓冲,最大USART2_MAX_RECV_LEN字节
extern u8  USART3_TX_BUF[USART3_MAX_SEND_LEN]; 		//发送缓冲,最大USART2_MAX_SEND_LEN字节
extern u16 USART3_RX_STA;   						//接收数据状态

void USART3_Init(u32 bound);				//串口2初始化 
void u3_printf(char* fmt,...);
void USART2_Init(unsigned int baud);


#endif


