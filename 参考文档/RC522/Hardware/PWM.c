//PWM频率：		Freq = CK_PSC / (PSC + 1) / (ARR + 1)
//PWM占空比：	Duty = CCR / (ARR + 1)
//PWM分辨率：	Reso = 1 / (ARR + 1)

#include "stm32f10x.h"                  // Device header

void PWM_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);	//打开定时器2的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);	//打开GPIOA的时钟			/*注意：只有部分端口才有PWM功能*/

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	TIM_InternalClockConfig(TIM2);	//设置定时器2的时钟源为内部时钟源
	
	TIM_TimeBaseInitTypeDef TimBaseInitStructure;		//设置时基单元
	TimBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;		
	TimBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; 	//设置计数模式为向上计数
	TimBaseInitStructure.TIM_Period = 20000-1;		//ARR(自动重装器，设置计数峰值)
	TimBaseInitStructure.TIM_Prescaler = 72-1;		//PSC(预分频器，设置计时频率)
	TimBaseInitStructure.TIM_RepetitionCounter = 0;	//设置计数重装值
	TIM_TimeBaseInit(TIM2, &TimBaseInitStructure);
	
	TIM_OCInitTypeDef TIM_OCInitStructure;		//设置输出比较单元
	TIM_OCStructInit(&TIM_OCInitStructure);		//先对TIM_OCInitStructure所有成员赋值，之后只会对部分成员重新赋值，防止出现部分成员未赋值情况
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	//设置输出比较模式为PWM1模式
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	//输出比较极性（设置REF，有效电平为高电平）
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//输出状态使能
	TIM_OCInitStructure.TIM_Pulse = 0;		//CRR(输出比较值)
	TIM_OC2Init(TIM2,&TIM_OCInitStructure);	//使用输出比较通道2
	
	TIM_Cmd(TIM2,ENABLE);	//定时器2使能
}

void PWM_SetCompare2(uint16_t Compare)	//设置输出比较值
{
	TIM_SetCompare2(TIM2,Compare);
}
