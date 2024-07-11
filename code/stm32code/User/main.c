#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Servo.h"
#include "Key.h"
#include "LED.h"
#include "BEEP.h"
#include "USART.h"
#include "RC522.h"
#include "as608.h"
#include "as608_use.h"
#include "esp8266.h"
#include "onenet.h"
#include "Flash.h"
#include "DHT11.H"
#include "timer.h"
#include "servo.h"
uint8_t KeyNum;
float Angle;
extern SysPara AS608Para;							/*指纹模块AS608参数*/

extern u16 ValidN;										/*模块内有效指纹个数*/

u8 Line = 5;  //按钮上下切换的行数
extern u16 ValidN;											/*模块内有效指纹个数*/
u8 lock = 0; //开关锁状态 
u8 ESP8266_INIT_OK;//wifi初始化成功后的标志


u8 interface_logo = 1; //定义页面显示标志
u8 Led_Status; //灯标志位
u8 Power_Status; //门禁电源标志位
u8 Beep_Status; //蜂鸣器报警标志位

u8 Fail_Num; // 门禁解锁错误次数

u8 upload_card_number_flag; //发送卡号给小程序标志
u8 upload_fin_number_flag;//发送指纹个数给小程序标志


char oledBuf[128];

char PUB_BUF[256];//上传数据的buf
const char *devSubTopic[] = {"/myhome/sub"};
const char devPubTopic[] = "/myhome/pub";

u8 humidityH;	  //湿度整数部分
u8 humidityL;	  //湿度小数部分
u8 temperatureH;   //温度整数部分
u8 temperatureL;   //温度小数部分


void unlock_interface(void);  //解锁动态显示界面
void main_interface(void);  //功能主界面
void main1_interface(void);  //解锁后的主界面 第二页

void Init_ALL(void)
{
	u8 ensure;
	OLED_Init();
	Servo_Init();
	Key_Init();
	LED_Init();
	BEEP_Init();
	DHT11_Init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	USART1_Init(115200);

	RC522_IO_Init();//RC522初始化
	PcdReset();  //复位RC522
	PcdAntennaOff();  //关闭天线
	delay_ms(100);
	PcdAntennaOn();  //开启天线
	
	Read_Card(); //读取卡号信息
	
	//初始化继电器引脚
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	GPIO_SetBits(GPIOA,GPIO_Pin_8);
	Power_Status = 1;

	BEEP_ON();
	delay_ms(500);
	BEEP_OFF();
	DEBUG_LOG("初始化中\r\n");
	
	USART2_Init(115200);
	DEBUG_LOG("UART2初始化			[OK]");
	
	DEBUG_LOG("初始化ESP8266 WIFI模块...");


	if(!ESP8266_INIT_OK){
		OLED_Clear();
		sprintf(oledBuf,"Waiting For");
		OLED_ShowString(1,4,oledBuf);
		sprintf(oledBuf,"WiFi");
		OLED_ShowString(2,8,oledBuf);
		sprintf(oledBuf,"Connection");
		OLED_ShowString(3,4,oledBuf);
				
		delay_ms(500);
		OLED_ShowChar(4,12,'.');
		delay_ms(500);
		OLED_ShowChar(4,13,'.');
		delay_ms(500);
		OLED_ShowChar(4,14,'.');
		delay_ms(500);
		OLED_ShowChar(4,15,'.');
	}
	
	ESP8266_Init();					//初始化ESP8266	

	while(OneNet_DevLink()){//接入OneNET
				delay_ms(500);
	}	

	OneNet_Subscribe(devSubTopic,1);//订阅MQTT
	OneNet_Publish(devPubTopic,"mqtt test send msg");
	OLED_Clear();

	
	USART3_Init(57600);	/*初始化串口3,用于与指纹模块通讯*/
	PS_StaGPIO_Init();					/*初始化FR读状态引脚*/


	while(PS_HandShake(&AS608Addr))			/*与AS608模块握手*/
	{
		delay_ms(400);
		DEBUG_LOG("未检测到模块\r\n");
		delay_ms(1000);
		DEBUG_LOG("尝试重新连接模块\r\n"); 
	}
	DEBUG_LOG("与指纹模块握手成功！r\n");
	DEBUG_LOG("通讯成功\r\n");
	DEBUG_LOG("波特率:%d   地址:%x\r\n",57600,AS608Addr);		/*打印信息*/
	ensure=PS_ValidTempleteNum(&ValidN);										/*读库指纹个数*/
	if(ensure!=0x00)
		ShowErrMessage(ensure);								/*显示确认码错误信息*/
	ensure=PS_ReadSysPara(&AS608Para);  		/*读参数 */
	if(ensure==0x00)
	{
		DEBUG_LOG("库容量:%d     对比等级: %d",AS608Para.PS_max-ValidN,AS608Para.PS_level);
	}
	else
		ShowErrMessage(ensure);		
	
	TIM3_Int_Init(2499,7199);
	

}	

