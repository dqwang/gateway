#include "main.h"

void gpio_init_wifi(void)
{
	gpio_dir(GPIO_WIFI_FACTORY, GPIO_OUTPUT);
	gpio_dir(GPIO_WIFI_RESET, GPIO_OUTPUT);
	gpio_dir(GPIO_WIFI_MODE_WPS, GPIO_OUTPUT);
	gpio_dir(GPIO_WIFI_SLEEP, GPIO_OUTPUT);

	gpio_ctrl(GPIO_WIFI_FACTORY, GPIO_HIGH);
	gpio_ctrl(GPIO_WIFI_RESET, GPIO_HIGH);
	gpio_ctrl(GPIO_WIFI_MODE_WPS, GPIO_HIGH);
	gpio_ctrl(GPIO_WIFI_SLEEP, GPIO_HIGH);

	//test_hwapi04_wifi_reset();
}


