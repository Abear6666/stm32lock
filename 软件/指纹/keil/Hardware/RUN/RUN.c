/*�˲������ڿ��Ƴ����˶�*/

#include "RUN.h"
#include "sys.h"
#include "delay.h"
#include "hcsr04.h"

float a,b,c,d,f;//�洢��������ľ��룬���бȽ��ж�
float lmax=15;
float lmin=3;
float mark=0;

/*��ʱ������12�����Һ��֣�12Ϊ��ת��21Ϊ��ת��34��������֣�*/
//ǰ�������ɶ�ʱ������34������34����ת
//ǰ�����Ƕ�ʱ������12������

int detect(void){
	mark=0;
	TIM_SetCompare4(TIM4,7500);//��ʼת���,ת��7500������ʱ
	delay_ms(1500);
//	a= Hcsr04GetLength();   //�����㷨&&(((a-b)<0.8)||((a-b)>-0.8))
//	delay_ms(50);
//	b= Hcsr04GetLength();
//	if((a<=lmax)&&(a>lmin)&&(b<=lmax)&&(b>lmin)){
//		c=(a+b)/2;
//	}
//	delay_ms(50);
	//printf("ǰ���ϰ���c����Ϊ��%3fcm\r\n",c);
	TIM_SetCompare4(TIM4,1500);//��ʼת���,ת��1500������ʱ
	delay_ms(1500);	
//	a= Hcsr04GetLength();//�����㷨&&(((a-b)<0.8)||((a-b)>-0.8))
//	delay_ms(50);
//	b= Hcsr04GetLength();
//	if((a<=lmax)&&(a>lmin)&&(b<=lmax)&&(b>lmin)){
//		d=(a+b)/2;
//	}
	//delay_ms(50);
	//printf("ǰ���ϰ���d����Ϊ��%3fcm\r\n",d);
	TIM_SetCompare4(TIM4,4500);//��ʼת���,ת��4500������ʱ
	delay_ms(200);
    if((c<10)&&(c>3)&&(d<10)&&(d>3)){//���߶����ϰ�������Ϊ1������
		 mark=1;           //a=1����������
	}
	else if((c<10)&&(c>3)&&((d>14)||(d=0))){
		 mark=2;	        //a=2��������ת
	}
	else if((d<10)&&(d>3)&&((c>14)||(c=0))){
		 mark=3;	        //a=2��������ת
	}
			
	return mark;
}
void stop_run(void){
	 TIM_SetCompare1(TIM3,0);//��ͣ
	 TIM_SetCompare2(TIM3,0);
	 TIM_SetCompare3(TIM3,0);
	 TIM_SetCompare4(TIM3,0);//��ͣ
	 
	 TIM_SetCompare2(TIM2,0);
	 TIM_SetCompare1(TIM2,0);
	 TIM_SetCompare3(TIM2,0);
	 TIM_SetCompare4(TIM2,0);
 }
 
 void back_run(void){       //����
	 TIM_SetCompare1(TIM3,25100);
	 TIM_SetCompare2(TIM3,0);//
	 TIM_SetCompare3(TIM3,25100);  
	 TIM_SetCompare4(TIM3,0);
	 
	 TIM_SetCompare2(TIM2,25100);
	 TIM_SetCompare1(TIM2,0);
	 TIM_SetCompare4(TIM2,25100);
	 TIM_SetCompare3(TIM2,0);
 }
 void run(void){        //ֱ����ʻ
	 TIM_SetCompare2(TIM3,25100);//
	 TIM_SetCompare1(TIM3,0);//
	 TIM_SetCompare4(TIM3,25100);  
	 TIM_SetCompare3(TIM3,0);
	 
	 TIM_SetCompare1(TIM2,25100);//
	 TIM_SetCompare2(TIM2,0);
	 TIM_SetCompare4(TIM2,0);//**
	 TIM_SetCompare3(TIM2,25100);//**
	 delay_s(1);
 }
 void l_b_run(void){
	 TIM_SetCompare2(TIM3,5100);//����ת���ұߵ���һ����Ҫ��ǰ�ߣ���ߵ����ת
	 TIM_SetCompare1(TIM3,0);//����
	 TIM_SetCompare3(TIM3,35100);
	 TIM_SetCompare4(TIM3,0);
	 
	 TIM_SetCompare2(TIM4,30100);//ǰ��
	 TIM_SetCompare1(TIM4,0);
	 TIM_SetCompare3(TIM4,35100);
	 TIM_SetCompare4(TIM2,0);
	 delay_ms(500);
 }
 
 void r_b_run(void){
	 TIM_SetCompare1(TIM3,0);//����ת����ߵ���һ����Ҫ��ǰ�ߣ��ұߵ����ת
	 TIM_SetCompare2(TIM3,0);//����
	 TIM_SetCompare1(TIM2,0);
	 TIM_SetCompare2(TIM2,0);
	 
	 TIM_SetCompare4(TIM3,45100);//ǰת
	 TIM_SetCompare3(TIM3,0);
	 TIM_SetCompare4(TIM2,45100);
	 TIM_SetCompare3(TIM2,0);
	 
	 
 }