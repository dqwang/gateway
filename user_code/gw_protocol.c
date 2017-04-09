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



