#include "main.h"

/*TDD: Testing Driven Develop*/



int main(void)
{
	//delay_ms(1000);
    SystemInit();                                                       /* 初始化目标板，切勿删除       */
	delay_ms(100);//system poweron,for gprs

    GPIOInit();                                                         /* GPIO初始化                   */
	WKTInit();

	UART0Init();
	UART1Init();
	UART2Init();
	
	queue_init();
	

	
	hwapi07_rf433m_get_addr_channel();


	lock_addr_channel_array_init();
	
	//gprs_init();

	//test_hwapi05_wifi_factory();

	
	
	//hwapi08_rf433m_mode1_prepare();

	
	hwapi08_rf433m_mode0_prepare();

	
	//test_eeprom();
	
	
    while (1) {
		
		//uart0_thread();
		//uart2_thread();
		//test_server_packet_union();
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

		//test_gprs();

		//test_hwapi07_rf433m_get_config();
		//test_hwapi07_rf433m_set_config();
		//test_rf433m_mode1_transport();
		//test_rf433m_mode0_transport();

		system_work_led_thread();

		handle_server_packet_thread();
		handle_lock_packet_thread();
    }
}

