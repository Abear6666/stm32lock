#include "rc522.h"
#include "stdio.h"

//SPI2_SCK 				PB13
//SPI2_MISO				PB14
//SPI2_MOSI 			PB15
//RCC522_RST(CE)        PB9
//RCC522_NSS(SDA）      PB8
//RCC522_IRQ 			悬空


uint8_t UID[5],Temp[4];          
uint8_t RF_Buffer[18];
uint8_t Password_Buffer[6]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}; // Mifare One 缺省密码

void RC522_Init(void)
{
	RC522_SDA_LOW;
	HAL_SPI_Transmit(&hspi2, (uint8_t *)0xaa, sizeof((uint8_t *)0xaa), 0xFF);//启动传输	
	RC522_SDA_HIGH;

    HAL_Delay(50);
	PcdReset();//复位RC522读卡器
	HAL_Delay(10);
	PcdReset();//复位RC522读卡器
	HAL_Delay(10);
	PcdAntennaOff();//关闭天线发射                                                         
	HAL_Delay(10);	 
    PcdAntennaOn();//开启天线发射
	printf("RFID-MFRC522 初始化完成\r\nFindCard Starting ...\r\n");  //测试引脚初始化完成
}


void delay_ns(uint32_t ns)
{
  uint32_t i;
  for(i=0;i<ns;i++)
  {
    __nop();
    __nop();
    __nop();
  }
}



//SPIx 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
//***************************************************************************/

