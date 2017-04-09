#include "main.h"


volatile u16 led_timer = 0;


void delay_ms (uint32_t ulTime)
{
    uint32_t i;
    
    while (ulTime--) {
        for (i = 0; i < 2401; i++);
    }
}

void WKTInit (void)
{
    LPC_SYSCON->SYSAHBCLKCTRL |=  (1 << 9);                             /* ��ʼ��WKT AHBʱ��            */
    LPC_SYSCON->PRESETCTRL    &= ~(1 << 9);
    LPC_SYSCON->PRESETCTRL    |=  (1 << 9);                             /* ��λWKT                      */
    
    /*
     * WKTʱ��Դѡ��
     * 0��IRC�����ڲ�RCʱ��Դ��750 kHz��    1��LPO�����͹���������10 kHz������ +/-45%��
     * WKT��ʱʱ�����ã�
     * IRC��N(ms) ���� COUNT = 750 * N      LPO��N(ms) ���� COUNT = 10 * N
     */
    LPC_WKT->CTRL  = 0;                                                 /* IRCΪʱ��Դ                  */

	LPC_WKT->COUNT = 750 * 1;// 1ms
	NVIC_EnableIRQ(WKT_IRQn);
}


void WKT_IRQHandler (void)
{
    if (LPC_WKT->CTRL & (1 << 1)) {
        LPC_WKT->CTRL |= (1 << 1);                                      /* ����жϱ�־                 */
		
		LPC_WKT->COUNT = 750 * 1;// 1ms
		led_timer++;
    }
}


void WKTdelayMs (uint32_t delayInMs)
{
    LPC_WKT->COUNT = 750 * delayInMs;    
    while( !(LPC_WKT->CTRL & (1 << 1))) {;}    
    LPC_WKT->CTRL |= (1 << 1);
}


/*����������ģʽ����Ҫ��WKT���ѣ���رմ˽���*/
void system_work_led_thread(void)
{
	static u8 io_flag = 0;

	if(led_timer <SYSTEM_WORK_LED_FLASH_TIME && io_flag == 0){
		hwapi02_led2_ctrl(ON);
		io_flag = 1;
	}

	if(led_timer>=SYSTEM_WORK_LED_FLASH_TIME && io_flag ==1){
		hwapi02_led2_ctrl(OFF);
		io_flag = 0;
	}

	
	if(led_timer >= 2*SYSTEM_WORK_LED_FLASH_TIME){
		led_timer = 0;
	}

}

