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

// invoke this reset function if you need to reconnect server
// 考虑时间是否该用定时器实现
void hwapi04_wifi_reset(void)
{
	gpio_ctrl(GPIO_WIFI_RESET, GPIO_LOW);
	delay_ms(20);//must be more than 10ms
	gpio_ctrl(GPIO_WIFI_RESET, GPIO_HIGH);
}


void hwapi04_wifi_disable(void)
{
	gpio_ctrl(GPIO_WIFI_RESET, GPIO_LOW);
}


void test_hwapi04_wifi_reset(void)
{
	delay_ms(1000);
	hwapi04_wifi_reset();
	delay_ms(1000);
}

//需增加flash flag, 记录是否已经恢复过出厂设置。

void hwapi05_wifi_factory(void)
{
	gpio_ctrl(GPIO_WIFI_FACTORY, GPIO_LOW);
	delay_ms(3500);//must be more than 3s
	gpio_ctrl(GPIO_WIFI_FACTORY, GPIO_HIGH);
}

void test_hwapi05_wifi_factory(void)
{
	hwapi05_wifi_factory();
}

void test_wifi_uart2(void)
{
	u8 sbuf[1]={0x0a};
	uart2_sendbuf(sbuf,sizeof(sbuf));
	delay_ms(1);
	//checkpoint
	//signal should be ok in hardware
	//PC rak415 tool should receive the data from mcu
	//next todo: mcu should receive data from PC
}

//USR-TCP232-T

void hwapi06_rj45_reset(void)
{
	gpio_ctrl(GPIO_WIFI_RESET, GPIO_LOW);
	delay_ms(250);//must be more than 200ms
	gpio_ctrl(GPIO_WIFI_RESET, GPIO_HIGH);
}

void test_hwapi06_rj45_reset(void)
{
	delay_ms(1000);
	hwapi06_rj45_reset();
	delay_ms(1000);
}

void test_rj45_uart2(void)
{
	u8 sbuf[1]={0x0b};
	uart2_sendbuf(sbuf,sizeof(sbuf));
	delay_ms(1);
}



