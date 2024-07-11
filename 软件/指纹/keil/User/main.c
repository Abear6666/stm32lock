//-------------2022-10-1       指纹小项目------------//
//     使用AS608的录指纹、刷指纹、和删除指纹功能       //
//                       指纹锁              --- 林瑾//


#include "stm32f10x.h" //STM32头文件
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "usart2.h"
#include "as608.h"
#include "sys.h"
#include "timer.h"
#include <stdio.h>
#include "OLED_I2C.h"
#include "string.h"	 
#include "BEEP.h"
#include "led.h"
#include "flash.h"
#include "NVIC.h"
#include "SG90.h"
#include "BEEP.h"
#define KEY1  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//读取按键
#define KEY2  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)//读取按键1
#define KEY3  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)//读取按键2  
#define KEY4  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)//读取按键2
//-----------一些定义----------//

u8 tmp_buf[33];	
u16 ValidN;            //模块内有效模板个数
u16 Finger_Num=123;
u16 Set_flag=0;
u16 Add_flag=0;
u16 Del_flag=0;
u16 Pre_flag=0;
u16 Choose_flag=0;
u16 Del_Choose_flag=0;
u16 Del_Move_flag=0;
u16 ID=0;    //存储的顺序
u16 ID_flag=0;
u16 Del_num=0;
u16 One_flag=0;
u16 end;
u16 Continue;
u16 mark=1;
u16 mark1=0;      //出刷指纹标志位
#define FLASH_START_ADDR  0x0801f000	  //写入的起始地址

//-----------------------------//


//------------添加指纹过程中自检退出程序------------//
//------------刷指纹过程中自检退出程序--------------//
//void check_Add(u16 I){
//	if(I==0){
//		
//	}
//}

//void check_Press(u16 I){
//	if(I==0){
//		
//	}
//}
//--------------------------------------------------//
//------------与AS608握手------------//

void fig_init()   
{
	 while(PS_HandShake(&AS608Addr))  //与AS608模块握手,需要自检成功才能退出
	 {
		 DelayMs(400);         //指纹工作正常，需要0.4秒缓冲
		 OLED_CLS();
		 OLED_ShowStr(30,3,"AS608_OFF",2);//自检错误显示，位置已调好
		 DelayMs(300);	  
		}
	 //DelayMs(100);
	 OLED_CLS();
	 OLED_ShowStr(30,3,"AS608_ON",2);//握手成功显示
	 DelayMs(500);
	 BEEP_ON( );
}

//---------------------------------------//

//------------ID输出测试------------------//
void NUM_Print(u16 ID){
	switch(ID){
		
		case 0: OLED_ShowStr(36,3,"NUM : 0",2);break;
		case 1: OLED_ShowStr(36,3,"NUM : 1",2);break;
		case 2: OLED_ShowStr(36,3,"NUM : 2",2);break;
		case 3: OLED_ShowStr(36,3,"NUM : 3",2);break;
		case 4: OLED_ShowStr(36,3,"NUM : 4",2);break;
	    case 5: OLED_ShowStr(36,3,"NUM : 5",2);break;
	    case 6: OLED_ShowStr(36,3,"NUM : 6",2);break;
		case 7: OLED_ShowStr(36,3,"NUM : 7",2);break;
		case 8: OLED_ShowStr(36,3,"NUM : 8",2);break;
		case 9: OLED_ShowStr(36,3,"NUM : 9",2);break;
		case 10: OLED_ShowStr(36,3,"NUM : 10",2);break;
		case 11: OLED_ShowStr(36,3,"NUM : 11",2);break;
	    case 12: OLED_ShowStr(36,3,"NUM : 12",2);break;
	    case 13: OLED_ShowStr(36,3,"NUM : 13",2);break;
		case 14: OLED_ShowStr(36,3,"NUM : 14",2);break;
	    case 15: OLED_ShowStr(36,3,"NUM : 15",2);break;
		case 16: OLED_ShowStr(36,3,"NUM : 16",2);break;
	    case 17: OLED_ShowStr(36,3,"NUM : 17",2);break;
		case 18: OLED_ShowStr(36,3,"NUM : 18",2);break;
	    case 19: OLED_ShowStr(36,3,"NUM : 19",2);break;
		case 20: OLED_ShowStr(36,3,"NUM : 20",2);break;
	    case 21: OLED_ShowStr(36,3,"NUM : 21",2);break;
		case 22: OLED_ShowStr(36,3,"NUM : 22",2);break;
	    case 23: OLED_ShowStr(36,3,"NUM : 23",2);break;
		default:  OLED_ShowStr(25,3,"ID : FULL",2);break;
	}
}


