

/*
//�������ҳ�Ʒ
//����ϵ�п�����Ӧ�ó���
//��ע΢�Ź��ںţ����ҵ���
//���ҿ������������� www.DoYoung.net/YT 
//������ѿ����н�ѧ��Ƶ�����ؼ������ϣ�������������
//�������ݾ��� ����������ҳ www.doyoung.net
*/

/*
���޸���־��
1- ������


*/




#include "SG90.h"

void SG90_Init(u16 arr,u16 psc){ //����ӿڳ�ʼ��
	GPIO_InitTypeDef     GPIO_InitStrue;
    TIM_OCInitTypeDef     TIM_OCInitStrue;
    TIM_TimeBaseInitTypeDef     TIM_TimeBaseInitStrue;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//ʹ��TIM3�����GPIOʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA,ENABLE);//ʹ��GPIOBʱ��(LED��PB0����)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//ʹ��AFIOʱ��(��ʱ��3ͨ��3��Ҫ��ӳ�䵽BP5����)
    
//    GPIO_InitStrue.GPIO_Pin=GPIO_Pin_13|GPIO_Pin_14;   // TIM_CH3
//    GPIO_InitStrue.GPIO_Mode=GPIO_Mode_Out_PP;    // ��������
//    GPIO_InitStrue.GPIO_Speed=GPIO_Speed_50MHz;    //�����������ٶ�
//    GPIO_Init(GPIOC,&GPIO_InitStrue);                //GPIO�˿ڳ�ʼ������
	
	GPIO_InitStrue.GPIO_Pin=GPIO_Pin_1;    // TIM3_CH4
    GPIO_InitStrue.GPIO_Mode=GPIO_Mode_AF_PP;    // ��������
    GPIO_InitStrue.GPIO_Speed=GPIO_Speed_50MHz;    //�����������ٶ�
    GPIO_Init(GPIOB,&GPIO_InitStrue);                //GPIO�˿ڳ�ʼ������
	
	
	\
    
//    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3,ENABLE); //ӳ�䣬��ӳ��ֻ����64��100��144�ŵ�Ƭ��
   //��û����ӳ��ʱ��TIM3���ĸ�ͨ��CH1��CH2��CH3��CH4�ֱ��ӦPA6��PA7,PB0,PB1
   //��������ӳ��ʱ��TIM3���ĸ�ͨ��CH1��CH2��CH3��CH4�ֱ��ӦPB4��PB5,PB0,PB1 (GPIO_PartialRemap_TIM3)
   //����ȫ��ӳ��ʱ��TIM3���ĸ�ͨ��CH1��CH2��CH3��CH4�ֱ��ӦPC6��PC7,PC8,PC9 (GPIO_FullRemap_TIM3) 
	      
    TIM_TimeBaseInitStrue.TIM_Period=arr;    //�����Զ���װ��ֵ
    TIM_TimeBaseInitStrue.TIM_Prescaler=psc;        //Ԥ��Ƶϵ��
    TIM_TimeBaseInitStrue.TIM_CounterMode=TIM_CounterMode_Up;    //�������������
    TIM_TimeBaseInitStrue.TIM_ClockDivision=TIM_CKD_DIV1;        //ʱ�ӵķ�Ƶ���ӣ�����һ������ʱ���ã�һ����ΪTIM_CKD_DIV1
    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStrue);        //TIM3��ʼ������(����PWM������)
	
    TIM_OCInitStrue.TIM_OCMode=TIM_OCMode_PWM1;        // PWMģʽ1:CNT < CCRʱ�����Ч��ƽ
    TIM_OCInitStrue.TIM_OCPolarity=TIM_OCPolarity_High;// ���ü���-��Ч��ƽΪ���ߵ�ƽ
    TIM_OCInitStrue.TIM_OutputState=TIM_OutputState_Enable;// ���ʹ��  
	       //TIM3��ͨ��3 PWM ģʽ����
		   
	TIM_OC4Init(TIM3,&TIM_OCInitStrue);

	TIM_OC4PreloadConfig(TIM3,TIM_OCPreload_Enable);
    TIM_Cmd(TIM3,ENABLE);        //ʹ��TIM3

}

//---------------����˶�--------------//

void steer(){
	
	TIM_SetCompare4(TIM3,4500);   //���ת��ָ��λ��
		DelayMs(600); 
		TIM_SetCompare4(TIM3,7450);//��ž�ʮ��
	    DelayMs(800);
		DelayMs(1500);
		TIM_SetCompare4(TIM3,4500);
        DelayMs(600); 
}

//------------------------------------//

/*********************************************************************************************
 * �������� www.DoYoung.net
 * ���ҵ��� www.DoYoung.net/YT 
*********************************************************************************************/


/*
ѡ��IO�ӿڹ�����ʽ��
GPIO_Mode_AIN ģ������
GPIO_Mode_IN_FLOATING ��������
GPIO_Mode_IPD ��������
GPIO_Mode_IPU ��������
GPIO_Mode_Out_PP �������
GPIO_Mode_Out_OD ��©���
GPIO_Mode_AF_PP �����������
GPIO_Mode_AF_OD ���ÿ�©���
*/
