#include "main.h"

rfac_u gw_addr_channel;
rfac_u lock_addr_channel_array[LOCK_MAX_NUM_PER_GATEWAY];



extern uart_t uart0;
extern uart_t uart1;
extern uart_t uart2;

extern Queue sq;//server queue
extern Queue lq;//lock queue

errno_t is_valid_packet_from_server(u8 *in_buf)
{
	//header tail length
	u8 tail[2] = {0x0D, 0x0A};
	
	if (0 == memcmp(in_buf, gw_addr_channel.rfac0, sizeof(rfac_u)) && 0 == memcmp(in_buf + SERVER_PACKET_SIZE -2, tail, 2))
		return EOK;
	else
		return E_INVALID_PACKET;
}

/*----------------------------------------------------------------------------*/

void hwapi09_ack_error_to_server(errno_t errno)
{
	//todo
	sp_u sp;

	CLEAR_TYPE(&sp, sp_u);
	
	memcpy(sp.sp0, gw_addr_channel.rfac0, sizeof(rfac_u));
	memset(sp.sp0 + sizeof(rfac_u), (u8)errno, LOCK_PACKET_SIZE);
	SH_PUT_2_BYTE(sp.sp2.tail,TAIL_ENTER);
	
	send2server(sp.sp0, SERVER_PACKET_SIZE);
}

errno_t hwapi09_decode_packet_from_server(u8 *in_buf, sp_u *out_sp)
{

	if (NULL == in_buf || NULL == out_sp){
		return 	E_NULL_POINTER;
	}
	
	if (EOK == is_valid_packet_from_server(in_buf)){
		memcpy(out_sp->sp0, in_buf, sizeof(sp_u)); //decode
		return EOK;
	}else{
		return E_INVALID_PACKET;
	}	
}

void wait_lock_ack(void)
{
	
}

errno_t hwapi09_encode_packet_to_lock(sp_u *in_sp)
{
	lpkt_u lpkt;

	if (NULL == in_sp){
		return E_NULL_POINTER;
	}

	memcpy(lpkt.lpkt0, in_sp->sp2.lpkt.lpkt0, sizeof(lpkt_u));
	send2lock(lpkt.lpkt0, sizeof(lpkt_u));

	return EOK;
}

errno_t is_heartbeat_from_server(sp_u *p_sp)
{
	if (p_sp->sp1.lpkt.lpkt1.lpro.lpro1.cmd == CMD_UPDATE_HEARTBEAT_TIME){
		return EOK;
	}else{
		return E_NOT_HEARTBEAT_CMD;
	}	
}

