#include "main.h"



void gpio_ctrl(uint32_t gpio, uint32_t value)
{
	if (value == GPIO_LOW)
		LPC_GPIO_PORT->PIN[0] &= ~gpio;
	if (value == GPIO_HIGH)
		LPC_GPIO_PORT->PIN[0] |=  gpio;
}

void gpio_dir(uint32_t gpio, uint32_t dir)
{
	if (dir == GPIO_INPUT)
		LPC_GPIO_PORT->DIR[0] &= ~gpio;
	if (dir == GPIO_OUTPUT)
		LPC_GPIO_PORT->DIR[0] |=  gpio;
}

void GPIOInit (void)
{
    LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 6);                              /* ��ʼ��GPIO AHBʱ��           */
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 7);                              /* ��ʼ��SWM  AHBʱ��           */

	gpio_init_beep();
	gpio_init_led();
	gpio_init_rf433m_mode();
	gpio_init_wifi();
#if 0

	LPC_GPIO_PORT->DIR[0] &= ~KEY;


	LPC_SYSCON->PINTSEL[0] = 0x1;                                       /* ����P0.1Ϊ�жϴ�������       */
    LPC_PININT->ISEL   &= ~0x1;                                         /* ���ش���                     */
    LPC_PININT->SIENF   |=  0x1;                                         /* �±��ش���                   */
    
    NVIC_EnableIRQ(PIN_INT0_IRQn);                                      /* ��PININT0�ж�              */
#endif
}





