#ifndef __GPRS_H__
#define __GPRS_H__

#include "main.h"
#include "errno.h"

#define GPIO_GPRS_RESET		(1 << 23)//RESET P0.23
#define GPIO_GPRS_INT		(1 << 14)//INT P0.14


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

void gprs_init(void);
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

#endif
