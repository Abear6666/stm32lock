#ifndef __CH376_H
#define __CH376_H	 
#include "sys.h"
#include "spi.h"
#include "delay.h"
#include "ch376inc.h"

#define CH376_INTPORT		GPIOA	//定义IO接口
#define CH376_INT			GPIO_Pin_15	//定义IO接口


void 	CH376_PORT_INIT( void ); /* CH376通讯接口初始化 */
void	xEndCH376Cmd( void ); /* 结束SPI命令 */
void 	xWriteCH376Cmd( u8 mCmd ); /* 向CH376写命令 */
void 	xWriteCH376Data( u8 mData );/* 向CH376写数据 */
u8 	xReadCH376Data( void );	/* 从CH376读数据 */
u8 	Query376Interrupt( void );/* 查询CH376中断(INT#引脚为低电平) */
u8 	mInitCH376Host( void ); /* 初始化CH376 */

		 				    
#endif
