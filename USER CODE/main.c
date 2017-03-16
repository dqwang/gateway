#include "main.h"

/******************************************************************************/
#define GPIO_LED1	(1 << 17)//LED1 P0.17
#define GPIO_LED2	(1 << 18)//LED2 P0.18
#define GPIO_LED3	(1 << 19)//LED3 P0.19
#define GPIO_BEEP	(1 << 12)//beep P0.12

#define GPIO_M0		(1 << 7)//RF_CTL1 P0.7
#define GPIO_M1		(1 << 8)//RF_CTL2 P0.8
#define GPIO_AUX 	(1 << 9)//RF_CTL3 P0.9


#define GPIO_WIFI_MODE_WPS	(1 << 24)//MODE/WPS P0.24
#define GPIO_WIFI_SLEEP		(1 << 25)//SLEEP P0.25
#define GPIO_WIFI_FACTORY	(1 << 26)//FACTORY P0.26
#define GPIO_WIFI_RESET 	(1 << 27)//RESET P0.27

#define GPIO_GPRS_RESET		(1 << 23)//RESET P0.23
#define GPIO_GPRS_INT		(1 << 14)//INT P0.14


#define UART0_BPS   115200/*433M*/
#define UART1_BPS	115200/*GPRS*/
#define UART2_BPS	115200/*WIFI and RJ45*/


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
uart_t uart1;
uart_t uart2;




//#define DEBUG_LOG(buf, len) uart0_sendbuf(buf, len)
#define DEBUG_LOG(buf, len)


#define send2server(buf,len) uart2_sendbuf(buf, len)

#define CLEAR_UART(p) memset(p,0x0,sizeof(uart_t))


typedef enum {
	   GPRS_FLAG0_NONE =0,

	   GPRS_FLAG1_REG,
	   GPRS_FLAG2_CLOSE_ECHO,
	   GPRS_FLAG3_GATT,
	   GPRS_FLAG4_GACT,

	   GPRS_FLAG5_CONNECT_TCP,
	   GPRS_FLAG6_TRS_CONFIG,
	   GPRS_FLAG7_TRS_OPEN,
	   GPRS_FLAG8_USER_DATA
}gprs_flag_e;

typedef struct gprs_{
	gprs_flag_e gprs_flag;
	//u8 *gprs_cmd[];
	//u8 *gprs_ack[];
}gprs_t;

#define sizeofstr(str) (sizeof(str)-1)
#define GPRS_SEND_CMD(cmd_buf) uart1_sendbuf(cmd_buf, sizeofstr(cmd_buf))
#define CLEAR_GPRS(p) memset(p,0x0,sizeof(gprs_t))

gprs_t gprs;


/*****************************function prototype*******************************/

void GPIOInit (void);
void gpio_ctrl(uint32_t gpio, uint32_t value);
void gpio_dir(uint32_t gpio, uint32_t dir);

void gpio_init_beep(void);
void gpio_init_led(void);
void gpio_init_rf433m_mode(void);
void gpio_init_wifi(void);

void gprs_init(void);



void hwapi01_beep_crtl(u8 on_off);
void hwapi02_led1_ctrl(u8 on_off);
void hwapi02_led2_ctrl(u8 on_off);
void hwapi02_led3_ctrl(u8 on_off);
void hwapi03_rf433m_mode(u8 mode);
void hwapi04_wifi_reset(void);
void hwapi05_wifi_factory(void);
void hwapi06_rj45_reset(void);


void gprs_trs_config(void);
void gprs_connect_tcp(void);
void gprs_act(void);
void gprs_att(void);
void gprs_close_echo(void);
errno_t check_gprs_cmd_ack(u8 *ok, u8 *ack_from_gprs);
void gprs_reg(void);
void gprs_trs_open(void);
void gprs_user_data(void);
void gprs_close_tcp(void);
void gprs_reconnect_server(void);


