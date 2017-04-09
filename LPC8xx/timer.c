#include "main.h"


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
}

/*********************************************************************************************************
** Function name:       WKTdelayMs
** Descriptions:        WKT延时
** input parameters:    delayInMs：延时时间（单位：ms）
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void WKTdelayMs (uint32_t delayInMs)
{
    LPC_WKT->COUNT = 750 * delayInMs;                                   /* 定时500 ms                   */
    
    while( !(LPC_WKT->CTRL & (1 << 1))) {                               /* 等待超时                     */
        ;
    }
    
    LPC_WKT->CTRL |= (1 << 1);                                          /* 清除超时标志                 */

	//time_ms_close_door++;
}

