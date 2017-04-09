#include "main.h"

extern uart_t uart0;
extern rfac_u gw_addr_channel;

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
		case RF_NORMAL_MODE://normal mode
			gpio_ctrl(GPIO_M1, GPIO_LOW);
			gpio_ctrl(GPIO_M0, GPIO_LOW);
			break;
		case RF_WAKEUP_MODE://wakeup mode
			gpio_ctrl(GPIO_M1, GPIO_LOW);
			gpio_ctrl(GPIO_M0, GPIO_HIGH);
			break;
		case RF_SLEEP_MODE://sleep mode
			gpio_ctrl(GPIO_M1, GPIO_HIGH);
			gpio_ctrl(GPIO_M0, GPIO_LOW);
			break;
		case RF_CONFIG_MODE://config mode
			gpio_ctrl(GPIO_M1, GPIO_HIGH);
			gpio_ctrl(GPIO_M0, GPIO_HIGH);
			break;
		default://normal mode
			gpio_ctrl(GPIO_M1, GPIO_LOW);
			gpio_ctrl(GPIO_M0, GPIO_LOW);
			break;
	}
}


void test_hwapi03_rf433m_mode(void)
{
	hwapi03_rf433m_mode(0);
	delay_ms(1000);
	hwapi03_rf433m_mode(3);
	delay_ms(1000);	
}

void hwapi07_mod_uart0_baud(u32 baud)
{
	
    LPC_SWM->PINASSIGN[0] &= ~( 0xFFFF << 0 );
    LPC_SWM->PINASSIGN[0] |=  ( 0 << 0 );                               /* P0.0 ~ UART0_RXD rf433m recv */
    LPC_SWM->PINASSIGN[0] |=  ( 6 << 8 );                               /* P0.6 ~ UART0_TXD rf433m send */

    LPC_SYSCON->UARTCLKDIV     = 1;                                     /* UART时钟分频值为 1           */
    LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 14);                             /* 初始化UART AHB时钟           */

    LPC_USART0->BRG = SystemCoreClock * LPC_SYSCON->SYSAHBCLKDIV /
                      (LPC_SYSCON->UARTCLKDIV * 16 * baud) - 1;     /* 串口通信波特率               */
    LPC_USART0->CFG = (1 << 0) |                                        /* 使能UART                     */
                      (1 << 2) |                                        /* 8位数据位                    */
                      (0 << 4) |                                        /* 无校验                       */
                      (0 << 6);                                         /* 1位停止位                    */

	UART0RecvEnable();                        
    NVIC_EnableIRQ(UART0_IRQn);	

	CLEAR_UART(&uart0);
}


void hwapi07_rf433m_get_addr_channel(void)
{
	u8 cmd_buf[3]={0xc1,0xc1,0xc1};	
	u8 flag = RF_GET_GATEWAY_ADDR_FAIL;


	if (EE_SUCCESS == eepromRead(EEPROM_ADDR_GATEWAY_RF433M_READ_CONFIG_FLAG, &flag, sizeof(u8)) && flag == RF_GET_GATEWAY_ADDR_SUCCESS){

		if (EE_SUCCESS != eepromRead(EEPROM_ADDR_GATEWAY_RF433M_ADDR_CHANNEL, gw_addr_channel.rfac0, sizeof(rfac_u))){
			eepromRead(EEPROM_ADDR_GATEWAY_RF433M_ADDR_CHANNEL, gw_addr_channel.rfac0, sizeof(rfac_u));
		}
		return;
	}

	CLEAR_TYPE(&gw_addr_channel, rfac_u);

	//mode3
	delay_ms(500);
	hwapi07_rf433m_mode3_prepare();
	delay_ms(500);
	//send
	uart0_sendbuf(cmd_buf, sizeof(cmd_buf));
	delay_ms(20);

	//recv
	if (uart0.rflag == 1){		
		if (0xc0 == uart0.rbuf[0]){

			memcpy(gw_addr_channel.rfac2.addr, &uart0.rbuf[1], RF_ADDR_SIZE);
			gw_addr_channel.rfac1.channel = uart0.rbuf[6];
			
			flag = RF_GET_GATEWAY_ADDR_SUCCESS;
			if (EE_SUCCESS != eepromWriteNByte(EEPROM_ADDR_GATEWAY_RF433M_ADDR_CHANNEL, gw_addr_channel.rfac0, sizeof(rfac_u))){
				eepromWriteNByte(EEPROM_ADDR_GATEWAY_RF433M_ADDR_CHANNEL, gw_addr_channel.rfac0, sizeof(rfac_u));
			}
			if (EE_SUCCESS != eepromWriteNByte(EEPROM_ADDR_GATEWAY_RF433M_READ_CONFIG_FLAG, &flag, sizeof(u8))){
				eepromWriteNByte(EEPROM_ADDR_GATEWAY_RF433M_READ_CONFIG_FLAG, &flag, sizeof(u8));
			}
		}
		//debug log
		send2server(gw_addr_channel.rfac0, sizeof(rfac_u));
		
		CLEAR_UART_RECV(&uart0);
	}else{
		send2server(gw_addr_channel.rfac0, sizeof(rfac_u));
		CLEAR_UART_RECV(&uart0);
	}	
}