void test_hwapi01_beep_crtl(void);
void test_hwapi02_led_ctrl(void);
void test_hwapi03_rf433m_mode(void);
void test_hwapi04_wifi_reset(void);
void test_hwapi05_wifi_factory(void);
void test_hwapi06_rj45_reset(void);


void test_wifi_uart2(void);
void test_rj45_uart2(void);

void test_gprs(void);


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
	gpio_init_wifi();
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
    LPC_SWM->PINASSIGN[2] &= ~( 0xFFFF << 16 );
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


void test_uart0_echo(void)
{
	if (uart0.rflag == 1){
		//myDelay(10);//waiting until the packet done
		uart0_sendbuf(uart0.rbuf, uart0.rindex);
		CLEAR_UART(&uart0);
	}
}


void test_uart1_echo(void)
{
	if (uart1.rflag == 1){
		//myDelay(10);//waiting until the packet done

		uart1_sendbuf(uart1.rbuf, uart1.rindex);
		CLEAR_UART(&uart1);
	}
}

void test_uart2_echo(void)
{
	if (uart2.rflag == 1){
		//myDelay(10);//waiting until the packet done
		uart2_sendbuf(uart2.rbuf, uart2.rindex);
		CLEAR_UART(&uart2);
	}
}

void test_uart0_send(void)
{
	u8 sbuf[1]={0x01};
	uart0_sendbuf(sbuf,sizeof(sbuf));
	myDelay(1);
}

void test_uart1_send(void)
{
	u8 sbuf[1]={0x01};
	uart1_sendbuf(sbuf,sizeof(sbuf));
	myDelay(1);
}

void test_uart2_send(void)
{
	u8 sbuf[1]={0x01};
	uart2_sendbuf(sbuf,sizeof(sbuf));
	myDelay(1);
}

void gpio_init_wifi(void)
{
	gpio_dir(GPIO_WIFI_FACTORY, GPIO_OUTPUT);
	gpio_dir(GPIO_WIFI_RESET, GPIO_OUTPUT);
	gpio_dir(GPIO_WIFI_MODE_WPS, GPIO_OUTPUT);
	gpio_dir(GPIO_WIFI_SLEEP, GPIO_OUTPUT);

	gpio_ctrl(GPIO_WIFI_FACTORY, GPIO_HIGH);
	gpio_ctrl(GPIO_WIFI_RESET, GPIO_HIGH);
	gpio_ctrl(GPIO_WIFI_MODE_WPS, GPIO_HIGH);
	gpio_ctrl(GPIO_WIFI_SLEEP, GPIO_HIGH);

	test_hwapi04_wifi_reset();
}

// invoke this reset function if you need to reconnect server
// 考虑时间是否该用定时器实现
void hwapi04_wifi_reset(void)
{
	gpio_ctrl(GPIO_WIFI_RESET, GPIO_LOW);
	myDelay(20);//must be more than 10ms
	gpio_ctrl(GPIO_WIFI_RESET, GPIO_HIGH);
}

void test_hwapi04_wifi_reset(void)
{
	myDelay(1000);
	hwapi04_wifi_reset();
	myDelay(1000);
}

//需增加flash flag, 记录是否已经恢复过出厂设置。
void hwapi05_wifi_factory(void)
{
	gpio_ctrl(GPIO_WIFI_FACTORY, GPIO_LOW);
	myDelay(3500);//must be more than 3s
	gpio_ctrl(GPIO_WIFI_FACTORY, GPIO_HIGH);
}

void test_hwapi05_wifi_factory(void)
{
	hwapi05_wifi_factory();
}

void test_wifi_uart2(void)
{
	u8 sbuf[1]={0x0a};
	uart2_sendbuf(sbuf,sizeof(sbuf));
	myDelay(1);
	//checkpoint
	//signal should be ok in hardware
	//PC rak415 tool should receive the data from mcu
	//next todo: mcu should receive data from PC
}

