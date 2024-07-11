#ifndef __SERVO_H
#define __SERVO_H

void Servo_Init(void);
void Servo_SetAngle(float Angle);
void open_door(void); //舵机旋转90度模仿开门
void close_door(void);//舵机旋转90度模仿关门

#endif
