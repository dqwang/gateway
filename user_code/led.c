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



void hwapi01_beep_crtl(u8 on_off)
{
	if (on_off > 0){
		gpio_ctrl(GPIO_BEEP, GPIO_HIGH);
	}else{
		gpio_ctrl(GPIO_BEEP, GPIO_LOW);
	}
}

void test_hwapi01_beep_crtl(void)
{
	hwapi01_beep_crtl(1);//beep on
	delay_ms(1000);
	hwapi01_beep_crtl(0);//beep off
	delay_ms(1000);
}


void hwapi02_led1_ctrl(u8 on_off)
{
	if (on_off > 0){
		gpio_ctrl(GPIO_LED1, GPIO_LOW);
	}else{
		gpio_ctrl(GPIO_LED1, GPIO_HIGH);
	}
}

void hwapi02_led2_ctrl(u8 on_off)
{
	if (on_off > 0){
		gpio_ctrl(GPIO_LED2, GPIO_LOW);
	}else{
		gpio_ctrl(GPIO_LED2, GPIO_HIGH);
	}
}

void hwapi02_led3_ctrl(u8 on_off)
{
	if (on_off > 0){
		gpio_ctrl(GPIO_LED3, GPIO_LOW);
	}else{
		gpio_ctrl(GPIO_LED3, GPIO_HIGH);
	}
}



void test_hwapi02_led_ctrl(void)
{
	hwapi02_led1_ctrl(1);//led on
	hwapi02_led2_ctrl(1);//led on
	hwapi02_led3_ctrl(1);//led on
	delay_ms(100);
	hwapi02_led1_ctrl(0);//led off
	hwapi02_led2_ctrl(0);//led off
	hwapi02_led3_ctrl(0);//led off	
	delay_ms(100);
}