void send_heartbeat_to_server(void)
{
	sp_u sp;
	
	lpp_u lpp;
	
	u8 time[TIME_SIZE]={0x20,0x17,0x04,0x13,0x19,0x35};	
	u8 room_addr[ROOM_ADDR_SIZE] = {0X00,0X00,0X01,0X01,0X01,0X01,0X02,0X01};
	u16 crc=0;

	//lpp
	memcpy(lpp.lpp_CMD_REPORT_HEARTBEAT_TIME.time1, time, TIME_SIZE);
	lpp.lpp_CMD_REPORT_HEARTBEAT_TIME.heartlen2 = 1;//minute
	lpp.lpp_CMD_REPORT_HEARTBEAT_TIME.cardver3 = 1;
	lpp.lpp_CMD_REPORT_HEARTBEAT_TIME.cardnum4 = 3;
	lpp.lpp_CMD_REPORT_HEARTBEAT_TIME.keyver5 = 5;
	lpp.lpp_CMD_REPORT_HEARTBEAT_TIME.livestate6 = 1;
	memcpy(lpp.lpp_CMD_REPORT_HEARTBEAT_TIME.room_addr7, room_addr,ROOM_ADDR_SIZE);
	lpp.lpp_CMD_REPORT_HEARTBEAT_TIME.wait_time8 = 3;
	memset(lpp.lpp_CMD_REPORT_HEARTBEAT_TIME.reserved9,0x00, 2);
	
	//sp
	memcpy(sp.sp0, gw_addr_channel.rfac0, sizeof(rfac_u));
	
	sp.sp1.lpkt.lpkt1.lock_addr = 0;
	sp.sp1.lpkt.lpkt1.lock_channel = 0;

	SH_PUT_4_BYTE(sp.sp1.lpkt.lpkt1.lpro.lpro2.rnd, 0X12345678);
	SH_PUT_2_BYTE(sp.sp1.lpkt.lpkt1.lpro.lpro2.header, LOCK_HEADER_5FE6);
	sp.sp1.lpkt.lpkt1.lpro.lpro1.len = PAYLOAD_LEN_HEARTBEAT_TIME;
	sp.sp1.lpkt.lpkt1.lpro.lpro1.cmd = CMD_REPORT_HEARTBEAT_TIME;

	memcpy(sp.sp1.lpkt.lpkt1.lpro.lpro2.lpp.lpp0, lpp.lpp0, sizeof(lpp_u));

	crc = crc16(sp.sp1.lpkt.lpkt0, LOCK_PACKET_SIZE - LOCK_CRC_SIZE);
	SH_PUT_2_BYTE(sp.sp1.lpkt.lpkt2.lpro.lpro2.crc,crc);

	SH_PUT_2_BYTE(sp.sp2.tail, TAIL_ENTER);

	//send
	send2server(sp.sp0, sizeof(sp_u));
	
}

extern timer_t heartbeat_timer;
extern timer_t heartbeat_server_timer;

void heartbeat_thread(void)
{
	if (heartbeat_timer.flag == 1){
		
		heartbeat_timer.flag = 0;
		heartbeat_server_timer.flag = 1;
		send_heartbeat_to_server();
		delay_ms(10);
		//uart1.slen = 0;//tbd
	}

/*
	if (heartbeat_server_timer.flag == 2){
		heartbeat_server_timer.flag = 0;
		heartbeat_server_timer.cnt = 0;

		gprs_reconnect_server();
	}
*/
}

void test_heartbeat_without_timer_irq(void)
{
	send_heartbeat_to_server();
	delay_ms(5000);
}

void hwapi09_handle_packet_from_server(u8 *in_buf)
{
	sp_u    sp;
	errno_t ret = EOK;

	if (NULL == in_buf){
		return;
	}
	
	ret = hwapi09_decode_packet_from_server(in_buf, &sp);
	if (ret != EOK)
		goto ACK_ERROR_TO_SERVER;


	//heartbeat between server and gateway, not forward to lock
	
	if (is_heartbeat_from_server(&sp) == EOK){
		//debug
		//hwapi01_beep_cnt(1, 100);
		hwapi02_led2_ctrl(ON);
		//return;
	}
	
	ret = hwapi09_encode_packet_to_lock(&sp);
	if (ret != EOK)
		goto ACK_ERROR_TO_SERVER;
	
	return;
	
ACK_ERROR_TO_SERVER:
	hwapi09_ack_error_to_server(ret);
}

/*----------------------------------------------------------------------------*/

