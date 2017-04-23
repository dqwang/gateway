#include "main.h"


volatile u16 led_timer = 0;
timer_t heartbeat_timer;
timer_t heartbeat_server_timer;


void delay_ms (uint32_t ulTime)
{
    uint32_t i;
    
    while (ulTime--) {
        for (i = 0; i < 2401; i++);
    }
}

void WKTInit (void)
{
    LPC_SYSCON->SYSAHBCLKCTRL |=  (1 << 9);                             /* 初始化WKT AHB时钟            */
    LPC_SYSCON->PRESETCTRL    &= ~(1 << 9);
    LPC_SYSCON->PRESETCTRL    |=  (1 << 9);                             /* 复位WKT                      */
    
    /*
     * WKT时钟源选择：
     * 0：IRC——内部RC时钟源（750 kHz）    1：LPO——低功耗振荡器（10 kHz，精度 +/-45%）
     * WKT定时时间设置：
     * IRC：N(ms) —— COUNT = 750 * N      LPO：N(ms) —— COUNT = 10 * N
     */
    LPC_WKT->CTRL  = 0;                                                 /* IRC为时钟源                  */

	LPC_WKT->COUNT = 750 * 1;// 1ms
	NVIC_EnableIRQ(WKT_IRQn);

	led_timer = 0;
	
	heartbeat_timer.cnt = 0;
	heartbeat_timer.flag = 0;

	heartbeat_server_timer.cnt = 0;
	heartbeat_server_timer.flag = 0;
}


void WKT_IRQHandler (void)
{
    if (LPC_WKT->CTRL & (1 << 1)) {
        LPC_WKT->CTRL |= (1 << 1);                                      /* 清除中断标志                 */
		
		LPC_WKT->COUNT = 750 * 1;// 1ms
		led_timer++;
		heartbeat_timer.cnt++;
		if (heartbeat_timer.cnt >= SYSTEM_HEARTBEAT_TIME){
			heartbeat_timer.cnt = 0;
			heartbeat_timer.flag = 1;
		}

/*
		if (heartbeat_server_timer.flag == 1){
			heartbeat_server_timer.cnt++;
			if (heartbeat_server_timer.cnt >= SERVER_HEARTBEAT_TIMEOUT){
				heartbeat_server_timer.flag = 2;
				heartbeat_server_timer.cnt=0;
			}
		}
*/		
    }
}


void WKTdelayMs (uint32_t delayInMs)
{
    LPC_WKT->COUNT = 750 * delayInMs;    
    while( !(LPC_WKT->CTRL & (1 << 1))) {;}    
    LPC_WKT->CTRL |= (1 << 1);
}


/*若处于休眠模式，需要用WKT唤醒，请关闭此进程*/
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

