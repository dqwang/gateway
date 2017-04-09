#ifndef __RF433M_H__
#define __RF433M_H__
#include "main.h"

#define GPIO_M0		(1 << 7)//RF_CTL1 P0.7
#define GPIO_M1		(1 << 8)//RF_CTL2 P0.8
#define GPIO_AUX 	(1 << 9)//RF_CTL3 P0.9

#define RF_NORMAL_MODE 0
#define RF_WAKEUP_MODE 1
#define RF_SLEEP_MODE 2
#define RF_CONFIG_MODE 3

void gpio_init_rf433m_mode(void);

void hwapi03_rf433m_mode(u8 mode);
void hwapi07_mod_uart0_baud(u32 baud);


void hwapi07_rf433m_set_config(void);
void hwapi07_rf433m_reset(void);


void hwapi07_rf433m_mode3_prepare(void);
void hwapi08_rf433m_mode1_prepare(void);
void hwapi08_rf433m_mode0_prepare(void);


void hwapi07_rf433m_get_addr_channel(void);


void test_hwapi03_rf433m_mode(void);


#endif
