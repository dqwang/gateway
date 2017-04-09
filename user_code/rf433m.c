#include "main.h"

void gpio_init_rf433m_mode(void)
{
	gpio_dir(GPIO_M0, GPIO_OUTPUT);
	gpio_dir(GPIO_M1, GPIO_OUTPUT);

	gpio_ctrl(GPIO_M1, GPIO_LOW);
	gpio_ctrl(GPIO_M0, GPIO_HIGH);//wakeup mode, to wakeup the lock
}


