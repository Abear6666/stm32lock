#include "stm32f10x.h"                  // Device header
#include "PWM.h"
#include "USART.h"
#include "delay.h"

void Servo_Init(void)
{
	PWM_Init();
}

void Servo_SetAngle(float Angle)
{
	
	PWM_SetCompare2(Angle / 180 * 2000 + 500);
}

void open_door(void) //�����ת90��ģ�¿���
{
	Servo_SetAngle(90);
	
}

void close_door(void)//�����ת90��ģ�¹���
{
	Servo_SetAngle(0);
}
