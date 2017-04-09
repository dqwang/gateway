#ifndef __LED_H__
#define __LED_H__

#define GPIO_LED1	(1 << 17)//LED1 P0.17
#define GPIO_LED2	(1 << 18)//LED2 P0.18
#define GPIO_LED3	(1 << 19)//LED3 P0.19
#define GPIO_BEEP	(1 << 12)//beep P0.12

void gpio_init_beep(void);
void gpio_init_led(void);



#endif
