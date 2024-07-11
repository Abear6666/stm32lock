#include "AS608_use.h"
#include "usart.h"
#include "OLED.h"
#include "delay.h"
#include "as608.h"
#include "onenet.h"
#include "esp8266.h"

SysPara AS608Para;							/*ָ��ģ��AS608����*/
u16 ValidN;											/*ģ������Чָ�Ƹ���*/
extern void unlock_interface(void);  //��������
extern void main_interface(void);
extern u8 Fail_Num; // �Ž������������
extern const char devPubTopic[];

//��ʾȷ���������Ϣ
void ShowErrMessage(u8 ensure)
{
		
	DEBUG_LOG("\r\n������Ϣ��%s \r\n",(u8*)EnsureMessage(ensure));
	
}

//��ѯָ�Ƹ������͵�С����鿴
void upload_fin_number(void)
{
		char testbuf[125] = {""};
		u16 sum = 0;
		
		PS_ValidTempleteNum(&sum);//����ָ�Ƹ���
		sprintf(testbuf,"{\"Fin_Msg\":%d}",sum);
		DEBUG_LOG("�������� ---%s-- OneNet_Publish",testbuf);

		OneNet_Publish(devPubTopic,testbuf);
		DEBUG_LOG("==================================================================================");		
		ESP8266_Clear();		


}

//ˢָ��
void press_FR(void)
{
	SearchResult seach;
	u8 ensure;
	ensure=PS_GetImage();
	if(ensure==0x00)//��ȡͼ��ɹ� 
	{	

		ensure=PS_GenChar(CharBuffer1);
		if(ensure==0x00) //���������ɹ�
		{		
			ensure=PS_HighSpeedSearch(CharBuffer1,0,AS608Para.PS_max,&seach);
			if(ensure==0x00)//�����ɹ�
			{				
				DEBUG_LOG("ˢָ�Ƴɹ�\r\n");
				DEBUG_LOG("ȷ�д���,ID:%d  ƥ��÷�:%d",seach.pageID,seach.mathscore);
			}
			else 
				ShowErrMessage(ensure);					
	  }
		else
			ShowErrMessage(ensure);

	}
		
}

//¼ָ��
void Add_FR(u16 FR_ID)
{
		OLED_Clear();
	//��ʾ¼��ָ����
	OLED_WriteCN(16,1,16,6);    //��ʾ->��ʾ¼��ָ����
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
				DEBUG_LOG("�밴ָ��\r\n");
				OLED_Clear();
				OLED_WriteCN(32,1,19,6);    //��ʾ->�밴ָ��
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					ensure=PS_GenChar(CharBuffer1);//��������
					if(ensure==0x00)
					{
						DEBUG_LOG("ָ������\r\n");
						OLED_Clear();
						OLED_WriteCN(32,1,20,6);    //��ʾ->�밴ָ��
						delay_s(1);
						i=0;
						processnum=1;//�����ڶ���						
					}else ShowErrMessage(ensure);				
				}else ShowErrMessage(ensure);						
				break;
			
			case 1:
				i++;
				DEBUG_LOG("�밴�ٰ�һ��ָ��\r\n");
					OLED_Clear();
						OLED_WriteCN(32,1,21,6);    //��ʾ->�밴ָ��
						delay_s(1);
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					ensure=PS_GenChar(CharBuffer2);//��������
					if(ensure==0x00)
					{
						DEBUG_LOG("ָ������\r\n");
	
						OLED_Clear();
						OLED_WriteCN(32,1,20,6);    //��ʾ->�밴ָ��
						delay_s(1);
						i=0;
						processnum=2;//����������
					}else ShowErrMessage(ensure);	
				}else ShowErrMessage(ensure);		
				break;

			case 2:
				DEBUG_LOG("�Ա�����ָ��\r\n");
				ensure=PS_Match();
				if(ensure==0x00) 
				{
					DEBUG_LOG("�Աȳɹ�,����ָ��һ��\r\n");
					processnum=3;//�������Ĳ�
				}
				else 
				{
					DEBUG_LOG("�Ա�ʧ�ܣ�������¼��ָ��\r\n");
					ShowErrMessage(ensure);
					i=0;
					processnum=0;//���ص�һ��		
				}
				delay_ms(1200);
				break;

			case 3:
				DEBUG_LOG("����ָ��ģ��\r\n");
				ensure=PS_RegModel();
				if(ensure==0x00) 
				{
					DEBUG_LOG("����ָ��ģ��ɹ�\r\n");
					processnum=4;//�������岽
				}else {processnum=0;ShowErrMessage(ensure);}
				delay_ms(1200);
				break;
				
			case 4:	
				DEBUG_LOG("����ָ��ID\r\n");
				DEBUG_LOG("0=< ID <=299\r\n");
				do
					ID=FR_ID;
				while(!(ID<AS608Para.PS_max));//����ID����С��ָ�����������ֵ
				ensure=PS_StoreChar(CharBuffer2,ID);//����ģ��
				if(ensure==0x00) 
				{					
					DEBUG_LOG("¼��ָ�Ƴɹ�\r\n");
					OLED_Clear();
					OLED_WriteCN(32,1,14,6);    //��ʾ->�밴ָ��
					delay_s(1);
					PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���	
					return ;
				}else {processnum=0;ShowErrMessage(ensure);}					
				break;				
		}
		delay_ms(1000);
		if(i==5)//����5��û�а���ָ���˳�
		{
			DEBUG_LOG("����5��û�а���ָ���˳�\r\n");
			break;	
		}				
	}
}

//ɾ��ָ��
void Del_FR(u16 FR_ID)	/*����ɾ��ָ��ID*/
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
	DEBUG_LOG("ɾ��ָ��\r\n");
	ensure=PS_DeletChar(FR_ID,1);//ɾ������ָ��
	if(ensure==0)
	{
		DEBUG_LOG("ɾ��ָ�Ƴɹ�\r\n");		
	}
  else
		ShowErrMessage(ensure);	
	delay_ms(1200);
	PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���
  				OLED_Clear();
				OLED_WriteCN(6,2,18,6);	
				delay_ms(500);				
				main_interface();	//�������������	

}

void Check_FR(void) //��⵽��ָ�� ����ʶ����֤����
{
	if(PS_Sta)	 //���PS_Sta״̬���������ָ����
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
					if(ensure==0x00)//��ȡͼ��ɹ� 
					{	

								ensure=PS_GenChar(CharBuffer1);
								if(ensure==0x00) //���������ɹ�
								{		
											ensure=PS_HighSpeedSearch(CharBuffer1,0,AS608Para.PS_max,&seach);
											if(ensure==0x00)//�����ɹ�
											{	
														
														unlock_interface();
														
														break;
											}
											
											else
											{
													char buf[25] = {""};
		
											//�жϱ�־ �Ž��д���������Ӽ���
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

