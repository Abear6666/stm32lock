#include "hcsr04.h"
#include "delay.h"
 
u16 msHcCount=0;

//基本定时器6
//IO口初始化 TRIG为普通推挽输出,ECHO为浮空输入
void Hcsr04_Init(void)
{  
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;   
    GPIO_InitTypeDef GPIO_InitStructure;
			
    HCSR04_PORTC_CLK_FUN(HCSR04_CLK, ENABLE);
   
    GPIO_InitStructure.GPIO_Pin =HCSR04_TRIG;      
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(HCSR04_PORT,HCSR04_TRIG);
     
    GPIO_InitStructure.GPIO_Pin =   HCSR04_ECHO;     
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);  
    GPIO_ResetBits(HCSR04_PORT,HCSR04_ECHO);    
     
          
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);   
     
    TIM_TimeBaseStructure.TIM_Period = (1000-1);         //1ms中断一次
    TIM_TimeBaseStructure.TIM_Prescaler =(72-1); 
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);          
        
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);  
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);    

    TIM_Cmd(TIM2,DISABLE);     
}

//关闭定时器2
static void CloseTimerForHc(void)    
{
   TIM_Cmd(TIM2, DISABLE); 
} 

//开启定时器2
static void OpenTimerForHc(void)  
{
   TIM_SetCounter(TIM2,0);
   msHcCount = 0;
   TIM_Cmd(TIM2, ENABLE); 
}

//获取定时器2计数器值
u32 GetEchoTimer(void)
{
   u32 t = 0;
   t = msHcCount*1000;
   t += TIM_GetCounter(TIM2);
   TIM2->CNT = 0;  
   delay_ms(50);
   return t;
}

//通过定时器6计数器值推算距离
//单位cm,假设测试值为x,距离y=x/1m*17000
//这里每测出5个求平均值输出
float Hcsr04GetLength(void)
{
   u32 t = 0;
   int i = 0;
   float lengthTemp = 0;
   float sum = 0;
	while(i==0)
   {
      GPIO_SetBits(HCSR04_PORT,HCSR04_TRIG);
      delay_us(10);
      GPIO_ResetBits(HCSR04_PORT,HCSR04_TRIG);
      while(GPIO_ReadInputDataBit(HCSR04_PORT,HCSR04_ECHO)==RESET)
      OpenTimerForHc();		//打开定时器
      while(GPIO_ReadInputDataBit(HCSR04_PORT,HCSR04_ECHO)==SET)
	  CloseTimerForHc();		//关闭定时器
	 t = GetEchoTimer();
      lengthTemp = ((float)t/58.0);//cm
     
      sum = lengthTemp + sum ;
    }
    lengthTemp = sum/1.0;
    return lengthTemp;
}