//USR-TCP232-T
void hwapi06_rj45_reset(void)
{
	gpio_ctrl(GPIO_WIFI_RESET, GPIO_LOW);
	myDelay(250);//must be more than 200ms
	gpio_ctrl(GPIO_WIFI_RESET, GPIO_HIGH);
}

void test_hwapi06_rj45_reset(void)
{
	myDelay(1000);
	hwapi06_rj45_reset();
	myDelay(1000);
}

void test_rj45_uart2(void)
{
	u8 sbuf[1]={0x0b};
	uart2_sendbuf(sbuf,sizeof(sbuf));
	myDelay(1);
}

void gprs_reg(void)
{
	u8 reg_cmd[]="AT+CREG?\r\n";
	u8 i=0;
	gprs.gprs_flag = GPRS_FLAG0_NONE;

	do{
		CLEAR_UART(&uart1);
		GPRS_SEND_CMD(reg_cmd);
		myDelay(5000);
		for(i=0;i<uart1.rindex;i++){
			if (uart1.rbuf[i] == ':'){
				if (uart1.rbuf[i+2] == '1' && uart1.rbuf[i+4] == '1'){
					gprs.gprs_flag = GPRS_FLAG1_REG;
					// 语音提示 GPRS注册成功
					break;
				}
			}
		}
	}while(gprs.gprs_flag == GPRS_FLAG0_NONE);
}

errno_t check_gprs_cmd_ack(u8 *ok, u8 *ack_from_gprs)
{
	u8 i=0;

	for (i=0;i<sizeofstr(ok);i++){
		if (ok[i] != ack_from_gprs[i]){
			return E_INVALID_PACKET;
		}
	}
	return EOK;
}

void gprs_close_echo(void)
{
	u8 close_echo_cmd[]="ATE0\r\n";
	u8 close_echo_ok[]="ATE0\r\n\r\nOK\r\n";

	do{
		CLEAR_UART(&uart1);
		GPRS_SEND_CMD(close_echo_cmd);
		myDelay(1000);

		if (EOK == check_gprs_cmd_ack(close_echo_ok, uart1.rbuf)){
			gprs.gprs_flag = GPRS_FLAG2_CLOSE_ECHO;
			// 语音提示 GPRS关闭回显功能成功
			break;
		}
	}while(gprs.gprs_flag == GPRS_FLAG1_REG);
}

void gprs_att(void)
{
	u8 att_cmd[]="AT+CGATT=1\r\n";
	u8 att_ok[]="\r\nOK\r\n";

	do{
		CLEAR_UART(&uart1);
		GPRS_SEND_CMD(att_cmd);
		myDelay(1000);

		if (EOK == check_gprs_cmd_ack(att_ok, uart1.rbuf)){
			gprs.gprs_flag = GPRS_FLAG3_GATT;
			// 语音提示 GPRS附着成功
			break;
		}
	}while(gprs.gprs_flag == GPRS_FLAG2_CLOSE_ECHO);
}

void gprs_act(void)
{
	u8 act_cmd[]="AT+CGACT=1,1\r\n";
	u8 act_ok[]="\r\nOK\r\n";

	do{
		CLEAR_UART(&uart1);
		GPRS_SEND_CMD(act_cmd);
		myDelay(3000);
		if (EOK == check_gprs_cmd_ack(act_ok, uart1.rbuf)){
			gprs.gprs_flag = GPRS_FLAG4_GACT;
			// 语音提示 GPRS PDP激活成功
			break;
		}
	}while(gprs.gprs_flag == GPRS_FLAG3_GATT);
}

void gprs_connect_tcp(void)
{
	u8 connect_tcp_cmd[]="AT+CIPSTART=TCP,120.55.117.108,10000\r\n";
	u8 connect_tcp_ok[]="\r\nCONNECT OK\r\n\r\nOK\r\n";

	do{
		CLEAR_UART(&uart1);
		GPRS_SEND_CMD(connect_tcp_cmd);
		myDelay(3000);

		if (EOK == check_gprs_cmd_ack(connect_tcp_ok, uart1.rbuf)){
			gprs.gprs_flag = GPRS_FLAG5_CONNECT_TCP;
			// 语音提示 GPRS 连接TCP服务器
			break;
		}
	}while(gprs.gprs_flag == GPRS_FLAG4_GACT);
}


