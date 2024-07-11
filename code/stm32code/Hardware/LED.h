#ifndef __LED_H
#define __LED_H
#include "sys.h"

void LED_Init(void);
void LED1_ON(void);
void LED1_OFF(void);
void LED1_Turn(void);

#define LED1 PCout(13)	// PC13	


#endif