char RC522_ReadWriteByte(uint8_t TxData)
{					 	
	while (SPI_CHECK_FLAG(SPI2->SR,((uint16_t)0x0002)) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
		{
			;
		}			  
	SPI2->DR=TxData; 																//通过外设SPIx发送一个数据
		while (SPI_CHECK_FLAG(SPI2->SR,((uint16_t)0x0001)) == RESET) //检查指定的SPI标志位设置与否:接受缓存非空标志位
		{
		}	  						    
	return SPI2->DR; 															//返回通过SPIx最近接收的数据					    
}

//******************************************************************/
//功    能：读RC522寄存器
//参数说明：Address[IN]:寄存器地址
//返    回：读出的值
//******************************************************************/
unsigned char ReadRawRC(unsigned char Address)
{
	uint8_t ucAddr;
  uint8_t ucResult=0;
	RC522_SDA_LOW;
	HAL_Delay(10);
   ucAddr = ((Address<<1)&0x7E)|0x80;
	RC522_ReadWriteByte(ucAddr);
	HAL_Delay(10);
	ucResult=RC522_ReadWriteByte(0);
	HAL_Delay(10);
	RC522_SDA_HIGH;
  return ucResult;
}

//******************************************************************/
//功    能：写RC522寄存器
//参数说明：Address[IN]:寄存器地址
//          value[IN]:写入的值
//******************************************************************/
void WriteRawRC(unsigned char Address, unsigned char value)
{
	uint8_t ucAddr;
	RC522_SDA_LOW;
	HAL_Delay(10);
	ucAddr = ((Address<<1)&0x7E) ;
	RC522_ReadWriteByte(ucAddr);
	HAL_Delay(10);
	RC522_ReadWriteByte(value);
	HAL_Delay(10);
	RC522_SDA_HIGH;
}

//******************************************************************/
//功    能：置RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:置位值
//******************************************************************/
void SetBitMask(unsigned char reg,unsigned char mask)  
{
  char tmp = 0x0            ;
  tmp = ReadRawRC(reg)| mask;
  WriteRawRC(reg,tmp | mask);  // set bit mask
}

//******************************************************************/
//功    能：清RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:清位值
//******************************************************************/
void ClearBitMask(unsigned char reg,unsigned char mask)  
{
  char tmp = 0x0              ;
  tmp = ReadRawRC(reg)&(~mask);
  WriteRawRC(reg, tmp)        ;  // clear bit mask
} 

//******************************************************************/
//功    能：复位RC522
//返    回: 成功返回MI_OK
//******************************************************************/
char PcdReset()
{
  RC522_RST_HIGH;                             ;
  delay_ns(10)                             ;
  RC522_RST_LOW;                             ;
  delay_ns(100)                             ;
  RC522_RST_HIGH;                             ;
  delay_ns(10)                           ;
  WriteRawRC(CommandReg,PCD_RESETPHASE);
  delay_ns(100)                             ;
  WriteRawRC(ModeReg,0x3D)             ;//定义发送和接收常用模式 和Mifare卡通讯，CRC初始值0x6363
  WriteRawRC(TReloadRegL,30)           ;//16位定时器低位 30
  WriteRawRC(TReloadRegH,0)            ;//16位定时器高位
  WriteRawRC(TModeReg,0x8D)            ;//定义内部定时器的设置
  WriteRawRC(TPrescalerReg,0x3E)       ;//设置定时器分频系数  
  WriteRawRC(TxASKReg,0x40)            ;//调制发送信号为100%ASK
  return MI_OK                         ; 
}
////////////////////////////////////////////////////////////////////// 
//设置RC522的工作方式   
//////////////////////////////////////////////////////////////////////
char MF522PcdConfigISOType(unsigned char  type)
{
	if (type == 'A')	//ISO14443_A
		{
			ClearBitMask(Status2Reg,0x08);	//状态2寄存器         
			WriteRawRC(ModeReg,0x3D);	//3F  //和Mifare卡通讯，CRC初始值0x6363         
			WriteRawRC(RxSelReg,0x86);	//84   选择内部接收器设置，内部模拟部分调制信号，发送数据后，延迟6个位时钟，接收         
			WriteRawRC(RFCfgReg,0x7F);	//4F  配置接收器  48dB最大增益         
			WriteRawRC(TReloadRegL,30);	//tmoLength);TReloadVal = 'h6a =tmoLength(dec)      
			WriteRawRC(TReloadRegH,0);	//实际值是OXD3E 这部分主要是设置定时器寄存器       
			WriteRawRC(TModeReg,0x8D);
			WriteRawRC(TPrescalerReg,0x3E);
			delay_ns(1000);
			PcdAntennaOn();
		}     
	else {return 0xFE;}  
	return MI_OK; 
}  

//******************************************************************/
//开启天线发射  
//每次启动或关闭天险发射之间应至少有1ms的间隔
//******************************************************************/
void PcdAntennaOn()
{
  unsigned char i;
  WriteRawRC(TxASKReg,0x40)       ;
  HAL_Delay(1);
  i = ReadRawRC(TxControlReg)     ;
  if(!(i&0x03))
    SetBitMask(TxControlReg, 0x03);
  i=ReadRawRC(TxASKReg)       ;
}


//******************************************************************/
//关闭天线发射
//******************************************************************/
void PcdAntennaOff()
{
  ClearBitMask(TxControlReg, 0x03);
}

//******************************************************************/
//功    能：通过RC522和ISO14443卡通讯
//参数说明：Command[IN]:RC522命令字
//          pInData[IN]:通过RC522发送到卡片的数据
//          InLenByte[IN]:发送数据的字节长度
//          pOutData[OUT]:接收到的卡片返回数据
//          *pOutLenBit[OUT]:返回数据的位长度
//******************************************************************/
char PcdComMF522(unsigned char Command  ,unsigned char *pInData , 
                 unsigned char InLenByte,unsigned char *pOutData, 
                 unsigned int  *pOutLenBit                       )
{
  char status = MI_ERR                          ;
  unsigned char irqEn   = 0x00                  ;
  unsigned char waitFor = 0x00                  ;
  unsigned char lastBits                        ;
  unsigned char n                               ;
  unsigned int  i                               ;
  switch (Command)
  {
    case PCD_AUTHENT:
      irqEn   = 0x12                            ;
      waitFor = 0x10                            ;
      break                                     ;
    case PCD_TRANSCEIVE:
      irqEn   = 0x77                            ;
      waitFor = 0x30                            ;
      break                                     ;
    default:
      break                                     ;
  }
  WriteRawRC(ComIEnReg,irqEn|0x80)              ; //
  ClearBitMask(ComIrqReg,0x80)                  ;
  WriteRawRC(CommandReg,PCD_IDLE)               ;
  SetBitMask(FIFOLevelReg,0x80)                 ; // 清空FIFO 
  for(i=0; i<InLenByte; i++)
    WriteRawRC(FIFODataReg,pInData[i])          ; // 数据写入FIFO 
  WriteRawRC(CommandReg, Command)               ; // 命令写入命令寄存器
  if(Command == PCD_TRANSCEIVE)
    SetBitMask(BitFramingReg,0x80)              ; // 开始发送     
  i = 6000                                      ; //根据时钟频率调整，操作M1卡最大等待时间25ms
  do 
  {
    n = ReadRawRC(ComIrqReg)                    ;
    i--                                         ;
  }
  while((i!=0)&&!(n&0x01)&&!(n&waitFor))        ;
  ClearBitMask(BitFramingReg,0x80)              ;
  if(i!=0)
  {
    if(!(ReadRawRC(ErrorReg)&0x1B))
    {
      status = MI_OK                            ;
      if (n&irqEn&0x01)
        status = MI_NOTAGERR                    ;
      if(Command==PCD_TRANSCEIVE)
      {
        n = ReadRawRC(FIFOLevelReg)             ;
        lastBits = ReadRawRC(ControlReg)&0x07   ;
        if(lastBits)
          *pOutLenBit = (n-1)*8 + lastBits      ;
        else
          *pOutLenBit = n*8                     ;
        if(n==0)
          n = 1                                 ;
        if(n>MAXRLEN)
          n = MAXRLEN                           ;
        for (i=0; i<n; i++)
          pOutData[i] = ReadRawRC(FIFODataReg)  ; 
      }
    }
    else
      status = MI_ERR                           ;        
  }
  SetBitMask(ControlReg,0x80)                   ;// stop timer now
  WriteRawRC(CommandReg,PCD_IDLE)               ; 
  return status;
}

//******************************************************************/
//功    能：寻卡                                                    /
//参数说明: req_code[IN]:寻卡方式                                   /
//                0x52 = 寻感应区内所有符合14443A标准的卡           /
//                0x26 = 寻未进入休眠状态的卡                       /
//                pTagType[OUT]：卡片类型代码                       /
//                0x4400 = Mifare_UltraLight                        /
//                0x0400 = Mifare_One(S50)                          /
//                0x0200 = Mifare_One(S70)                          /
//                0x0800 = Mifare_Pro(X)                            /
//                0x4403 = Mifare_DESFire                           /
//返    回: 成功返回MI_OK                                           /
//******************************************************************/
char PcdRequest(unsigned char req_code,unsigned char *pTagType)
{
  char status                                        ;  
  unsigned int  unLen                                ;
  unsigned char ucComMF522Buf[MAXRLEN]               ; 

  ClearBitMask(Status2Reg,0x08)                      ;//清除MRCrypto1on，要用软件清零
  WriteRawRC(BitFramingReg,0x07)                     ;//startsend=0,rxalign=0,在FIFO中存放的位置，TXlastbit=7
  SetBitMask(TxControlReg,0x03)                      ;//TX2rfen=1,TX1RFen=1,传递调制的13.56MHZ的载波信号
 
  ucComMF522Buf[0] = req_code                        ;

  status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen       );
  if ((status == MI_OK) && (unLen == 0x10))
  {    
    *pTagType     = ucComMF522Buf[0]                 ;
    *(pTagType+1) = ucComMF522Buf[1]                 ;
  }
  else
    status = MI_ERR                                  ;
  return status                                      ;
}