void handle_server_packet_thread(void)
{
	u16 i=0;

#if 0
	if (uart2.rflag == 1){
	
		delay_ms(WIFI_RECV_TIME);/*wifi 模块全功耗模式大概10ms接收完毕，若低功耗模式可能需要200ms*/

		for (i=0; i<UART_RBUF_SIZE-SERVER_PACKET_SIZE+1; i++){
			if (EOK == is_valid_packet_from_server(uart2.rbuf+i)){
				Enqueue(&sq, i);				
			}
		}
		while (!is_queue_empty(&sq)){
			hwapi09_handle_packet_from_server(uart2.rbuf + front(&sq));
			Dequeue(&sq);
			delay_ms(RF_SEND_DELAY_TIME);//send
		}
		CLEAR_UART(&uart2);
	}
#else
	if (uart1.rflag == 1){
		delay_ms(GPRS_RECV_TIME);

		for (i=0; i<UART_RBUF_SIZE-SERVER_PACKET_SIZE+1; i++){
			if (EOK == is_valid_packet_from_server(uart1.rbuf+i)){
				Enqueue(&sq, i);				
			}
		}

		while (!is_queue_empty(&sq)){
			hwapi09_handle_packet_from_server(uart1.rbuf + front(&sq));
			Dequeue(&sq);
			delay_ms(RF_SEND_DELAY_TIME);//send
			
			hwapi02_led2_ctrl(OFF);// when recv heartbeat, led2 flash once
		}
		
		CLEAR_UART(&uart1);
	}
#endif
}

/*----------------------------------------------------------------------------*/


void lock_addr_channel_array_init(void)
{
	//todo 从服务器下发，存在flash中。
	//todo read from the flash
	rfac_u lock1_addr_channel;

	SH_PUT_4_BYTE(lock1_addr_channel.rfac2.addr, 0x00000002);
	lock1_addr_channel.rfac1.channel = 0x28;

	memset(lock_addr_channel_array,0xff, sizeof(lock_addr_channel_array));
	
	memcpy(lock_addr_channel_array, lock1_addr_channel.rfac0, sizeof(rfac_u));

	//todo
}

errno_t is_valid_packet_from_lock(u8 *in_buf)
{
	//header length
	u8 i=0;

	for (i=0;i<LOCK_MAX_NUM_PER_GATEWAY;i++){
		if (0 == memcmp(in_buf, lock_addr_channel_array[i].rfac0, sizeof(rfac_u)))//todo, the tail != 0
			//&& !(in_buf[LOCK_PACKET_SIZE -2] ==0 && in_buf[LOCK_PACKET_SIZE -1] ==0))//For new 433m module
		{
			return EOK;
		}
	}
	
	return E_INVALID_PACKET;
}

errno_t hwapi10_decode_packet_from_lock(u8 *in_buf, lpkt_u *out_lp)
{
	if (NULL == in_buf || NULL == out_lp){
		return E_NULL_POINTER;
	}

	//decode
	if (EOK == is_valid_packet_from_lock(in_buf)){
		memcpy(out_lp->lpkt0, in_buf, sizeof(lpkt_u));
		return EOK;
	}else{
		return E_INVALID_PACKET;
	}
}

errno_t hwapi10_encode_packet_to_server(lpkt_u *in_lp)
{
	sp_u sp;

	if (NULL == in_lp){
		return E_NULL_POINTER;
	}

	//encode
	memcpy(sp.sp0, gw_addr_channel.rfac0, sizeof(rfac_u));
	memcpy(sp.sp2.lpkt.lpkt0, in_lp->lpkt0, sizeof(lpkt_u));		
	SH_PUT_2_BYTE(sp.sp2.tail, TAIL_ENTER);

	//send
	send2server(sp.sp0, sizeof(sp_u));
	
	return EOK;
}

void hwapi10_ack_error_to_lock(errno_t error, rfac_u *lock_addr_channel)
{
	lpkt_u lp;

	CLEAR_TYPE(&lp, lpkt_u);

	memcpy(lp.lpkt2.lock_addr, lock_addr_channel->rfac2.addr, sizeof(RF_ADDR_SIZE));
	lp.lpkt1.lock_channel = lock_addr_channel->rfac1.channel;
	memset(lp.lpkt0+sizeof(rfac_u), (u8)error, LOCK_PROTOCOL_SIZE);

	send2lock(lp.lpkt0, LOCK_PACKET_SIZE);
}


