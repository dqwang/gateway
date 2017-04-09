#ifndef __RF433M_H__
#define __RF433M_H__
#include "main.h"

#define GPIO_M0		(1 << 7)//RF_CTL1 P0.7
#define GPIO_M1		(1 << 8)//RF_CTL2 P0.8
#define GPIO_AUX 	(1 << 9)//RF_CTL3 P0.9

void gpio_init_rf433m_mode(void);

#endif
