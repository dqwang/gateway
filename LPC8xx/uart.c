#include "main.h"

uart_t uart0;
uart_t uart1;
uart_t uart2;


void CLEAR_UART_RECV(uart_t *p)
{
	p->rflag=0;
	p->rindex=0;
}

void UART0RecvEnable(void)
{
	LPC_USART0->INTENSET = (1 << 0);
}

void UART0RecvDisable(void)
{
	LPC_USART0->INTENCLR = (1 << 0);
}
void UART0SendEnable(void)
{
	LPC_USART0->INTENSET |= (1 << 2);
}

void UART0SendDisable(void)
{
	LPC_USART0->INTENCLR |= (1 << 2);
}

void UART0Init (void)
{
    LPC_SWM->PINASSIGN[0] &= ~( 0xFFFF << 0 );
    LPC_SWM->PINASSIGN[0] |=  ( 0 << 0 );                               /* P0.0 ~ UART0_RXD rf433m recv */
    LPC_SWM->PINASSIGN[0] |=  ( 6 << 8 );                               /* P0.6 ~ UART0_TXD rf433m send */

    LPC_SYSCON->UARTCLKDIV     = 1;                                     /* UART时钟分频值为 1           */
    LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 14);                             /* 初始化UART AHB时钟           */

    LPC_USART0->BRG = SystemCoreClock * LPC_SYSCON->SYSAHBCLKDIV /
                      (LPC_SYSCON->UARTCLKDIV * 16 * UART0_BPS) - 1;     /* 串口通信波特率               */
    LPC_USART0->CFG = (1 << 0) |                                        /* 使能UART                     */
                      (1 << 2) |                                        /* 8位数据位                    */
                      (0 << 4) |                                        /* 无校验                       */
                      (0 << 6);                                         /* 1位停止位                    */

	UART0RecvEnable();
    NVIC_EnableIRQ(UART0_IRQn);

	CLEAR_UART(&uart0);
}

void uart0_sendbuf(u8* buf, u16 size)
{
	if (buf == NULL || size <=0){
		return;
	}
	memcpy(uart0.sbuf, buf, size);
	uart0.slen = size;
	uart0.sindex = 0;
	UART0SendEnable();
}

void UART0_IRQHandler (void)
{
    if (LPC_USART0->STAT & 0x01) {//recv
		uart0.rbuf[uart0.rindex++] = LPC_USART0->RXDATA;
		if (uart0.rindex >= UART_RBUF_SIZE){
			uart0.rindex = 0;
		}
		uart0.rflag=1;
    }

	if (LPC_USART0->STAT & 0x04) {//send
		if (uart0.slen == 0){
			return;
		}
		LPC_USART0->TXDATA = uart0.sbuf[uart0.sindex++];
		if (uart0.sindex >= uart0.slen) {
			UART0SendDisable();
			uart0.slen=0;
		}
	}
}



void UART1SendEnable(void)
{
	LPC_USART1->INTENSET |= (1 << 2);
}

void UART1SendDisable(void)
{
	LPC_USART1->INTENCLR |= (1 << 2);
}

void UART1RecvEnable(void)
{
	LPC_USART1->INTENSET = (1 << 0);
}

void UART1Init (void)
{
    LPC_SWM->PINASSIGN[1] &= ~( 0xFFFF << 8 );
    LPC_SWM->PINASSIGN[1] |=  ( 21 << 8 );                               /* P0.21 ~ UART1_TXD             */
    LPC_SWM->PINASSIGN[1] |=  ( 22 << 16 );                              /* P0.22 ~ UART1_RXD             */

    LPC_SYSCON->UARTCLKDIV     = 1;                                     /* UART时钟分频值为 1           */
    LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 15);                             /* 初始化UART AHB时钟           */

    LPC_USART1->BRG = SystemCoreClock * LPC_SYSCON->SYSAHBCLKDIV /
                      (LPC_SYSCON->UARTCLKDIV * 16 * UART1_BPS) - 1;     /* 串口通信波特率               */
    LPC_USART1->CFG = (1 << 0) |                                        /* 使能UART                     */
                      (1 << 2) |                                        /* 8位数据位                    */
                      (0 << 4) |                                        /* 无校验                       */
                      (0 << 6);                                         /* 1位停止位                    */

    LPC_USART1->INTENSET = (1 << 0);                                    /* 使能接收中断                 */
    NVIC_EnableIRQ(UART1_IRQn);                                         /* 打开UART中断                 */
}

void UART1_IRQHandler (void)
{
	if (LPC_USART1->STAT & 0x01) {//recv
		#if 1
		uart1.rbuf[uart1.rindex++] = LPC_USART1->RXDATA;
		if (uart1.rindex >= UART_RBUF_SIZE){
			uart1.rindex = 0;
		}
		uart1.rflag=1;
		#else
		uart_test =  LPC_USART1->RXDATA;
		UART1SendEnable();
		#endif
    }
	if (LPC_USART1->STAT & 0x04) {//send
	#if 1
		if (uart1.slen == 0){
			return;
		}
		LPC_USART1->TXDATA = uart1.sbuf[uart1.sindex++];
		if (uart1.sindex >= uart1.slen) {
			UART1SendDisable();
			uart1.slen=0;
		}
	#else
		LPC_USART1->TXDATA = uart_test;
		UART1SendDisable();
	#endif
	}
}