void hwapi07_rf433m_set_config(void)
{
	u8 cmd2_buf[8]={0xc0,0x00,0x00,0x00,0x01,0x3a,0x11,0xdc};
	uart0_sendbuf(cmd2_buf, sizeof(cmd2_buf));
}


void hwapi07_rf433m_reset(void)
{
	u8 cmd3_buf[3]={0xc3,0xc3,0xc3};
	uart0_sendbuf(cmd3_buf, sizeof(cmd3_buf));
}



//mode3
void hwapi07_rf433m_mode3_prepare(void)
{	
	hwapi03_rf433m_mode(RF_CONFIG_MODE);
	hwapi07_mod_uart0_baud(UART0_BPS_CONFIG_RF433M);
}



void test_hwapi07_rf433m_set_config(void)
{
	delay_ms(1000);
	hwapi07_rf433m_set_config();
	//delay_ms(100);
}

//input IRQ
void test_rf433m_aux(void)
{
	
}


//mode1
void hwapi08_rf433m_mode1_prepare(void)
{	
	hwapi03_rf433m_mode(RF_WAKEUP_MODE);
	hwapi07_mod_uart0_baud(UART0_BPS);
}


void hwapi08_rf433m_send(u8 *addr_buf, u8 channel, u8 *data_buf, u8 data_size)
{
	u8 buf[5+32]={0};
	//set mode , optional
	
	memcpy(buf, addr_buf, 4);
	memcpy(buf+4, &channel, 1);

	//if (data_size <= 32){
		memcpy(buf+5, data_buf, data_size);
	//}

	uart0_sendbuf(buf,sizeof(buf));
}


//mode1: work
void test_rf433m_mode1_transport(void)
{
	u8 addr_buf[4]={0x00, 0x00, 0x00,0x02};
	u8 channel = 0x28;
	u8 data_buf[32]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07, 0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
					 0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17, 0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f};
	u8 data_size = 32;


	hwapi08_rf433m_mode1_prepare();
	hwapi08_rf433m_send(addr_buf, channel,data_buf,data_size);
	delay_ms(1000);
}

//mode0

void hwapi08_rf433m_mode0_prepare(void)
{	
	hwapi03_rf433m_mode(RF_NORMAL_MODE);
	hwapi07_mod_uart0_baud(UART0_BPS);
}

void test_rf433m_mode0_transport(void)
{
	u8 addr_buf[4]={0x00, 0x00, 0x00,0x02};
	u8 channel = 0x28;
	u8 data_buf[32]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07, 0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
					 0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17, 0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f};
	u8 data_size = 32;


	hwapi08_rf433m_mode0_prepare();
	hwapi08_rf433m_send(addr_buf, channel,data_buf,data_size);
	delay_ms(1000);
}


