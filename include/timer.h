#ifndef __TIMER_H__
#define __TIMER_H__

#define SYSTEM_WORK_LED_FLASH_TIME 100//ms

void delay_ms (uint32_t ulTime);
void WKTInit (void);

void WKTdelayMs (uint32_t delayInMs);
void system_work_led_thread(void);



#endif