//******************************************************************/
//功    能：防冲撞                                                  /
//参数说明: pSnr[OUT]:卡片序列号，4字节                             /
//返    回: 成功返回MI_OK                                           /
//******************************************************************/
char PcdAnticoll(unsigned char *pSnr)
{
    char status;
    unsigned char i,snr_check=0;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 
    
    ClearBitMask(Status2Reg,0x08);//清除MRCrypto1on，要用软件清零
    WriteRawRC(BitFramingReg,0x00);//表示最后一个字节所有位都发送
    ClearBitMask(CollReg,0x80);//CollRegCollReg0冲突结束后冲突位被置零
 
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x20;

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);

    if (status == MI_OK)
    {
    	 for (i=0; i<4; i++)
         {   
             *(pSnr+i)  = ucComMF522Buf[i];
             snr_check ^= ucComMF522Buf[i];
         }
         if (snr_check != ucComMF522Buf[i])
         {   status = MI_ERR;    }
    }
    
    SetBitMask(CollReg,0x80);//CollRegCollReg 在106kbps良好的防冲突情况下该位置1
    return status;
}



//==============================================================================
//读取卡的类型
//读取卡的ID号
//==============================================================================
void ReaderCard(void)
{
	char temp_value;
    
	if(PcdRequest(PICC_REQALL,Temp)==MI_OK)	//选卡
	{
		if(Temp[0]==0x04&&Temp[1]==0x00)  
				printf("\r\nMFOne-S50\r\n");
		else if(Temp[0]==0x02&&Temp[1]==0x00)
			printf("MFOne-S70");
		else if(Temp[0]==0x44&&Temp[1]==0x00)
			printf("MF-UltraLight");
		else if(Temp[0]==0x08&&Temp[1]==0x00)
			printf("MF-Pro");
		else if(Temp[0]==0x44&&Temp[1]==0x03)
			printf("MF Desire");
		else
			printf("Unknown");
		if(PcdAnticoll(UID)==MI_OK)			//防冲撞
		{ 
			printf("Card Id is:");
			/* 获取卡值  */
			printf("%d%d%d%d\r\n",UID[0],UID[1],UID[2],UID[3]); 		              
			temp_value = ((UID[0]>>4)*10+(UID[0]&0x0f));
			switch(temp_value)
			{
				case 63 : printf("管理员:%d 验证通过!\r\n",temp_value);    break;
				default : printf("无效卡:%d 验证失败!\r\n",temp_value);    break;
			}	                             
		}
  } 
}



























