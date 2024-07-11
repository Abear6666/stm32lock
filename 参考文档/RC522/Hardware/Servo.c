#include "stm32f10x.h"                  // Device header
#include "PWM.h"

void Servo_Init()
{
	PWM_Init();
}
	
void Servo_SetAngle(float Angle)
{
	PWM_SetCompare2(Angle/180*2000+500);	//输出比较值与角度转换
	/*	0.5ms --> 500  --> -90' */
	/*	1ms   --> 1000 --> -45' */
	/*	1.5ms --> 1500  --> 0'  */
	/*	2ms	  --> 2000 --> 45' */
	/*	2.5ms --> 2500 --> 90' */
}