//---------------------------------------//
//---------------添加指纹-----------------//

void Add_FR(void){
	u8 i=0,ensure ,processnum=0;
	while(Add_flag==0){
					goto end;        //高频次扫描，扫描是否返回主菜单
	           }
	while(1){
		switch(processnum){
			case 0:      //第一次指纹输入
				i++;
				OLED_CLS();
			    DelayMs(100);
				OLED_ShowStr(23,3,"Put Finger",2);
				while(Add_flag==0){
					goto end;     //高频次扫描，扫描是否返回主菜单
	           }
			    while(1){   //检测到有指纹按下后继续执行
					while(Add_flag==0){
					goto end;     //高频次扫描，扫描是否返回主菜单
	           }
					if(Have==1) break;
				}
				Have=0;
				ensure=PS_GetImage();
				while(Add_flag==0){
					goto end;     //高频次扫描
	           }
				OLED_CLS();
				DelayMs(100);
			   while(Add_flag==0){
					goto end;     //高频次扫描
	           }
				if(ensure==0x00){
					OLED_ShowStr(22,3,"FingerGet 1",2);//获取到指纹
					 DelayMs(500);
					while(Add_flag==0){
					goto end;     //高频次扫描
	           }
					ensure=PS_GenChar(CharBuffer1);//生成特征，放于CharBuffer1
					if(ensure==0x00){
						while(Add_flag==0){
					goto end;     //高频次扫描
	           }
						OLED_ShowStr(22,3,"Finger Ok-1",2);//指纹正确,可继续
						while(Add_flag==0){
					goto end;     //高频次扫描
	           }
						processnum=1;//跳到第二步
						DelayMs(400);
				   while(Add_flag==0){
						goto end;     //高频次扫描
				   }
					   }else {
						   OLED_ShowStr(25,3,"Finger ERROR",2);//指纹有误				
				}
				}else {
					DelayMs(100);
					OLED_ShowStr(35,3,"NOT GET",2);   //指纹未获取
					while(Add_flag==0){
					goto end;     //高频次扫描
	           }
				  } 
                break;	
             case 1:       //第二次指纹输入
                i++;
				OLED_CLS();
				OLED_ShowStr(18, 3,"Finger Again",2);//再按一次手指
				 while(Add_flag==0){
						goto end;     //高频次扫描
				   }
			    while(1){       //检测到有指纹按下后继续执行
					while(Add_flag==0){
					goto end;     //高频次扫描，扫描是否返回主菜单
	           }
					if(Have==1) break;
				}
				Have=0;
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					DelayMs(200);
					OLED_CLS();
					OLED_ShowStr(22, 3,"FingerGet 2",2);  //获取到手指
					while(Add_flag==0){
					goto end;     //高频次扫描
	           }
					DelayMs(300);
					OLED_CLS();
					ensure=PS_GenChar(CharBuffer2);  //生成特征			
					if(ensure==0x00)
					{
							while(Add_flag==0){
						goto end;     //高频次扫描
				   }
						OLED_ShowStr(22, 3,"Finger OK-2",2);//指纹正确					
						i=0;
						GPIO_SetBits(GPIOB,GPIO_Pin_1); //LED灯为低电平（1）
						DelayMs(1000);
						GPIO_ResetBits(GPIOB,GPIO_Pin_1); //LED灯为低电平（1）
						processnum=2;//跳到第三步
					}else OLED_ShowStr(25, 3,"Finger ERROR",2);	 //指纹错误
				}else OLED_ShowStr(35, 3,"NOT GET",2);  //未找到指纹	
				break;
				
            case 2:          //两次指纹比较
				OLED_CLS();
				OLED_ShowStr(13, 3,"Finger Comper",2);//对比两次指纹
				while(Add_flag==0){
						goto end;     //高频次扫描
				   }
			    DelayMs(500);
				OLED_CLS();
				ensure=PS_Match();
				if(ensure==0x00) 
				{
					OLED_ShowStr(30, 3,"Match-OK",2);//两次指纹是一样的
					while(Add_flag==0){
					goto end;     //高频次扫描
	           }
					DelayMs(500);				
					processnum=3;//跳到第四步
				}
				else 
				{
					OLED_ShowStr(10, 2,"Finger Match NO",2);
					while(Add_flag==0){
					goto end;     //高频次扫描
	           }
					DelayMs(200);
					OLED_CLS();
					OLED_ShowStr(10, 4,"Back To Step 1",2);
					DelayMs(100);
					i=0;
					processnum=0;//跳回第一步		
				}
				DelayMs(1000);
				break;	
				
			case 3:       //产生一个模板
				OLED_CLS();
				OLED_ShowStr(25, 3,"Creat Mode",2);//产生一个指纹模板
				while(Add_flag==0){
						goto end;     //高频次扫描
				   }
			    DelayMs(500);
				OLED_CLS();
				ensure=PS_RegModel();
				if(ensure==0x00) 
				{
					OLED_CLS();		
					OLED_ShowStr(17, 3,"CreatSuccess",2);//生成指纹模板成功
					DelayMs(100);
					while(Add_flag==0){
					goto end;     //高频次扫描
	           }
					processnum=4;//跳到第五步
				}else {processnum=0;OLED_ShowStr(15, 3,"Creat Unsccess",2);}
				DelayMs(1000);
				break;	
				
		    case 4:     //存储模板
				OLED_CLS();				
				PS_ValidTempleteNum(&ValidN);//读库指纹个数
				ID=ValidN;
//				NUM_Print(ValidN);
			    DelayMs(200);
                OLED_CLS();			
			    DelayMs(300);
					ensure=PS_StoreChar(CharBuffer2,ID);//储存模板
					if(ensure==0x00) 
					{	
						
						OLED_ShowStr(8, 3,"Add Fingger Ok",2);//添加指纹成功
						DelayMs(300);
						while(Add_flag==0){
					       goto end;     //高频次扫描
	                      }
						OLED_CLS();		
//						OLED_ShowStr(20, 3,"Left ID num:",2);//添加指纹成功
						PS_ValidTempleteNum(&ValidN);//读库指纹个数
						ID=ValidN;
				        NUM_Print(ID);
						DelayMs(1000);
						ID_flag=0;//ID_flag归0
						ID++;//指纹增加一个
						
						FLASH_W(FLASH_START_ADDR,ID);  //flash保存ID的值，等下次用
						Set_flag=1;
						Add_flag=0;
						Del_flag=0;
						return ;
					}else {while(Add_flag==0){
							goto end;     //高频次扫描
							}processnum=0;OLED_ShowStr(10, 3,"Add Fingger NO",2);}					
					break;				
       }
		DelayMs(500);	
	}
	end:
	OLED_CLS();
	return;
}

