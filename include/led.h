#ifndef __LED_H__
#define __LED_H__

#define GPIO_LED1	(1 << 17)//LED1 P0.17
#define GPIO_LED2	(1 << 18)//LED2 P0.18
#define GPIO_LED3	(1 << 19)//LED3 P0.19
#define GPIO_BEEP	(1 << 12)//beep P0.12

void gpio_init_beep(void);
void gpio_init_led(void);

void hwapi01_beep_crtl(u8 on_off);
void hwapi02_led1_ctrl(u8 on_off);
void hwapi02_led2_ctrl(u8 on_off);
void hwapi02_led3_ctrl(u8 on_off);

void hwapi01_beep_cnt(u8 cnt, u16 ms);


void test_hwapi01_beep_crtl(void);
void test_hwapi02_led_ctrl(void);

#endif
