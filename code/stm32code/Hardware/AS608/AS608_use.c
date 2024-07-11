#include "AS608_use.h"
#include "usart.h"
#include "OLED.h"
#include "delay.h"
#include "as608.h"
#include "onenet.h"
#include "esp8266.h"

SysPara AS608Para;							/*指纹模块AS608参数*/
u16 ValidN;											/*模块内有效指纹个数*/
extern void unlock_interface(void);  //解锁界面
extern void main_interface(void);
extern u8 Fail_Num; // 门禁解锁错误次数
extern const char devPubTopic[];

//显示确认码错误信息
void ShowErrMessage(u8 ensure)
{
		
	DEBUG_LOG("\r\n错误信息：%s \r\n",(u8*)EnsureMessage(ensure));
	
}

//查询指纹个数发送到小程序查看
void upload_fin_number(void)
{
		char testbuf[125] = {""};
		u16 sum = 0;
		
		PS_ValidTempleteNum(&sum);//读库指纹个数
		sprintf(testbuf,"{\"Fin_Msg\":%d}",sum);
		DEBUG_LOG("发布数据 ---%s-- OneNet_Publish",testbuf);

		OneNet_Publish(devPubTopic,testbuf);
		DEBUG_LOG("==================================================================================");		
		ESP8266_Clear();		


}

//刷指纹
void press_FR(void)
{
	SearchResult seach;
	u8 ensure;
	ensure=PS_GetImage();
	if(ensure==0x00)//获取图像成功 
	{	

		ensure=PS_GenChar(CharBuffer1);
		if(ensure==0x00) //生成特征成功
		{		
			ensure=PS_HighSpeedSearch(CharBuffer1,0,AS608Para.PS_max,&seach);
			if(ensure==0x00)//搜索成功
			{				
				DEBUG_LOG("刷指纹成功\r\n");
				DEBUG_LOG("确有此人,ID:%d  匹配得分:%d",seach.pageID,seach.mathscore);
			}
			else 
				ShowErrMessage(ensure);					
	  }
		else
			ShowErrMessage(ensure);

	}
		
}

//录指纹
void Add_FR(u16 FR_ID)
{
		OLED_Clear();
	//显示录入指纹中
	OLED_WriteCN(16,1,16,6);    //显示->显示录入指纹中
	OLED_ShowString(3,5,"...");
	delay_s(1);
	u8 i,ensure ,processnum=0;
	u16 ID;
	while(1)
	{
		switch (processnum)
		{
			case 0:
				i++;
				DEBUG_LOG("请按指纹\r\n");
				OLED_Clear();
				OLED_WriteCN(32,1,19,6);    //显示->请按指纹
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					ensure=PS_GenChar(CharBuffer1);//生成特征
					if(ensure==0x00)
					{
						DEBUG_LOG("指纹正常\r\n");
						OLED_Clear();
						OLED_WriteCN(32,1,20,6);    //显示->请按指纹
						delay_s(1);
						i=0;
						processnum=1;//跳到第二步						
					}else ShowErrMessage(ensure);				
				}else ShowErrMessage(ensure);						
				break;
			
			case 1:
				i++;
				DEBUG_LOG("请按再按一次指纹\r\n");
					OLED_Clear();
						OLED_WriteCN(32,1,21,6);    //显示->请按指纹
						delay_s(1);
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					ensure=PS_GenChar(CharBuffer2);//生成特征
					if(ensure==0x00)
					{
						DEBUG_LOG("指纹正常\r\n");
	
						OLED_Clear();
						OLED_WriteCN(32,1,20,6);    //显示->请按指纹
						delay_s(1);
						i=0;
						processnum=2;//跳到第三步
					}else ShowErrMessage(ensure);	
				}else ShowErrMessage(ensure);		
				break;

			case 2:
				DEBUG_LOG("对比两次指纹\r\n");
				ensure=PS_Match();
				if(ensure==0x00) 
				{
					DEBUG_LOG("对比成功,两次指纹一样\r\n");
					processnum=3;//跳到第四步
				}
				else 
				{
					DEBUG_LOG("对比失败，请重新录入指纹\r\n");
					ShowErrMessage(ensure);
					i=0;
					processnum=0;//跳回第一步		
				}
				delay_ms(1200);
				break;

			case 3:
				DEBUG_LOG("生成指纹模板\r\n");
				ensure=PS_RegModel();
				if(ensure==0x00) 
				{
					DEBUG_LOG("生成指纹模板成功\r\n");
					processnum=4;//跳到第五步
				}else {processnum=0;ShowErrMessage(ensure);}
				delay_ms(1200);
				break;
				
			case 4:	
				DEBUG_LOG("储存指纹ID\r\n");
				DEBUG_LOG("0=< ID <=299\r\n");
				do
					ID=FR_ID;
				while(!(ID<AS608Para.PS_max));//输入ID必须小于指纹容量的最大值
				ensure=PS_StoreChar(CharBuffer2,ID);//储存模板
				if(ensure==0x00) 
				{					
					DEBUG_LOG("录入指纹成功\r\n");
					OLED_Clear();
					OLED_WriteCN(32,1,14,6);    //显示->请按指纹
					delay_s(1);
					PS_ValidTempleteNum(&ValidN);//读库指纹个数	
					return ;
				}else {processnum=0;ShowErrMessage(ensure);}					
				break;				
		}
		delay_ms(1000);
		if(i==5)//超过5次没有按手指则退出
		{
			DEBUG_LOG("超过5次没有按手指则退出\r\n");
			break;	
		}				
	}
}