//--------------------------------------//

//----------------刷指纹----------------//

void press_fr(void){
	SearchResult seach;
	u8 ensure;
	char *str;
	Continue=0;
	OLED_CLS();
	OLED_ShowStr(15, 3, "Press Finger",2);  //提示可以刷指纹了
	while(Pre_flag==0){
					goto end1;    //高频次扫描
	           }
	while(1){                     //检测到有指纹按下后继续执行
			 while(Pre_flag==0){
					goto end1;
	           }	 
		     if(Have==1) break;
			}
	Have=0;
	ensure=PS_GetImage();
	while(Pre_flag==0){
			goto end1;
	   }
	if(ensure==0x00)//获取要刷的图像成功 
	{	
		OLED_CLS();	
		while(Pre_flag==0){
					goto end1;    //高频次扫描
	           }
		DelayMs(300);
		OLED_ShowStr(23,3,"Finger Get",2);//获取到指纹
	   while(Pre_flag==0){
			goto end1;    //高频次扫描，扫描返回菜单是否按下
	   }
		ensure=PS_GenChar(CharBuffer1);
		if(ensure==0x00) //生成要刷的图像特征成功
		{	
            while(Pre_flag==0){
					goto end1;    //高频次扫描
	           }		
			ensure=PS_HighSpeedSearch(CharBuffer1,0,300,&seach);
			if(ensure==0x00)//搜索成功
			{
				OLED_CLS();		
				OLED_ShowStr(23, 3, "Find Finger",2);//搜索指纹成功
				while(Pre_flag==0){
					goto end1;    //高频次扫描
	           }
				DelayMs(500);
			   while(Pre_flag==0){
					goto end1;    //高频次扫描
	           }
				OLED_CLS();	
				OLED_ShowStr(35, 3, "PASS!",2);
			    Continue=1;
			    BEEP_ON( );
			    if(mark==1){
			    steer();
				}
			   while(Pre_flag==0){
					goto end1;    //高频次扫描
	           }
			}
			else
			{				
				OLED_ShowStr(15, 3, "Unknow Finger",2);//未搜索到指纹
				while(Pre_flag==0){
					goto end1;    //高频次扫描
	           }
			}				
	  }
		else{
			OLED_ShowStr(20, 3, "Create Fail",2);//生成失败
			while(Pre_flag==0){
						goto end1;    //高频次扫描
				   }
			   }
		end1:
		Pre_flag=0; //刷指纹标志位置为0
		OLED_CLS();
		return ;
	}

}


