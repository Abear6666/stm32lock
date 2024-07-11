#ifndef _HCSR04_H
#define _HCSR04_H

#include "sys.h"

#define HCSR04_PORT              GPIOB
#define HCSR04_PORTC_CLK_FUN     RCC_APB2PeriphClockCmd
#define HCSR04_CLK               RCC_APB2Periph_GPIOB
#define HCSR04_TRIG              GPIO_Pin_5
#define HCSR04_ECHO              GPIO_Pin_6

void Hcsr04_Init(void);
static void CloseTimerForHc(void);
static void OpenTimerForHc(void); 
u32 GetEchoTimer(void);
float Hcsr04GetLength(void);

#endif