int main(void)
{
	unsigned char *dataPtr = NULL;//接收云平台的数据
	unsigned short timeCount = 0;	//发送间隔变量
	Init_ALL();//所有模块设备初始化

	while (1)
	{

		//数据回发给小程序

		if(timeCount % 60 == 0)//1500ms / 25 = 60  1.5秒执行一次
		{

			if(upload_card_number_flag)
			{
				upload_card_number(); //回发存储的卡号数据给小程序
				upload_card_number_flag = 0;
			
			}else if(upload_fin_number_flag)
			{
			
				upload_fin_number();//回发指纹个数数据给小程序
				upload_fin_number_flag = 0;
				
			
			}else{
			
			/********** 读取LED0的状态 **************/
			Led_Status = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_12);
				/********** 温湿度传感器获取数据**************/
			DHT11_Read_Data(&humidityH,&humidityL,&temperatureH,&temperatureL);
			DEBUG_LOG(" | 湿度：%d.%d C | 温度：%d.%d %%  ",humidityH,humidityL,temperatureH,temperatureL);
			
			DEBUG_LOG("==================================================================================");
			DEBUG_LOG("发布数据 ----- OneNet_Publish");
			sprintf(PUB_BUF,"{\"Hum\":%d.%d,\"Temp\":%d.%d,\"Door\":%d,\"Led\":%d,\"Beep\":%d,\"Power\":%d}",
				humidityH,humidityL,temperatureH,temperatureL,lock,Led_Status?0:1,Beep_Status,Power_Status);
			OneNet_Publish(devPubTopic, PUB_BUF);
			DEBUG_LOG("==================================================================================");
			ESP8266_Clear();			
			
			}
									
		}	
		
		if(lock)//开锁状态
		{
			KeyNum = Key_GetNum();//扫描按键
			
		if (KeyNum == 1)
		{
			
			OLED_Clear();
			if(Line <= 5)
			{
				Line = 8;
				interface_logo = 1;
				
			}else{
				Line --;
			}
			if(interface_logo == 1)
			{
					main_interface();	//解锁后的主界面	第一页
			
			}else{
					main1_interface();	//解锁后的主界面	第二页
			}
			
		}else if(KeyNum == 2)
		{
			
			OLED_Clear();
			if(Line >= 8)
			{
				Line = 5;
				interface_logo = 2;
			}else{
				Line ++;
			}
			
			
			if(interface_logo == 1)
			{
					main_interface();	//解锁后的主界面	第一页
			
			}else{
					main1_interface();	//解锁后的主界面	第二页
			}
		}else if(KeyNum == 3) //确认按键
		{
			
			if(interface_logo == 1) //界面第一页的选择确认
			{
				if(Line == 5) //灯光变换
				{
								
					LED1_Turn();
		
				}			

				if(Line == 6) //添加指纹
				{
									
					Add_FR(ValidN++);
				
		
				}				
				
							
				if(Line == 7) //删除指纹
				{
					Del_FR(--ValidN);
						
													
				}

				if(Line == 8)
				{
					close_door();
					OLED_Clear();
					lock = 0;
					continue;
									
				}	

					main_interface();
			
			
			}else if(interface_logo == 2)
			{//第二页选择确认
				if(Line == 5) //增加卡号
				{
								
					RC522_Add_card();
		
				}			

				if(Line == 6) //删除卡号
				{
					
				
					RC522_Rm_card();
		
				}				
				
							
				if(Line == 7) //打开警报
				{

					if(Beep_Status == 1)
					{
						Beep_Status = 0;
					}else{
					
						Beep_Status = 1;
					}
													
				}

				if(Line == 8) //开关电源
				{
					if (GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_8) == 0)
					{
						GPIO_SetBits(GPIOA, GPIO_Pin_8);
						Power_Status = 1;
					}
					else
					{
						GPIO_ResetBits(GPIOA, GPIO_Pin_8);
							Power_Status = 0;
					}
									
				}	
					main1_interface();		//解锁后的主界面	第二页		
			
			
			}
		
		}			
					
				
		}
		else
		{//关锁状态
			OLED_WriteCN(16,1,0,6);    //显示->智能门禁系统
			OLED_WriteCN(56,4,1,1);		 //显示锁

			sprintf(oledBuf,"T:%d.%d  H:%d.%d%%",temperatureH,temperatureL,humidityH,humidityL);
			OLED_ShowString(8,1,oledBuf);//8*16 “ABC”			
			RC522_Get_card();//检测到有刷卡 启动识别验证开锁
			Check_FR(); //检测到有指纹 启动识别验证开锁			
					
		}
		
		if(Fail_Num >=5) //门禁识别错误次数达到5次
		{
			Beep_Status= 1;	
			DEBUG_LOG("发布数据 --Warning--- OneNet_Publish");

			OneNet_Publish(devPubTopic, "{\"Warning\":1}");
			DEBUG_LOG("==================================================================================");

			ESP8266_Clear();
			Fail_Num = 0;
		}
	
		
		dataPtr = ESP8266_GetIPD(3); //获取平台返回的数据
		if(dataPtr != NULL)
			OneNet_RevPro(dataPtr);//平台返回数据检测
		delay_ms(10);
		timeCount++;
	
	}
}


