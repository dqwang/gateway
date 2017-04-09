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
}

/*********************************************************************************************************
** Function name:       WKTdelayMs
** Descriptions:        WKT��ʱ
** input parameters:    delayInMs����ʱʱ�䣨��λ��ms��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void WKTdelayMs (uint32_t delayInMs)
{
    LPC_WKT->COUNT = 750 * delayInMs;                                   /* ��ʱ500 ms                   */
    
    while( !(LPC_WKT->CTRL & (1 << 1))) {                               /* �ȴ���ʱ                     */
        ;
    }
    
    LPC_WKT->CTRL |= (1 << 1);                                          /* �����ʱ��־                 */

	//time_ms_close_door++;
}

