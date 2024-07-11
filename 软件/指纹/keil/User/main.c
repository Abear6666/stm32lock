//-------------2022-10-1       ָ��С��Ŀ------------//
//     ʹ��AS608��¼ָ�ơ�ˢָ�ơ���ɾ��ָ�ƹ���       //
//                       ָ����              --- ���//


#include "stm32f10x.h" //STM32ͷ�ļ�
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
#define KEY1  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//��ȡ����
#define KEY2  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)//��ȡ����1
#define KEY3  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)//��ȡ����2  
#define KEY4  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)//��ȡ����2
//-----------һЩ����----------//

u8 tmp_buf[33];	
u16 ValidN;            //ģ������Чģ�����
u16 Finger_Num=123;
u16 Set_flag=0;
u16 Add_flag=0;
u16 Del_flag=0;
u16 Pre_flag=0;
u16 Choose_flag=0;
u16 Del_Choose_flag=0;
u16 Del_Move_flag=0;
u16 ID=0;    //�洢��˳��
u16 ID_flag=0;
u16 Del_num=0;
u16 One_flag=0;
u16 end;
u16 Continue;
u16 mark=1;
u16 mark1=0;      //��ˢָ�Ʊ�־λ
#define FLASH_START_ADDR  0x0801f000	  //д�����ʼ��ַ

//-----------------------------//


//------------���ָ�ƹ������Լ��˳�����------------//
//------------ˢָ�ƹ������Լ��˳�����--------------//
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
//------------��AS608����------------//

void fig_init()   
{
	 while(PS_HandShake(&AS608Addr))  //��AS608ģ������,��Ҫ�Լ�ɹ������˳�
	 {
		 DelayMs(400);         //ָ�ƹ�����������Ҫ0.4�뻺��
		 OLED_CLS();
		 OLED_ShowStr(30,3,"AS608_OFF",2);//�Լ������ʾ��λ���ѵ���
		 DelayMs(300);	  
		}
	 //DelayMs(100);
	 OLED_CLS();
	 OLED_ShowStr(30,3,"AS608_ON",2);//���ֳɹ���ʾ
	 DelayMs(500);
	 BEEP_ON( );
}

//---------------------------------------//

//------------ID�������------------------//
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
//---------------���ָ��-----------------//