//----------------------------------------------//



//-----------------删除指纹---------------//
void Del_FR(void)
{
	u8  ensure;
	
//	OLED_Show_Str(20, 0, "Del Finger",16);//显示删除指纹
//	OLED_Show_Str(0,20,"Single_ID:",16);//显示输入ID并按下“Enter”
//	OLED_Show_Str(0,40,"Hole___ID:",16);//显示输入ID并按下“Enter”
//	DelayMs(50);
//	
//	if(!Del_Move_flag)
//	{
//		OLED_Show_Str(80,20,"<<-",16);//显示输入ID并按下“Enter”
//		OLED_Show_Str(80,40,"   ",16);//
//	}
//	else
//	{
//		OLED_Show_Str(80,20,"   ",16);//
//		OLED_Show_Str(80,40,"<<-",16);//
//	}
//	if(Del_Choose_flag==1)
//		{
//			ensure=PS_DeletChar(Del_num,1);//删除单个指纹,从Del_num开始的N个模板
//			Del_flag=0;
//			}
//		else if(Del_Choose_flag==2)
//		{
					ensure=PS_Empty();//清空指纹库
					Del_flag=0;
//		}
//		
//		
	if(ensure==0)
	{	
		OLED_CLS();
		OLED_ShowStr(26, 3, "Delete OK",2);//删除指纹成功	
		DelayMs(500);
		ensure=201;
		Del_num++;
		Set_flag=1;
		Add_flag=0;
		Del_flag=0;
		return;
	}
//  else
//		OLED_Show_Str(10, 40, "Del Finger NO",16);	
	DelayMs(500);//延时后清除显示
}



//------------------------------------//