void uart1_sendbuf(u8* buf, u16 size)
{
	if (buf == NULL || size <=0){
		return;
	}
	memcpy(uart1.sbuf, buf, size);
	uart1.slen = size;
	uart1.sindex = 0;
	UART1SendEnable();
}


void UART2SendEnable(void)
{
	LPC_USART2->INTENSET |= (1 << 2);
}

void UART2SendDisable(void)
{
	 LPC_USART2->INTENCLR |= (1 << 2);
}

void UART2Init (void)
{
    LPC_SWM->PINASSIGN[2] &= ~( (uint32_t)0xFFFF << 16 );
    LPC_SWM->PINASSIGN[2] |=  ( 16 << 16 );                              /* P0.16 ~ UART2_TXD             */
    LPC_SWM->PINASSIGN[2] |=  ( 10 << 24 );                              /* P0.10 ~ UART2_RXD             */

    LPC_SYSCON->UARTCLKDIV     = 1;                                     /* UART时钟分频值为 1           */
    LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 16);                             /* 初始化UART AHB时钟           */

    LPC_USART2->BRG = SystemCoreClock * LPC_SYSCON->SYSAHBCLKDIV /
                      (LPC_SYSCON->UARTCLKDIV * 16 * UART2_BPS) - 1;     /* 串口通信波特率               */
    LPC_USART2->CFG = (1 << 0) |                                        /* 使能UART                     */
                      (1 << 2) |                                        /* 8位数据位                    */
                      (0 << 4) |                                        /* 无校验                       */
                      (0 << 6);                                         /* 1位停止位                    */

    LPC_USART2->INTENSET = (1 << 0);                                    /* 使能接收中断                 */
    NVIC_EnableIRQ(UART2_IRQn);                                         /* 打开UART中断                 */
}

void UART2_IRQHandler (void)
{
    if (LPC_USART2->STAT & 0x01) {                                      /* 接收中断                     */
		uart2.rbuf[uart2.rindex++] = LPC_USART2->RXDATA;
		if (uart2.rindex >= UART_RBUF_SIZE){
			uart2.rindex = 0;
		}
		uart2.rflag=1;
    }

    if (LPC_USART2->STAT & 0x04) {                                      /* 发送中断                     */
		if (uart2.slen == 0){
			return;
		}
		LPC_USART2->TXDATA = uart2.sbuf[uart2.sindex++];
		if (uart2.sindex >= uart2.slen) {
			UART2SendDisable();
			uart2.slen=0;
		}
    }
}

void uart2_sendbuf(u8* buf, u16 size)
{
	if (buf == NULL || size <=0){
		return;
	}
	memcpy(uart2.sbuf, buf, size);
	uart2.slen = size;
	uart2.sindex = 0;
	UART2SendEnable();
}



void test_uart0_echo(void)
{
	if (uart0.rflag == 1){
		//delay_ms(10);//waiting until the packet done
		uart0_sendbuf(uart0.rbuf, uart0.rindex);
		CLEAR_UART_RECV(&uart0);
	}
}


void test_uart1_echo(void)
{
	if (uart1.rflag == 1){
		//delay_ms(10);//waiting until the packet done

		uart1_sendbuf(uart1.rbuf, uart1.rindex);
		CLEAR_UART_RECV(&uart1);
	}
}

void test_uart2_echo(void)
{
	if (uart2.rflag == 1){
		delay_ms(10);//waiting until the packet done
		uart2_sendbuf(uart2.rbuf, uart2.rindex);

		CLEAR_UART_RECV(&uart2);
	}
}

void test_uart0_send(void)
{
	u8 sbuf[1]={0x01};
	uart0_sendbuf(sbuf,sizeof(sbuf));
	delay_ms(1);
}

void test_uart1_send(void)
{
	u8 sbuf[1]={0x01};
	uart1_sendbuf(sbuf,sizeof(sbuf));
	delay_ms(1);
}

void test_uart2_send(void)
{
	u8 sbuf[1]={0x01};
	uart2_sendbuf(sbuf,sizeof(sbuf));
	delay_ms(1);
}

void uart0_thread(void)
{
	if (uart0.rflag == 1){
		delay_ms(20);//waiting until the packet done
		
		send2server(uart0.rbuf,uart0.rindex);		
		CLEAR_UART_RECV(&uart0);
		
	}
}


void uart2_thread(void)
{
	if (uart2.rflag == 1){
		/*wifi 模块全功耗模式大概10ms接收完毕，若低功耗模式可能需要200ms*/
		#if 1//delay mode
		delay_ms(WIFI_RECV_TIME);//waiting until the packet done
		send2lock(uart2.rbuf, uart2.rindex);	

		delay_ms(10);
				
		CLEAR_UART_RECV(&uart2);	
		#else//ring mode
		//if (uart2.rindex >){}
		
		#endif
	}
}

