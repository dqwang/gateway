#ifndef __WIFI_H__
#define __WIFI_H__
#include "main.h"

#define GPIO_WIFI_MODE_WPS	(1 << 24)//MODE/WPS P0.24
#define GPIO_WIFI_SLEEP		(1 << 25)//SLEEP P0.25
#define GPIO_WIFI_FACTORY	(1 << 26)//FACTORY P0.26
#define GPIO_WIFI_RESET 	(1 << 27)//RESET P0.27


#define WIFI_RECV_TIME 100 //ms
#define GPRS_RECV_TIME 100

void gpio_init_wifi(void);
void hwapi04_wifi_reset(void);
void hwapi05_wifi_factory(void);

void hwapi06_rj45_reset(void);
void hwapi04_wifi_disable(void);


#endif