//删除指纹
void Del_FR(u16 FR_ID)	/*输入删除指纹ID*/
{
	
	u8  ensure;
				OLED_Clear();
				OLED_WriteCN(6,2,17,5);	
				delay_ms(500);
				OLED_ShowChar(6,12,'.');
				delay_ms(500);
				OLED_ShowChar(6,13,'.');
				delay_ms(500);
				OLED_ShowChar(6,14,'.');
				delay_ms(500);
				OLED_ShowChar(6,15,'.');	
	DEBUG_LOG("删除指纹\r\n");
	ensure=PS_DeletChar(FR_ID,1);//删除单个指纹
	if(ensure==0)
	{
		DEBUG_LOG("删除指纹成功\r\n");		
	}
  else
		ShowErrMessage(ensure);	
	delay_ms(1200);
	PS_ValidTempleteNum(&ValidN);//读库指纹个数
  				OLED_Clear();
				OLED_WriteCN(6,2,18,6);	
				delay_ms(500);				
				main_interface();	//解锁后的主界面	

}

void Check_FR(void) //检测到有指纹 启动识别验证开锁
{
	if(PS_Sta)	 //检测PS_Sta状态，如果有手指按下
	{	
			OLED_Clear();
			OLED_WriteCN(6,2,4,5);	
			delay_ms(500);
			OLED_ShowChar(6,12,'.');
			delay_ms(500);
			OLED_ShowChar(6,13,'.');
			delay_ms(500);
			OLED_ShowChar(6,14,'.');
			delay_ms(500);
			OLED_ShowChar(6,15,'.');
			while(1)
			{
					SearchResult seach;
					u8 ensure;
					ensure=PS_GetImage();
					if(ensure==0x00)//获取图像成功 
					{	

								ensure=PS_GenChar(CharBuffer1);
								if(ensure==0x00) //生成特征成功
								{		
											ensure=PS_HighSpeedSearch(CharBuffer1,0,AS608Para.PS_max,&seach);
											if(ensure==0x00)//搜索成功
											{	
														
														unlock_interface();
														
														break;
											}
											
											else
											{
													char buf[25] = {""};
		
											//判断标志 才进行错误次数增加减少
											Fail_Num ++;
											sprintf(buf,"fail num: %d",Fail_Num);
											OLED_ShowString(2,1,buf);
												delay_ms(500);
												OLED_Clear();
												ShowErrMessage(ensure);		
												break;
											
											}												
																
								}
								else{
									ShowErrMessage(ensure);
									break;
								
								}
											

					}

			}
			//delay_ms(100);	

		

}
}

