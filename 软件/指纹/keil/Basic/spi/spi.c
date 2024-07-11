

/*
//杜洋工作室出品
//洋桃系列开发板应用程序
//关注微信公众号：洋桃电子
//洋桃开发板资料下载 www.DoYoung.net/YT 
//即可免费看所有教学视频，下载技术资料，技术疑难提问
//更多内容尽在 杜洋工作室主页 www.doyoung.net
*/

/*
《修改日志》
1-201708202309 创建。


*/



#include "spi.h"


void SPI2_Init(void){ //SPI2初始化
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);//使能SPI_2时钟

	GPIO_InitStructure.GPIO_Pin = SPI2_MISO;  //SPI2的MISO（PB14）为浮空输入
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(SPI2PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = SPI2_MOSI | SPI2_SCK;	//SPI2的MOSI（PB15）和SCLK（PB13）为复用推免输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(SPI2PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = SPI2_NSS;	 //SPI2的NSS（PB12）为推免输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SPI2PORT,&GPIO_InitStructure);

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//双线输入输出全双工模式
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;//设置为SPI的主机模式（SCK主动产生时钟）
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;//SPI数据大小：发送8位帧数据结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;//空闲状态时SCK的状态，High为高电平，Low为低电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;//时钟相位，1表示在SCK的奇数沿边采样，2表示偶数沿边采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; //NSS由软件控件片选
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;//时钟的预分频值（0~256）
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; //MSB高位在前
	SPI_InitStructure.SPI_CRCPolynomial = 7; //CRC较验和的多项式
	SPI_Init(SPI2,&SPI_InitStructure); //初始化SPI2的配置项
	SPI_Cmd(SPI2,ENABLE); //使能SPI2  
}

//SPI2数据发+收程序（主要用于发送）
u8 SPI2_SendByte(u8 Byte){ //通过SPI2口发送1个数据，同时接收1个数据
	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE) == RESET); //如果发送寄存器数据没有发送完，循环等待
	SPI_I2S_SendData(SPI2,Byte);  //往发送寄存器写入要发送的数据
	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_RXNE) == RESET); //如果接受寄存器没有收到数据，循环
	return SPI_I2S_ReceiveData(SPI2);
}

 
/*********************************************************************************************
 * 杜洋工作室 www.DoYoung.net
 * 洋桃电子 www.DoYoung.net/YT 
*********************************************************************************************/


/*
应用的方法：

SPI_SendByte(a); //发送一个字节数据，a是一个变量或者数值


*/
