#include "main.h"

/******************************************************************************/
#define GPIO_LED1	(1 << 17)//LED1 P0.17
#define GPIO_LED2	(1 << 18)//LED2 P0.18
#define GPIO_LED3	(1 << 19)//LED3 P0.19
#define GPIO_BEEP	(1 << 12)//beep P0.12

#define GPIO_M0		(1 << 7)//RF_CTL1 P0.7
#define GPIO_M1		(1 << 8)//RF_CTL2 P0.8



#define UART0_BPS        115200/*433M*/



#define UART_RBUF_SIZE (64)



typedef struct uart{
	u8 rflag;
	u8 rbusy;
	u8 rindex;
	u8 rbuf[UART_RBUF_SIZE];
	u8 slen;
	u8 sindex;
	u8 sbuf[UART_RBUF_SIZE];
}uart_t;

typedef enum ERRNO{
	EOK = 0,
	E_INVALID_PACKET,
	E_NULL_POINTER,
	E_INVALID_CMD,

	
}errno_t;

/*
typedef struct packet{
	u8 header[2];//0x56 0x59 
	u8 len;//total 
	u8 cmd;
	u8 payload[PAYLOAD_MAX_SIZE];	
}packet_t;

*/


uart_t uart0;





//#define DEBUG_LOG(buf, len) uart0_sendbuf(buf, len)
#define DEBUG_LOG(buf, len)


#define send2server(buf,len) uart0_sendbuf(buf, len)



#define CLEAR_UART(p) memset(p,0x0,sizeof(uart_t))


/*****************************function prototype*******************************/

void GPIOInit (void);
void gpio_ctrl(uint32_t gpio, uint32_t value);
void gpio_dir(uint32_t gpio, uint32_t dir);

void gpio_init_beep(void);
void gpio_init_led(void);
void gpio_init_rf433m_mode(void);


void hwapi01_beep_crtl(u8 on_off);
void hwapi02_led1_ctrl(u8 on_off);
void hwapi02_led2_ctrl(u8 on_off);
void hwapi02_led3_ctrl(u8 on_off);
void hwapi03_rf433m_mode(u8 mode);



void test_hwapi01_beep_crtl(void);
void test_hwapi02_led_ctrl(void);
void test_hwapi03_rf433m_mode(void);

/******************************************************************************/



void myDelay (uint32_t ulTime)
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

void GPIOInit (void)
{
    LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 6);                              /* 初始化GPIO AHB时钟           */
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 7);                              /* 初始化SWM  AHB时钟           */

	gpio_init_beep();
	gpio_init_led();
	gpio_init_rf433m_mode();

#if 0

	LPC_GPIO_PORT->DIR[0] &= ~KEY;


	LPC_SYSCON->PINTSEL[0] = 0x1;                                       /* 设置P0.1为中断触发引脚       */
    LPC_PININT->ISEL   &= ~0x1;                                         /* 边沿触发                     */
    LPC_PININT->SIENF   |=  0x1;                                         /* 下边沿触发                   */
    
    NVIC_EnableIRQ(PIN_INT0_IRQn);                                      /* 打开PININT0中断              */
#endif
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
    LPC_SWM->PINASSIGN[0] |=  ( 6 << 0 );                               /* P0.0 ~ UART0_RXD rf433m recv */
    LPC_SWM->PINASSIGN[0] |=  ( 0 << 8 );                               /* P0.6 ~ UART0_TXD rf433m send */
    
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

void uart0_thread(void)
{
	if (uart0.rflag == 1){
		//myDelay(10);//waiting until the packet done
		

		#if 1
		//debug
		uart0_sendbuf(uart0.rbuf, uart0.rindex);
		CLEAR_UART(&uart0);
		return;
		#endif

		CLEAR_UART(&uart0);
		//CLEAR_PACKET(&server);
	}
}

/*TDD: Testing Driven Develop*/

void gpio_init_beep(void)
{
	gpio_dir(GPIO_BEEP, GPIO_OUTPUT);
    gpio_ctrl(GPIO_BEEP, GPIO_LOW);
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
	myDelay(1000);
	hwapi01_beep_crtl(0);//beep off
	myDelay(1000);
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
	myDelay(1000);
	hwapi02_led1_ctrl(0);//led off
	hwapi02_led2_ctrl(0);//led off
	hwapi02_led3_ctrl(0);//led off	
	myDelay(1000);
}

void gpio_init_rf433m_mode(void)
{
	gpio_dir(GPIO_M0, GPIO_OUTPUT);
	gpio_dir(GPIO_M1, GPIO_OUTPUT);

	gpio_ctrl(GPIO_M1, GPIO_LOW);
	gpio_ctrl(GPIO_M0, GPIO_HIGH);//wakeup mode, to wakeup the lock
}

void hwapi03_rf433m_mode(u8 mode)
{
	switch (mode){
		case 0://normal mode
			gpio_ctrl(GPIO_M1, GPIO_LOW);
			gpio_ctrl(GPIO_M0, GPIO_LOW);
			break;
		case 1://wakeup mode
			gpio_ctrl(GPIO_M1, GPIO_LOW);
			gpio_ctrl(GPIO_M0, GPIO_HIGH);
			break;
		case 2://sleep mode
			gpio_ctrl(GPIO_M1, GPIO_HIGH);
			gpio_ctrl(GPIO_M0, GPIO_LOW);
			break;
		case 3://config mode
			gpio_ctrl(GPIO_M1, GPIO_HIGH);
			gpio_ctrl(GPIO_M0, GPIO_HIGH);
			break;
		default:
			gpio_ctrl(GPIO_M1, GPIO_LOW);
			gpio_ctrl(GPIO_M0, GPIO_LOW);
			break;
	}
}


void test_hwapi03_rf433m_mode(void)
{
	hwapi03_rf433m_mode(0);
	myDelay(1000);
	hwapi03_rf433m_mode(3);
	myDelay(1000);	
}


int main(void)
{
	//myDelay(1000);//system poweron	
	
    SystemInit();                                                       /* 初始化目标板，切勿删除       */
    GPIOInit();                                                         /* GPIO初始化                   */

	WKTInit();

	UART0Init();


	
    while (1) {
		uart0_thread();


		//test_hwapi01_beep_crtl();
		//test_hwapi02_led_ctrl();
		//test_hwapi03_rf433m_mode();
    }
}

/*********************************************************************************************************
  End Of File
*********************************************************************************************************/