void Add_FR(void){
	u8 i=0,ensure ,processnum=0;
	while(Add_flag==0){
					goto end;        //��Ƶ��ɨ�裬ɨ���Ƿ񷵻����˵�
	           }
	while(1){
		switch(processnum){
			case 0:      //��һ��ָ������
				i++;
				OLED_CLS();
			    DelayMs(100);
				OLED_ShowStr(23,3,"Put Finger",2);
				while(Add_flag==0){
					goto end;     //��Ƶ��ɨ�裬ɨ���Ƿ񷵻����˵�
	           }
			    while(1){   //��⵽��ָ�ư��º����ִ��
					while(Add_flag==0){
					goto end;     //��Ƶ��ɨ�裬ɨ���Ƿ񷵻����˵�
	           }
					if(Have==1) break;
				}
				Have=0;
				ensure=PS_GetImage();
				while(Add_flag==0){
					goto end;     //��Ƶ��ɨ��
	           }
				OLED_CLS();
				DelayMs(100);
			   while(Add_flag==0){
					goto end;     //��Ƶ��ɨ��
	           }
				if(ensure==0x00){
					OLED_ShowStr(22,3,"FingerGet 1",2);//��ȡ��ָ��
					 DelayMs(500);
					while(Add_flag==0){
					goto end;     //��Ƶ��ɨ��
	           }
					ensure=PS_GenChar(CharBuffer1);//��������������CharBuffer1
					if(ensure==0x00){
						while(Add_flag==0){
					goto end;     //��Ƶ��ɨ��
	           }
						OLED_ShowStr(22,3,"Finger Ok-1",2);//ָ����ȷ,�ɼ���
						while(Add_flag==0){
					goto end;     //��Ƶ��ɨ��
	           }
						processnum=1;//�����ڶ���
						DelayMs(400);
				   while(Add_flag==0){
						goto end;     //��Ƶ��ɨ��
				   }
					   }else {
						   OLED_ShowStr(25,3,"Finger ERROR",2);//ָ������				
				}
				}else {
					DelayMs(100);
					OLED_ShowStr(35,3,"NOT GET",2);   //ָ��δ��ȡ
					while(Add_flag==0){
					goto end;     //��Ƶ��ɨ��
	           }
				  } 
                break;	
             case 1:       //�ڶ���ָ������
                i++;
				OLED_CLS();
				OLED_ShowStr(18, 3,"Finger Again",2);//�ٰ�һ����ָ
				 while(Add_flag==0){
						goto end;     //��Ƶ��ɨ��
				   }
			    while(1){       //��⵽��ָ�ư��º����ִ��
					while(Add_flag==0){
					goto end;     //��Ƶ��ɨ�裬ɨ���Ƿ񷵻����˵�
	           }
					if(Have==1) break;
				}
				Have=0;
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					DelayMs(200);
					OLED_CLS();
					OLED_ShowStr(22, 3,"FingerGet 2",2);  //��ȡ����ָ
					while(Add_flag==0){
					goto end;     //��Ƶ��ɨ��
	           }
					DelayMs(300);
					OLED_CLS();
					ensure=PS_GenChar(CharBuffer2);  //��������			
					if(ensure==0x00)
					{
							while(Add_flag==0){
						goto end;     //��Ƶ��ɨ��
				   }
						OLED_ShowStr(22, 3,"Finger OK-2",2);//ָ����ȷ					
						i=0;
						GPIO_SetBits(GPIOB,GPIO_Pin_1); //LED��Ϊ�͵�ƽ��1��
						DelayMs(1000);
						GPIO_ResetBits(GPIOB,GPIO_Pin_1); //LED��Ϊ�͵�ƽ��1��
						processnum=2;//����������
					}else OLED_ShowStr(25, 3,"Finger ERROR",2);	 //ָ�ƴ���
				}else OLED_ShowStr(35, 3,"NOT GET",2);  //δ�ҵ�ָ��	
				break;
				
            case 2:          //����ָ�ƱȽ�
				OLED_CLS();
				OLED_ShowStr(13, 3,"Finger Comper",2);//�Ա�����ָ��
				while(Add_flag==0){
						goto end;     //��Ƶ��ɨ��
				   }
			    DelayMs(500);
				OLED_CLS();
				ensure=PS_Match();
				if(ensure==0x00) 
				{
					OLED_ShowStr(30, 3,"Match-OK",2);//����ָ����һ����
					while(Add_flag==0){
					goto end;     //��Ƶ��ɨ��
	           }
					DelayMs(500);				
					processnum=3;//�������Ĳ�
				}
				else 
				{
					OLED_ShowStr(10, 2,"Finger Match NO",2);
					while(Add_flag==0){
					goto end;     //��Ƶ��ɨ��
	           }
					DelayMs(200);
					OLED_CLS();
					OLED_ShowStr(10, 4,"Back To Step 1",2);
					DelayMs(100);
					i=0;
					processnum=0;//���ص�һ��		
				}
				DelayMs(1000);
				break;	
				
			case 3:       //����һ��ģ��
				OLED_CLS();
				OLED_ShowStr(25, 3,"Creat Mode",2);//����һ��ָ��ģ��
				while(Add_flag==0){
						goto end;     //��Ƶ��ɨ��
				   }
			    DelayMs(500);
				OLED_CLS();
				ensure=PS_RegModel();
				if(ensure==0x00) 
				{
					OLED_CLS();		
					OLED_ShowStr(17, 3,"CreatSuccess",2);//����ָ��ģ��ɹ�
					DelayMs(100);
					while(Add_flag==0){
					goto end;     //��Ƶ��ɨ��
	           }
					processnum=4;//�������岽
				}else {processnum=0;OLED_ShowStr(15, 3,"Creat Unsccess",2);}
				DelayMs(1000);
				break;	
				
		    case 4:     //�洢ģ��
				OLED_CLS();				
				PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���
				ID=ValidN;
//				NUM_Print(ValidN);
			    DelayMs(200);
                OLED_CLS();			
			    DelayMs(300);
					ensure=PS_StoreChar(CharBuffer2,ID);//����ģ��
					if(ensure==0x00) 
					{	
						
						OLED_ShowStr(8, 3,"Add Fingger Ok",2);//���ָ�Ƴɹ�
						DelayMs(300);
						while(Add_flag==0){
					       goto end;     //��Ƶ��ɨ��
	                      }
						OLED_CLS();		
//						OLED_ShowStr(20, 3,"Left ID num:",2);//���ָ�Ƴɹ�
						PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���
						ID=ValidN;
				        NUM_Print(ID);
						DelayMs(1000);
						ID_flag=0;//ID_flag��0
						ID++;//ָ������һ��
						
						FLASH_W(FLASH_START_ADDR,ID);  //flash����ID��ֵ�����´���
						Set_flag=1;
						Add_flag=0;
						Del_flag=0;
						return ;
					}else {while(Add_flag==0){
							goto end;     //��Ƶ��ɨ��
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

//----------------ˢָ��----------------//

void press_fr(void){
	SearchResult seach;
	u8 ensure;
	char *str;
	Continue=0;
	OLED_CLS();
	OLED_ShowStr(15, 3, "Press Finger",2);  //��ʾ����ˢָ����
	while(Pre_flag==0){
					goto end1;    //��Ƶ��ɨ��
	           }
	while(1){                     //��⵽��ָ�ư��º����ִ��
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
	if(ensure==0x00)//��ȡҪˢ��ͼ��ɹ� 
	{	
		OLED_CLS();	
		while(Pre_flag==0){
					goto end1;    //��Ƶ��ɨ��
	           }
		DelayMs(300);
		OLED_ShowStr(23,3,"Finger Get",2);//��ȡ��ָ��
	   while(Pre_flag==0){
			goto end1;    //��Ƶ��ɨ�裬ɨ�践�ز˵��Ƿ���
	   }
		ensure=PS_GenChar(CharBuffer1);
		if(ensure==0x00) //����Ҫˢ��ͼ�������ɹ�
		{	
            while(Pre_flag==0){
					goto end1;    //��Ƶ��ɨ��
	           }		
			ensure=PS_HighSpeedSearch(CharBuffer1,0,300,&seach);
			if(ensure==0x00)//�����ɹ�
			{
				OLED_CLS();		
				OLED_ShowStr(23, 3, "Find Finger",2);//����ָ�Ƴɹ�
				while(Pre_flag==0){
					goto end1;    //��Ƶ��ɨ��
	           }
				DelayMs(500);
			   while(Pre_flag==0){
					goto end1;    //��Ƶ��ɨ��
	           }
				OLED_CLS();	
				OLED_ShowStr(35, 3, "PASS!",2);
			    Continue=1;
			    BEEP_ON( );
			    if(mark==1){
			    steer();
				}
			   while(Pre_flag==0){
					goto end1;    //��Ƶ��ɨ��
	           }
			}
			else
			{				
				OLED_ShowStr(15, 3, "Unknow Finger",2);//δ������ָ��
				while(Pre_flag==0){
					goto end1;    //��Ƶ��ɨ��
	           }
			}				
	  }
		else{
			OLED_ShowStr(20, 3, "Create Fail",2);//����ʧ��
			while(Pre_flag==0){
						goto end1;    //��Ƶ��ɨ��
				   }
			   }
		end1:
		Pre_flag=0; //ˢָ�Ʊ�־λ��Ϊ0
		OLED_CLS();
		return ;
	}

}


//----------------------------------------------//



//-----------------ɾ��ָ��---------------//
void Del_FR(void)
{
	u8  ensure;
	
//	OLED_Show_Str(20, 0, "Del Finger",16);//��ʾɾ��ָ��
//	OLED_Show_Str(0,20,"Single_ID:",16);//��ʾ����ID�����¡�Enter��
//	OLED_Show_Str(0,40,"Hole___ID:",16);//��ʾ����ID�����¡�Enter��
//	DelayMs(50);
//	
//	if(!Del_Move_flag)
//	{
//		OLED_Show_Str(80,20,"<<-",16);//��ʾ����ID�����¡�Enter��
//		OLED_Show_Str(80,40,"   ",16);//
//	}
//	else
//	{
//		OLED_Show_Str(80,20,"   ",16);//
//		OLED_Show_Str(80,40,"<<-",16);//
//	}
//	if(Del_Choose_flag==1)
//		{
//			ensure=PS_DeletChar(Del_num,1);//ɾ������ָ��,��Del_num��ʼ��N��ģ��
//			Del_flag=0;
//			}
//		else if(Del_Choose_flag==2)
//		{
					ensure=PS_Empty();//���ָ�ƿ�
					Del_flag=0;
//		}
//		
//		
	if(ensure==0)
	{	
		OLED_CLS();
		OLED_ShowStr(26, 3, "Delete OK",2);//ɾ��ָ�Ƴɹ�	
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
	DelayMs(500);//��ʱ�������ʾ
}



//------------------------------------//


//--------------������----------------//
int main (void){//������
	unsigned char i;
	int j,m=128;
	extern const unsigned char BMP1[];
	u16 angle;
    RCC_Configuration(); //ʱ������
	LED_Init();             //���ֳ�ʼ��
	DelayInit();
	I2C_Configuration();
	OLED_Init();
	uart_Init(9600); 
	usart2_Init(9600);    //AS608������Ϊ9600
	PS_StaGPIO_Init();         //��ʼ��PA6   ��TCH���������ţ��ߵ�ƽ����������Ӧ   
	Infrared_INT_INIT();  //�����жϳ�ʼ�� 
	SG90_Init(59999,23);    //�����ʼ��
	BEEP_Init();          //��������ʼ��
	OLED_Fill(0xFF);    //ȫ������
	DelayMs(200);
	OLED_Fill(0x00);    //ȫ����
	OLED_CLS();         //����
	fig_init();    //�������֣�Ҳ�����Լ죬�ɹ������
	DelayMs(50);
	OLED_CLS();         //����
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
		//---------------ˢָ��ģʽ-------------------//
		while(Pre_flag==1 && Add_flag==0)
		{
			OLED_CLS();
			DelayMs(100);
			press_fr();        //ˢָ��
		    Pre_flag=0;   //��־ȥ��
			
		}
	//------------------- ���ģʽ-------------------//
		while(Add_flag==1 && Pre_flag==0)
		{
			
			OLED_CLS();
			DelayMs(100);
			OLED_ShowStr(15,3,"verification",2);
			DelayMs(300);
			mark=0;
			OLED_CLS();
			while((Continue==0) && (mark1==0)){
			Pre_flag=1;        //����ˢָ�Ƶ�����
			Add_flag=0;       //����
			press_fr();        //���֮ǰ�Ȱ�ָ����֤
			}
			mark1==0;
			mark=1;
			Pre_flag=0;        //��־ȥ��
			Add_flag=1; 
			Add_FR();     
			Add_flag==0; //���ָ�Ʊ�־ȥ��
		}
		
	}
    //PS_Empty();	//Ҫ��ָ�Ƶ�ʱ����
						
      }

//----------------�����жϺ���----------------//
	
void  EXTI0_IRQHandler(void){
	if(EXTI_GetITStatus(EXTI_Line0)!=RESET){    //��һ������ˢָ��
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
	if(EXTI_GetITStatus(EXTI_Line1)!=RESET){    //���������Լ�ָ��
		Add_flag=0; 
		EXTI_ClearITPendingBit(EXTI_Line1);
		DelayMs(10);
//		OLED_CLS();
		if(KEY2==0){
	    Add_flag=1;      //������ӱ�־λ
		}
	}
	return;
}

void  EXTI4_IRQHandler(void){
	if(EXTI_GetITStatus(EXTI_Line4)!=RESET){    //���������Լӻص����˵�
//		OLED_CLS();
//		OLED_ShowStr(42,3," ADD ",2);
		EXTI_ClearITPendingBit(EXTI_Line4);
		DelayMs(10);
		if(KEY3==0){
//			OLED_CLS();
			Add_flag=0;      //������ӱ�־λ
			Pre_flag=0;
			mark1=1;        //������ָ���е�ˢָ��
		}
	}
	return;
}
	
	
	
//-------------------------------------------//





















	
	
	
	
//--------------------����--------------------//
/*
	//OLED_DrawBMP(0,0,128,8,(unsigned char *)BMP1);//����BMPλͼ��ʾ
	
	//		for(i=0;i<5;i++)
    //		{
	//			OLED_ShowCN(22+i*16,0,i);//������ʾ����
	//		}
	//		DelayS(1);
	//		OLED_ShowStr(44,3,"OK",2);    //����6*8�ַ�  //�ڶ����������У�0Ϊ���ϣ�3�����м���
	                                      //��һ���Ǹ�������Ӧ��λ�ã�60�����м俪ʼ//���ĸ������壬��1�ź�2�ŵģ�2�Žϴ�
	
	//GPIO_SetBits(GPIOB,GPIO_Pin_1); //LED�ƶ�Ϊ�ߵ�ƽ��1��
	
	//		OLED_OFF();//����OLED����
    //		OLED_ON();//����OLED���ߺ���
*/