//--------------主函数----------------//
int main (void){//主程序
	unsigned char i;
	int j,m=128;
	extern const unsigned char BMP1[];
	u16 angle;
    RCC_Configuration(); //时钟设置
	LED_Init();             //各种初始化
	DelayInit();
	I2C_Configuration();
	OLED_Init();
	uart_Init(9600); 
	usart2_Init(9600);    //AS608波特率为9600
	PS_StaGPIO_Init();         //初始化PA6   与TCH相连的引脚，高电平代表触摸屏感应   
	Infrared_INT_INIT();  //按键中断初始化 
	SG90_Init(59999,23);    //舵机初始化
	BEEP_Init();          //蜂鸣器初始化
	OLED_Fill(0xFF);    //全屏点亮
	DelayMs(200);
	OLED_Fill(0x00);    //全屏灭
	OLED_CLS();         //清屏
	fig_init();    //进行握手，也就是自检，成功后继续
	DelayMs(50);
	OLED_CLS();         //清屏
	while(1){
		
//		for(m=50;m>0;m--){
//		OLED_ShowStr(m,3,"Main Menu",2);
//		DelayUs(10);
//		OLED_CLS();
//		}
		DelayMs(300); 
		while((Pre_flag==0)&&(Add_flag==0)){
			OLED_ShowStr(23,3,"Main  Menu",2);
		}
		if(m==0){  m=50; }
		//---------------刷指纹模式-------------------//
		while(Pre_flag==1 && Add_flag==0)
		{
			OLED_CLS();
			DelayMs(100);
			press_fr();        //刷指纹
		    Pre_flag=0;   //标志去除
			
		}
	//------------------- 添加模式-------------------//
		while(Add_flag==1 && Pre_flag==0)
		{
			
			OLED_CLS();
			DelayMs(100);
			OLED_ShowStr(15,3,"verification",2);
			DelayMs(300);
			mark=0;
			OLED_CLS();
			while((Continue==0) && (mark1==0)){
			Pre_flag=1;        //满足刷指纹的条件
			Add_flag=0;       //如上
			press_fr();        //添加之前先按指纹验证
			}
			mark1==0;
			mark=1;
			Pre_flag=0;        //标志去除
			Add_flag=1; 
			Add_FR();     
			Add_flag==0; //添加指纹标志去除
		}
		
	}
    //PS_Empty();	//要清指纹的时候用
						
      }

//----------------按键中断函数----------------//
	
void  EXTI0_IRQHandler(void){
	if(EXTI_GetITStatus(EXTI_Line0)!=RESET){    //按一键可以刷指纹
//		OLED_CLS();
//		OLED_ShowStr(40,3,"PRESS",2);
		EXTI_ClearITPendingBit(EXTI_Line0);
		Pre_flag=0;
		DelayMs(10);
//		OLED_CLS();
        if(KEY1==0){
		Pre_flag=1;
		}
	}
	return;
}


void  EXTI1_IRQHandler(void){
	if(EXTI_GetITStatus(EXTI_Line1)!=RESET){    //按二键可以加指纹
		Add_flag=0; 
		EXTI_ClearITPendingBit(EXTI_Line1);
		DelayMs(10);
//		OLED_CLS();
		if(KEY2==0){
	    Add_flag=1;      //进行添加标志位
		}
	}
	return;
}

void  EXTI4_IRQHandler(void){
	if(EXTI_GetITStatus(EXTI_Line4)!=RESET){    //按三键可以加回到主菜单
//		OLED_CLS();
//		OLED_ShowStr(42,3," ADD ",2);
		EXTI_ClearITPendingBit(EXTI_Line4);
		DelayMs(10);
		if(KEY3==0){
//			OLED_CLS();
			Add_flag=0;      //进行添加标志位
			Pre_flag=0;
			mark1=1;        //打断添加指纹中的刷指纹
		}
	}
	return;
}
	
	
	
//-------------------------------------------//





















	
	
	
	
//--------------------额外--------------------//
/*
	//OLED_DrawBMP(0,0,128,8,(unsigned char *)BMP1);//测试BMP位图显示
	
	//		for(i=0;i<5;i++)
    //		{
	//			OLED_ShowCN(22+i*16,0,i);//测试显示中文
	//		}
	//		DelayS(1);
	//		OLED_ShowStr(44,3,"OK",2);    //测试6*8字符  //第二个是上下行，0为最上，3就是中间行
	                                      //第一个是格数，对应列位置，60差不多从中间开始//第四个是字体，有1号和2号的，2号较大
	
	//GPIO_SetBits(GPIOB,GPIO_Pin_1); //LED灯都为高电平（1）
	
	//		OLED_OFF();//测试OLED休眠
    //		OLED_ON();//测试OLED休眠后唤醒
*/
