#ifndef _AS608_use_h_
#define _AS608_use_h_
#include "stm32f10x.h"   //stm32ͷ�ļ�
#include "as608.h"						/*ָ��ģ���ʼ��*/
#include "DELAY.h"						/*��ʱ����*/

void Add_FR(u16 FR_ID);						/*¼�ĸ�ID��ָ��*/
void Del_FR(u16 FR_ID);						/*ɾ���ĸ�ID��ָ��*/
void press_FR(void);							/*ˢָ��*/
void ShowErrMessage(u8 ensure);		/*��ʾȷ���������Ϣ*/
void Check_FR(void); //��⵽��ָ�� ����ʶ����֤����
void upload_fin_number(void);//��ѯָ�Ƹ������͵�С����鿴
#endif
