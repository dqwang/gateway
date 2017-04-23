#include "main.h"

#include "gprs.h"
#include "timer.h"


gprs_t gprs;
extern uart_t uart1;

void gprs_reg(void)
{
	u8 reg_cmd[]="AT+CREG?\r\n";
	u8 i=0;
	gprs.gprs_flag = GPRS_FLAG0_NONE;

	do{
		CLEAR_UART(&uart1);
		GPRS_SEND_CMD(reg_cmd);
		delay_ms(5000);
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
		delay_ms(1000);

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
		delay_ms(1000);

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
		delay_ms(3000);
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
		delay_ms(3000);

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
		delay_ms(1000);

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
		delay_ms(1000);
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

		hwapi02_led3_ctrl(ON);
	}
}

void gprs_close_tcp(void)
{
	//u8 close_tcp_cmd[]="AT+CIPCLOSE\r\n";
}

void gprs_init(void)
{
	CLEAR_GPRS(&gprs);

	gpio_dir(GPIO_GPRS_RESET, GPIO_OUTPUT);
	gpio_ctrl(GPIO_GPRS_RESET, GPIO_HIGH);
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
		delay_ms(10);//waiting until the packet done

		uart1_sendbuf(uart1.rbuf, uart1.rindex);

		uart1.rflag = 0;
		uart1.rindex = 0;
	}
}


