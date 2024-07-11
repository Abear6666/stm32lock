/*此部分用于控制车身运动*/

#include "RUN.h"
#include "sys.h"
#include "delay.h"
#include "hcsr04.h"

float a,b,c,d,f;//存储三个方向的距离，进行比较判断
float lmax=15;
float lmin=3;
float mark=0;

/*定时器三的12控制右后轮，12为反转，21为正转；34控制左后轮，*/
//前右轮是由定时器三的34决定，34是正转
//前左轮是定时器三的12决定，

int detect(void){
	mark=0;
	TIM_SetCompare4(TIM4,7500);//开始转舵机,转到7500处，延时
	delay_ms(1500);
//	a= Hcsr04GetLength();   //距离算法&&(((a-b)<0.8)||((a-b)>-0.8))
//	delay_ms(50);
//	b= Hcsr04GetLength();
//	if((a<=lmax)&&(a>lmin)&&(b<=lmax)&&(b>lmin)){
//		c=(a+b)/2;
//	}
//	delay_ms(50);
	//printf("前方障碍物c距离为：%3fcm\r\n",c);
	TIM_SetCompare4(TIM4,1500);//开始转舵机,转到1500处，延时
	delay_ms(1500);	
//	a= Hcsr04GetLength();//距离算法&&(((a-b)<0.8)||((a-b)>-0.8))
//	delay_ms(50);
//	b= Hcsr04GetLength();
//	if((a<=lmax)&&(a>lmin)&&(b<=lmax)&&(b>lmin)){
//		d=(a+b)/2;
//	}
	//delay_ms(50);
	//printf("前方障碍物d距离为：%3fcm\r\n",d);
	TIM_SetCompare4(TIM4,4500);//开始转舵机,转到4500处，延时
	delay_ms(200);
    if((c<10)&&(c>3)&&(d<10)&&(d>3)){//两边都有障碍，将置为1，带出
		 mark=1;           //a=1代表将往后退
	}
	else if((c<10)&&(c>3)&&((d>14)||(d=0))){
		 mark=2;	        //a=2代表向左转
	}
	else if((d<10)&&(d>3)&&((c>14)||(c=0))){
		 mark=3;	        //a=2代表向右转
	}
			
	return mark;
}
void stop_run(void){
	 TIM_SetCompare1(TIM3,0);//车停
	 TIM_SetCompare2(TIM3,0);
	 TIM_SetCompare3(TIM3,0);
	 TIM_SetCompare4(TIM3,0);//车停
	 
	 TIM_SetCompare2(TIM2,0);
	 TIM_SetCompare1(TIM2,0);
	 TIM_SetCompare3(TIM2,0);
	 TIM_SetCompare4(TIM2,0);
 }
 
 void back_run(void){       //倒车
	 TIM_SetCompare1(TIM3,25100);
	 TIM_SetCompare2(TIM3,0);//
	 TIM_SetCompare3(TIM3,25100);  
	 TIM_SetCompare4(TIM3,0);
	 
	 TIM_SetCompare2(TIM2,25100);
	 TIM_SetCompare1(TIM2,0);
	 TIM_SetCompare4(TIM2,25100);
	 TIM_SetCompare3(TIM2,0);
 }
 void run(void){        //直线行驶
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
	 TIM_SetCompare2(TIM3,5100);//向左转向，右边的那一列需要向前走，左边的向后转
	 TIM_SetCompare1(TIM3,0);//后轮
	 TIM_SetCompare3(TIM3,35100);
	 TIM_SetCompare4(TIM3,0);
	 
	 TIM_SetCompare2(TIM4,30100);//前轮
	 TIM_SetCompare1(TIM4,0);
	 TIM_SetCompare3(TIM4,35100);
	 TIM_SetCompare4(TIM2,0);
	 delay_ms(500);
 }
 
 void r_b_run(void){
	 TIM_SetCompare1(TIM3,0);//向右转向，左边的那一列需要向前走，右边的向后转
	 TIM_SetCompare2(TIM3,0);//后轮
	 TIM_SetCompare1(TIM2,0);
	 TIM_SetCompare2(TIM2,0);
	 
	 TIM_SetCompare4(TIM3,45100);//前转
	 TIM_SetCompare3(TIM3,0);
	 TIM_SetCompare4(TIM2,45100);
	 TIM_SetCompare3(TIM2,0);
	 
	 
 }