void gprs_trs_config(void)
{
	u8 trs_config_cmd[]="AT+CIPTCFG=2,10\r\n";// 10 bytes at least per packet
	u8 trs_config_ok[]="\r\nOK\r\n";

	do{
		CLEAR_UART(&uart1);
		GPRS_SEND_CMD(trs_config_cmd);
		myDelay(1000);

		if (EOK == check_gprs_cmd_ack(trs_config_ok, uart1.rbuf)){
			gprs.gprs_flag = GPRS_FLAG6_TRS_CONFIG;
			// 语音提示 GPRS 透传设置参数

			break;
		}
	}while(gprs.gprs_flag == GPRS_FLAG5_CONNECT_TCP);
}

void gprs_trs_open(void)
{
	u8 trs_open_cmd[]="AT+CIPTMODE=1\r\n";
	u8 trs_open_ok[]="\r\nOK\r\n";

	do{
		CLEAR_UART(&uart1);
		GPRS_SEND_CMD(trs_open_cmd);
		myDelay(1000);
		if (EOK == check_gprs_cmd_ack(trs_open_ok, uart1.rbuf)){
			gprs.gprs_flag = GPRS_FLAG7_TRS_OPEN;
			// 语音提示 GPRS 透传模式开启
			break;
		}
	}while(gprs.gprs_flag == GPRS_FLAG6_TRS_CONFIG);
}

void gprs_user_data(void)
{
	if (gprs.gprs_flag == GPRS_FLAG7_TRS_OPEN){
		gprs.gprs_flag = GPRS_FLAG8_USER_DATA;
		//next to uart1_thread()


	}
}

void gprs_close_tcp(void)
{
	u8 close_tcp_cmd[]="AT+CIPCLOSE\r\n";
}

void gprs_init(void)
{
	CLEAR_GPRS(&gprs);
	gprs_reg();
	gprs_close_echo();
	gprs_att();
	gprs_act();

	gprs_connect_tcp();
	gprs_trs_config();
	gprs_trs_open();

	gprs_user_data();
}

void gprs_reconnect_server(void)
{
	gprs.gprs_flag = GPRS_FLAG4_GACT;

	gprs_connect_tcp();
	gprs_trs_config();
	gprs_trs_open();

	gprs_user_data();
}



void test_gprs(void)
{
	if (uart1.rflag == 1 && gprs.gprs_flag == GPRS_FLAG8_USER_DATA){
		myDelay(10);//waiting until the packet done

		uart1_sendbuf(uart1.rbuf, uart1.rindex);

		uart1.rflag = 0;
		uart1.rindex = 0;
	}
}


int main(void)
{
	myDelay(1000);//system poweron
	
    SystemInit();                                                       /* 初始化目标板，切勿删除       */
    GPIOInit();                                                         /* GPIO初始化                   */

	WKTInit();

	UART0Init();
	UART1Init();
	UART2Init();

	gprs_init();
	//test_hwapi05_wifi_factory();
    while (1) {
		//uart0_thread();


		//test_hwapi01_beep_crtl();
		//test_hwapi02_led_ctrl();
		//test_hwapi03_rf433m_mode();
		//test_uart0_echo();
		//test_uart1_echo();
		//test_uart2_echo();
		//test_uart0_send();
		//test_uart1_send();
		//test_uart2_send();

		//test_hwapi04_wifi_reset();
		//test_wifi_uart2();

		//test_hwapi06_rj45_reset();
		//test_rj45_uart2();

		test_gprs();
    }
}

/*********************************************************************************************************
  End Of File
*********************************************************************************************************/