/////////////////////////////////////////////////////////////////////
//功    能：选定卡片
//参数说明: pSnr[IN]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
//char PcdSelect(unsigned char *pSnr)
//{
//    char status;
//    unsigned char i;
//    unsigned int  unLen;
//    unsigned char ucComMF522Buf[MAXRLEN]; 
//    
//    ucComMF522Buf[0] = PICC_ANTICOLL1;
//    ucComMF522Buf[1] = 0x70;
//    ucComMF522Buf[6] = 0;
//    for (i=0; i<4; i++)
//    {
//    	ucComMF522Buf[i+2] = *(pSnr+i);
//    	ucComMF522Buf[6]  ^= *(pSnr+i);
//    }
//    CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);
//  
//    ClearBitMask(Status2Reg,0x08);//清零MFcryon

//    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);
//    
//    if ((status == MI_OK) && (unLen == 0x18))
//    {   status = MI_OK;  }
//    else
//    {   status = MI_ERR;    }

//    return status;
//}

////******************************************************************/
////功    能：验证卡片密码
////参数说明: auth_mode[IN]: 密码验证模式
////                 0x60 = 验证A密钥
////                 0x61 = 验证B密钥 
////          addr[IN]：块地址
////          pKey[IN]：密码
////          pSnr[IN]：卡片序列号，4字节
////返    回: 成功返回MI_OK
////******************************************************************/
//char PcdAuthState(unsigned char auth_mode,unsigned char addr,
//                  unsigned char *pKey,unsigned char *pSnr    )
//{
//    char status;
//    unsigned int  unLen;
//    unsigned char i,ucComMF522Buf[MAXRLEN]; 

//    ucComMF522Buf[0] = auth_mode;//验证A密钥
//    ucComMF522Buf[1] = addr;//addr[IN]：块地址
//    for (i=0; i<6; i++)
//    {    ucComMF522Buf[i+2] = *(pKey+i);   }
//    for (i=0; i<6; i++)
//    {    ucComMF522Buf[i+8] = *(pSnr+i);   }
//    
//    status = PcdComMF522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
//    if ((status != MI_OK) || (!(ReadRawRC(Status2Reg) & 0x08)))
//    {   status = MI_ERR;   }
//    
//    return status;
//}

////******************************************************************/
////功    能：读取M1卡一块数据
////参数说明: addr[IN]：块地址
////          pData[OUT]：读出的数据，16字节
////返    回: 成功返回MI_OK
////******************************************************************/
//char PcdRead(unsigned char addr,unsigned char *pData)
//{
//    char status                                          ;
//    unsigned int  unLen                                  ;
//    unsigned char i,ucComMF522Buf[MAXRLEN]               ; 

