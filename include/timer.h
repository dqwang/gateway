#ifndef __TIMER_H__
#define __TIMER_H__

#define SYSTEM_WORK_LED_FLASH_TIME 100//ms
#define SYSTEM_HEARTBEAT_TIME (15*1000)//ms
#define SERVER_HEARTBEAT_TIMEOUT (5*1000)//ms

typedef struct timer{
	u32 cnt;
	u8 flag;
}timer_t;


void delay_ms (uint32_t ulTime);
void WKTInit (void);

void WKTdelayMs (uint32_t delayInMs);
void system_work_led_thread(void);


#endif
