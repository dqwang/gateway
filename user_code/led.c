#include "main.h"

void gpio_init_beep(void)
{
	gpio_dir(GPIO_BEEP, GPIO_OUTPUT);
    gpio_ctrl(GPIO_BEEP, GPIO_LOW);
}

void gpio_init_led(void)
{
	gpio_dir(GPIO_LED1, GPIO_OUTPUT);
	gpio_dir(GPIO_LED2, GPIO_OUTPUT);
	gpio_dir(GPIO_LED3, GPIO_OUTPUT);
    gpio_ctrl(GPIO_LED1, GPIO_LOW);
	gpio_ctrl(GPIO_LED2, GPIO_HIGH);
	gpio_ctrl(GPIO_LED3, GPIO_HIGH);
}