//    ucComMF522Buf[0] = PICC_READ                         ;
//    ucComMF522Buf[1] = addr                              ;
//    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2])       ;   
//    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,
//                         ucComMF522Buf,&unLen           );
//    if ((status == MI_OK) && (unLen == 0x90))
//    {
//        for (i=0; i<16; i++)
//            *(pData+i) = ucComMF522Buf[i];   
//    }
//    else
//      status = MI_ERR;       
//    return status;
//}

////******************************************************************/
////功    能：读取M1卡一块数据
////参数说明: addr[IN]：块地址
////          pData[OUT]：读出的数据，16字节
////返    回: 成功返回MI_OK
////******************************************************************/
//char Read_Block(unsigned char Block,unsigned char *Buf)
//{
//  char result                                             ;
//  result = PcdAuthState(0x60,Block,Password_Buffer,UID)   ;
//  if(result!=MI_OK)
//    return result                                         ;
//  result = PcdRead(Block,Buf)                             ;
////  return result; // 2011.01.03
//  
//  if(result!=MI_OK)     return   result                   ;
//  if(Block!=0x00&&des_on)
//  {
////     Des_Decrypt((char *)Buf    ,KK,(char *)Buf    )       ;
////     Des_Decrypt((char *)&Buf[8],KK,(char *)&Buf[8])       ;  
//  }
//  return SUCCESS                                          ; 
//}

////******************************************************************/
////功    能：写数据到M1卡一块
////参数说明: addr[IN]：块地址
////          pData[IN]：写入的数据，16字节
////返    回: 成功返回MI_OK
////******************************************************************/
//char PcdWrite(unsigned char addr,unsigned char *pData)
//{
//  char status                                             ;
//  unsigned int  unLen                                     ;
//  unsigned char i,ucComMF522Buf[MAXRLEN]                  ; 
//    
//  ucComMF522Buf[0] = PICC_WRITE                           ;
//  ucComMF522Buf[1] = addr                                 ;
//  CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2])          ;
//  status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,
//                       ucComMF522Buf,&unLen          )    ;
//  if(  ( status != MI_OK)||(unLen != 4)
//     ||((ucComMF522Buf[0]&0x0F)!= 0x0A))
//    status = MI_ERR                                       ;           
//  if (status == MI_OK)
//  {
//    for (i=0; i<16; i++)
//      ucComMF522Buf[i] = *(pData+i)                       ;  
//    CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16])      ;
//    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,
//                         18,ucComMF522Buf,&unLen     )    ;
//    if(  (status != MI_OK)||(unLen != 4 )
//       ||((ucComMF522Buf[0]&0x0F)!= 0x0A))
//      status = MI_ERR                                     ;   
//  }    
//  return status                                           ;
//}
////******************************************************************/
////功    能：写数据到M1卡一块
////参数说明: addr[IN]：块地址
////          pData[IN]：写入的数据，16字节
////返    回: 成功返回MI_OK
////******************************************************************/

//char Write_Block(unsigned char Block)
//{
//  char result                                             ;
//  if(des_on)
//  {
////     Des_Encrypt((char *)RF_Buffer    ,KK,   (char *)RF_Buffer )                ;// for debug
////     Des_Encrypt((char *)&RF_Buffer[8],KK,   (char *)&RF_Buffer[8]  )                ;// for debug  
//  }
//  result = PcdAuthState(0x60,Block,Password_Buffer,UID)   ;
//  if(result!=MI_OK)
//    return result                                         ;  
//  result = PcdWrite(Block,RF_Buffer)                      ;
//  return result                                           ;  
//}

////******************************************************************/
////功    能：扣款和充值
////参数说明: dd_mode[IN]：命令字
////               0xC0 = 扣款
////               0xC1 = 充值
////          addr[IN]：钱包地址
////          pValue[IN]：4字节增(减)值，低位在前
////返    回: 成功返回MI_OK
////******************************************************************/
//char PcdValue(unsigned char dd_mode,unsigned char addr,unsigned char *pValue)
//{
//    char status;
//    unsigned int  unLen;
//    unsigned char i,ucComMF522Buf[MAXRLEN]; 
//    
//    ucComMF522Buf[0] = dd_mode;
//    ucComMF522Buf[1] = addr;
//    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
// 
//    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