void unlock_interface(void)  //解锁界面
{
//解锁界面
	if(Power_Status == 1) // 电源正常才能解锁
	{
			lock = 1;
	}else{
	
			lock = 0;
			OLED_Clear();
			OLED_ShowString(2,5,"fail:notpower!");
			delay_ms(1000);
		OLED_Clear();
		return;
	}
	
	OLED_Clear();
	LED1_ON();
	BEEP_ON();
	delay_ms(200);
	OLED_WriteCN(32,1,2,3);		 //显示已解锁
	OLED_WriteCN(56,4,1,1);		 //显示锁
	LED1_OFF();
	BEEP_OFF();	
	delay_ms(600);
	open_door();	
	OLED_WriteCN(56,4,3,1);		 //显示解锁
	delay_ms(800);	
	main_interface();	//解锁后的主界面	
	

	Fail_Num = 0; //错误次数清零
	Beep_Status = 0; //取消报警
}
 
void main_interface(void)  //解锁后的主界面
{
	OLED_Clear();
	OLED_ShowString(Line,2,"->"); // 5 6 7 8 
	OLED_WriteCN(32,0,7,4);
	OLED_WriteCN(32,2,8,4);
	OLED_WriteCN(32,4,9,4);
	OLED_WriteCN(32,6,10,4);	
}

void main1_interface(void)  //解锁后的主界面 第二页
{
	OLED_Clear();
	OLED_ShowString(Line,2,"->"); // 5 6 7 8 
	OLED_WriteCN(32,0,22,4);
	OLED_WriteCN(32,2,23,4);
	OLED_WriteCN(32,4,24,4);
	OLED_WriteCN(32,6,25,4);	
}

