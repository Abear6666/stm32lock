

/*
//杜洋工作室出品
//洋桃系列开发板应用程序
//关注微信公众号：洋桃电子
//洋桃开发板资料下载 www.DoYoung.net/YT 
//即可免费看所有教学视频，下载技术资料，技术疑难提问
//更多内容尽在 杜洋工作室主页 www.doyoung.net
*/

/*
《修改日志》
1- 创建。


*/



#include "pwm.h"


void TIM3_PWM_Init(u16 arr,u16 psc){  //TIM3 PWM初始化 arr重装载值 psc预分频系数
    GPIO_InitTypeDef     GPIO_InitStrue;
    TIM_OCInitTypeDef     TIM_OCInitStrue;
    TIM_TimeBaseInitTypeDef     TIM_TimeBaseInitStrue;
    
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3|RCC_APB1Periph_TIM4|RCC_APB1Periph_TIM2,ENABLE);//使能TIM3和相关GPIO时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOA,ENABLE);//使能GPIOB时钟(LED在PB0引脚)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//使能AFIO时钟(定时器3通道3需要重映射到BP5引脚)
    
//    GPIO_InitStrue.GPIO_Pin=GPIO_Pin_13|GPIO_Pin_14;   // TIM_CH3
//    GPIO_InitStrue.GPIO_Mode=GPIO_Mode_Out_PP;    // 复用推挽
//    GPIO_InitStrue.GPIO_Speed=GPIO_Speed_50MHz;    //设置最大输出速度
//    GPIO_Init(GPIOC,&GPIO_InitStrue);                //GPIO端口初始化设置
	
	GPIO_InitStrue.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_9|GPIO_Pin_8|GPIO_Pin_7;     // TIM_CH3
    GPIO_InitStrue.GPIO_Mode=GPIO_Mode_AF_PP;    // 复用推挽
    GPIO_InitStrue.GPIO_Speed=GPIO_Speed_50MHz;    //设置最大输出速度
    GPIO_Init(GPIOB,&GPIO_InitStrue);                //GPIO端口初始化设置
	
	GPIO_InitStrue.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_8|GPIO_Pin_0;     // TIM_CH3
    GPIO_InitStrue.GPIO_Mode=GPIO_Mode_AF_PP;    // 复用推挽
    GPIO_InitStrue.GPIO_Speed=GPIO_Speed_50MHz;    //设置最大输出速度
    GPIO_Init(GPIOA,&GPIO_InitStrue);                //GPIO端口初始化设置
	
	\
    
//    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3,ENABLE); //映射，重映射只用于64、100、144脚单片机
   //当没有重映射时，TIM3的四个通道CH1，CH2，CH3，CH4分别对应PA6，PA7,PB0,PB1
   //当部分重映射时，TIM3的四个通道CH1，CH2，CH3，CH4分别对应PB4，PB5,PB0,PB1 (GPIO_PartialRemap_TIM3)
   //当完全重映射时，TIM3的四个通道CH1，CH2，CH3，CH4分别对应PC6，PC7,PC8,PC9 (GPIO_FullRemap_TIM3) 
	      
    TIM_TimeBaseInitStrue.TIM_Period=arr;    //设置自动重装载值
    TIM_TimeBaseInitStrue.TIM_Prescaler=psc;        //预分频系数
    TIM_TimeBaseInitStrue.TIM_CounterMode=TIM_CounterMode_Up;    //计数器向上溢出
    TIM_TimeBaseInitStrue.TIM_ClockDivision=TIM_CKD_DIV1;        //时钟的分频因子，起到了一点点的延时作用，一般设为TIM_CKD_DIV1
    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStrue);        //TIM3初始化设置(设置PWM的周期)
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStrue);
    TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStrue); 
	
    TIM_OCInitStrue.TIM_OCMode=TIM_OCMode_PWM1;        // PWM模式1:CNT < CCR时输出有效电平
    TIM_OCInitStrue.TIM_OCPolarity=TIM_OCPolarity_High;// 设置极性-有效电平为：高电平
    TIM_OCInitStrue.TIM_OutputState=TIM_OutputState_Enable;// 输出使能  
	TIM_OC1Init(TIM2,&TIM_OCInitStrue);   
	TIM_OC2Init(TIM2,&TIM_OCInitStrue);   
	TIM_OC3Init(TIM2,&TIM_OCInitStrue);   
	TIM_OC4Init(TIM2,&TIM_OCInitStrue);    
	TIM_OC1Init(TIM3,&TIM_OCInitStrue);    
	TIM_OC2Init(TIM3,&TIM_OCInitStrue);
    TIM_OC3Init(TIM3,&TIM_OCInitStrue);        //TIM3的通道3 PWM 模式设置
	TIM_OC4Init(TIM3,&TIM_OCInitStrue);
	TIM_OC1Init(TIM4,&TIM_OCInitStrue);    
	TIM_OC2Init(TIM4,&TIM_OCInitStrue);
	TIM_OC3Init(TIM4,&TIM_OCInitStrue);      
    TIM_OC4Init(TIM4,&TIM_OCInitStrue);

	TIM_OC1PreloadConfig(TIM2,TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM2,TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM2,TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM2,TIM_OCPreload_Enable);
    TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM3,TIM_OCPreload_Enable);
    TIM_OC3PreloadConfig(TIM3,TIM_OCPreload_Enable);        //使能预装载寄存器
    TIM_OC4PreloadConfig(TIM3,TIM_OCPreload_Enable);
	TIM_OC1PreloadConfig(TIM4,TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM4,TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM4,TIM_OCPreload_Enable);
    TIM_OC4PreloadConfig(TIM4,TIM_OCPreload_Enable);//PB9是给舵机用的，其余八个为直流电机。
    TIM_Cmd(TIM3,ENABLE);        //使能TIM3
    TIM_Cmd(TIM4,ENABLE);
	TIM_Cmd(TIM2,ENABLE);
}