//    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
//    {   status = MI_ERR;   }
//        
//    if (status == MI_OK)
//    {
//        for (i=0; i<16; i++)
//        {    ucComMF522Buf[i] = *(pValue+i);   }
//        CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
//        unLen = 0;
//        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
//        if (status != MI_ERR)
//        {    status = MI_OK;    }
//    }
//    
//    if (status == MI_OK)
//    {
//        ucComMF522Buf[0] = PICC_TRANSFER;
//        ucComMF522Buf[1] = addr;
//        CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]); 
//   
//        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

//        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
//        {   status = MI_ERR;   }
//    }
//    return status;
//}

////******************************************************************/
////功    能：备份钱包
////参数说明: sourceaddr[IN]：源地址
////          goaladdr[IN]：目标地址
////返    回: 成功返回MI_OK
////******************************************************************/
//char PcdBakValue(unsigned char sourceaddr, unsigned char goaladdr)
//{
//    char status;
//    unsigned int  unLen;
//    unsigned char ucComMF522Buf[MAXRLEN]; 

//    ucComMF522Buf[0] = PICC_RESTORE;
//    ucComMF522Buf[1] = sourceaddr;
//    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
// 
//    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

//    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
//    {   status = MI_ERR;   }
//    
//    if (status == MI_OK)
//    {
//        ucComMF522Buf[0] = 0;
//        ucComMF522Buf[1] = 0;
//        ucComMF522Buf[2] = 0;
//        ucComMF522Buf[3] = 0;
//        CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
// 
//        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
//        if (status != MI_ERR)
//        {    status = MI_OK;    }
//    }
//    
//    if (status != MI_OK)
//    {    return MI_ERR;   }
//    
//    ucComMF522Buf[0] = PICC_TRANSFER;
//    ucComMF522Buf[1] = goaladdr;

//    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
// 
//    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

//    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
//    {   status = MI_ERR;   }

//    return status;
//}


//******************************************************************/
//功    能：命令卡片进入休眠状态
//返    回: 成功返回MI_OK
//******************************************************************/
//char PcdHalt(void)
//{
//    char status;
//    unsigned int  unLen;
//    unsigned char ucComMF522Buf[MAXRLEN]; 

//    ucComMF522Buf[0] = PICC_HALT;
//    ucComMF522Buf[1] = 0;
//    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
// 
//    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
//    return status;
////    return MI_OK;
//}

//******************************************************************/
//功    能：命令卡片进入休眠状态
//返    回: 成功返回MI_OK
//******************************************************************/
//char MIF_Halt(void)
//{
//    char status;
//    unsigned int  unLen;
//    unsigned char ucComMF522Buf[MAXRLEN]; 

//    ucComMF522Buf[0] = PICC_HALT;
//    ucComMF522Buf[1] = 0;
//    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
// 
//    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
//    return status ;  
////    return MI_OK;
//}



//******************************************************************/
//用MF522计算CRC16函数
//******************************************************************/
//void CalulateCRC(unsigned char *pIndata,unsigned char len,unsigned char *pOutData)
//{
//    unsigned char i,n;
//    ClearBitMask(DivIrqReg,0x04);
//    WriteRawRC(CommandReg,PCD_IDLE);//取消当前命令
//    SetBitMask(FIFOLevelReg,0x80);//FlushBuffer 清除ErrReg 的标志位
//    for (i=0; i<len; i++)
//    {   WriteRawRC(FIFODataReg, *(pIndata+i));   }
//    WriteRawRC(CommandReg, PCD_CALCCRC);
//    i = 0xFF;
//    do 
//    {
//        n = ReadRawRC(DivIrqReg);
//        i--;
//    }
//    while ((i!=0) && !(n&0x04));//当CRCIRq所有数据被处理完毕该位置位
//    pOutData[0] = ReadRawRC(CRCResultRegL);//显示计算出来的CRC值
//    pOutData[1] = ReadRawRC(CRCResultRegM);
//}