void hwapi10_handle_packet_from_lock(u8 *in_buf)
{
	lpkt_u lp;
	errno_t ret = EOK;

	if (NULL == in_buf){
		return;
	}

	ret = hwapi10_decode_packet_from_lock(in_buf, &lp);
	if(EOK != ret){
		goto ACK_ERROR_TO_LOCK;
	}

	ret = hwapi10_encode_packet_to_server(&lp);
	if (EOK != ret){
		goto ACK_ERROR_TO_LOCK;
	}

	return;

ACK_ERROR_TO_LOCK:
	hwapi10_ack_error_to_lock(ret, (rfac_u *)lp.lpkt0);
}


//lock send packet to server needed : more than 120ms
void handle_lock_packet_thread(void)
{
	u16 i=0;

	if (uart0.rflag == 1){
	
		delay_ms(20);/*TBD*/

		for (i=0; i<UART_RBUF_SIZE-LOCK_PACKET_SIZE+1; i++){
			if (EOK == is_valid_packet_from_lock(uart0.rbuf+i)){
				Enqueue(&lq, i);				
			}
		}
		while (!is_queue_empty(&lq)){
			hwapi10_handle_packet_from_lock(uart0.rbuf + front(&lq));
			Dequeue(&lq);
			delay_ms(10);//sendto server time
		}
		CLEAR_UART(&uart0);
	}
}


void test_server_packet_union(void)
{
	sp_u sp_u;

	/*

	wrong order
	01 00 00 00 11 02 00 00 00 28 78 56 34 12 59 56 01 01 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 10 11 12 13 14 15 ff ff 0a 0d 
	right order
	00 00 00 01 11 00 00 00 02 28 12 34 56 78 56 59 01 01 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 10 11 12 13 14 15 ee ff 0d 0a 
	000000011100000002281234567856590101000102030405060708090a0b0c0d0e0f101112131415eeff0d0a
	*/

	/*step 1, encode plaintext*/

	u8 x[LOCK_DATA_PAYLOAD_SIZE_MAX]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21};


	SH_PUT_4_BYTE(sp_u.sp2.gw_addr,0x00000001);
	sp_u.sp2.gw_channel[0]=0x11;
	
	
	SH_PUT_4_BYTE(sp_u.sp2.lpkt.lpkt2.lock_addr, 0x00000002);
	sp_u.sp2.lpkt.lpkt2.lock_channel[0]=0x28;

	SH_PUT_4_BYTE(sp_u.sp2.lpkt.lpkt2.lpro.lpro2.rnd,0x12345678);
	SH_PUT_2_BYTE(sp_u.sp2.lpkt.lpkt2.lpro.lpro2.header,0x5659);
	sp_u.sp2.lpkt.lpkt2.lpro.lpro2.len[0]=0x01;	
	sp_u.sp2.lpkt.lpkt2.lpro.lpro2.cmd[0]=0x01;

	memcpy(sp_u.sp2.lpkt.lpkt2.lpro.lpro2.lpp.lpp0, x, sizeof(x));

	SH_PUT_2_BYTE(sp_u.sp2.lpkt.lpkt2.lpro.lpro2.crc, 0xeeff);	

	SH_PUT_2_BYTE(sp_u.sp2.tail,TAIL_ENTER);


	/*step. send*/
	send2server(sp_u.sp0, sizeof(sp_u));
	delay_ms(10000);
}

const unsigned char auchCRCHi[] = {
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40
};

const char auchCRCLo[] = {
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
0x40
} ;

//Modicon CRC Generation Function
u16 crc16(u8 *puchMsg,  u8 usDataLen)
{
	unsigned char uchCRCHi = 0xff;		//high byte of CRC initialized
	unsigned char uchCRCLo = 0xff;		//low byte of CRC initialized
	unsigned char uchIndex;

	while(usDataLen--)
	{
		uchIndex = uchCRCHi ^ *puchMsg++;
		uchCRCHi = uchCRCLo ^ auchCRCHi[uchIndex];
		uchCRCLo = auchCRCLo[uchIndex];
	}
	return (uchCRCHi<<8 | uchCRCLo);
}


