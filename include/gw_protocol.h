#ifndef __GW_PROTOCOL_H__
#define __GW_PROTOCOL_H__

#include "type.h"

#define LOCK_MAX_NUM_PER_GATEWAY 20

#define FOR_LPC824 1

/*************************************
 define the server packet offset
*************************************/
#define LOCK_HEADER_5FE6 0x5FE6
#define TAIL_ENTER 0X0D0A

#define SERVER_PACKET_NUM_MAX 24
#define LOCK_PACKET_NUM_MAX 28

/*************************************
 define the packet size
*************************************/
#define RF_ADDR_SIZE 4
#define RF_CHANNEL_SIZE 1

#define SERVER_PACKET_SIZE 44
#define SERVER_PAYLOAD_SIZE 37
#define SERVER_TAIL_SIZE 2

#define LOCK_PACKET_SIZE 37
#define LOCK_PROTOCOL_SIZE 32// except the addr and channel

#define LOCK_RND_SIZE 4
#define LOCK_HEADER_SIZE 2
#define LOCK_LEN_SIZE 1
#define LOCK_CMD_SIZE 1
#define LOCK_DATA_PAYLOAD_SIZE_MAX 22// data size
#define LOCK_CRC_SIZE 2


enum cmd{
	//cmd server --> lock
	CMD_UPDATE_HEARTBEAT_TIME  = 0x13,
	CMD_UPDATE_ROOM_ADDR = 0x19,
	CMD_UPDATE_ROOM_STATUS = 0x22,
	CMD_UPDATE_ICCARD_WHITELIST = 0X25,
	CMD_UPDATE_ICCARD_KEY = 0x28,
	CMD_ROMOTE_OPEN_DOOR = 0X31,
	
	//ack from server to lock
	ACK_REPORT_OPENDOOR = 0x15,
	ACK_REPORT_LOW_VOLTAGE = 0x17,


	//cmd lock-->server

	CMD_REPORT_HEARTBEAT_TIME = 0X12,
	CMD_REPORT_OPENDOOR = 0X14,
	CMD_REPORT_LOW_VOLTAGE = 0X16,

	CMD_REQUEST_ROOM_ADDR =0X18,
	CMD_REQUEST_ROOM_STATUS= 0X21,
	CMD_REQUEST_ICCARD_WHITELIST = 0X24,
	CMD_REQUEST_ICCARD_KEY = 0X27,

	CMD_REPORT_TOUCH = 0X30,
	
	//ack lock -->server
	ACK_UPDATE_ROOM_ADDR = 0X20,
	ACK_UPDATE_ROOM_STATUS = 0X22,
	ACK_UPDATE_ICCARD_WHITELIST = 0X26,
	ACK_UPDATE_ICCARD_KEY=0X29,
	
};


/*----------------------------------------------------------------------------*/

#pragma pack (1)/*byte alignment*/


#define ROOM_ADDR_SIZE 8
#define TIME_SIZE 6

#define PAYLOAD_LEN_HEARTBEAT_TIME 20
typedef struct lock_protocol_payload1{
	u8 time1[TIME_SIZE];
	u8 heartlen2;
	u8 cardver3;
	u8 cardnum4;
	u8 keyver5;
	u8 livestate6;
	u8 room_addr7[ROOM_ADDR_SIZE];
	u8 wait_time8;
	u8 reserved9[2];	
}lpp_heartbeat_time_t;


typedef union{
	u8       lpp0[LOCK_DATA_PAYLOAD_SIZE_MAX];
	//lpp_cmd1 lpp1;
	//lpp_cmd2 lpp2;
	lpp_heartbeat_time_t lpp_CMD_REPORT_HEARTBEAT_TIME;
	//todo in lock
}lpp_u;

/*----------------------------------------------------------------------------*/

typedef struct lock_protocol1{
	u32 rnd;
	u16 header;
	u8 len;
	u8 cmd;
	lpp_u lpp;
	u16 crc;
}lpro_t1;


typedef struct lock_protocol2{
	u8 rnd[LOCK_RND_SIZE];
	u8 header[LOCK_HEADER_SIZE];
	u8 len[LOCK_LEN_SIZE];
	u8 cmd[LOCK_CMD_SIZE];
	lpp_u lpp;
	u8 crc[LOCK_CRC_SIZE];
}lpro_t2;


typedef union{
	u8      lpro0[LOCK_PROTOCOL_SIZE];// from rnd to crc
	lpro_t1 lpro1;
	lpro_t2 lpro2;
}lpro_u;

/*----------------------------------------------------------------------------*/

typedef struct lock_packet1{
	u32    lock_addr;
	u8     lock_channel;
	lpro_u lpro;
	
}lpkt_t1;

typedef struct lock_packet2{
	u8     lock_addr[RF_ADDR_SIZE];
	u8     lock_channel[RF_CHANNEL_SIZE];
	lpro_u lpro;
}lpkt_t2;


typedef union{//from lock addr to crc
	u8 		lpkt0[LOCK_PACKET_SIZE];
	lpkt_t1 lpkt1;
	lpkt_t2 lpkt2;
	
}lpkt_u;

/*----------------------------------------------------------------------------*/

typedef struct server_packet1{
	u32    gw_addr;
	u8     gw_channel;
	lpkt_u lpkt;
	u16    tail;//0x0d 0x0a
}sp_t1;

typedef struct server_packet2{
	u8     gw_addr[RF_ADDR_SIZE];
	u8     gw_channel[RF_CHANNEL_SIZE];
	lpkt_u lpkt;
	u8     tail[SERVER_TAIL_SIZE];
}sp_t2;

typedef union{//from gateway addr to enter
	u8    sp0[SERVER_PACKET_SIZE];
	sp_t1 sp1;
	sp_t2 sp2;	
}sp_u;

/*----------------------------------------------------------------------------*/

typedef struct rf_addr_channel1{
	u32 addr;
	u8 channel;
}rfac_t1;


typedef struct rf_addr_channel2{
	u8 addr[RF_ADDR_SIZE];
	u8 channel[RF_CHANNEL_SIZE];
}rfac_t2;

typedef union{
	u8      rfac0[RF_ADDR_SIZE + RF_CHANNEL_SIZE];
	rfac_t1 rfac1;
	rfac_t2 rfac2;
}rfac_u;



#pragma pack()/*stop alignment*/


/*----------------------------------------------------------------------------*/




//#define SH_GET_API_ID(p_api)    (p_api[SH_API_ID_OFFSET])
//#define SH_GET_API_VER(p_api)   (p_api[SH_API_VER_OFFSET])
//#define SH_GET_API_NELEM(p_api) (p_api[SH_API_NELEM_OFFSET])
//#define SH_GET_API_LEN(p_api)   (p_api[SH_API_LEN_LSB] |(p_api[SH_API_LEN_MSB] << 8))
//#define SH_GET_API_MTPASP(p_api)(p_api[SH_API_MTPSAP_OFFSET])


#define SH_GET_4_BYTE_LE(p_byte)  (((((u8*)(p_byte))[0]) << 0) | \
                                ((((u8*)(p_byte))[1]) << 8) | \
                                ((((u8*)(p_byte))[2]) << 16) | \
                                ((((u8*)(p_byte))[3]) << 24))
#define SH_GET_4_BYTE_BE(p_byte)  (((((u8*)(p_byte))[3]) << 0) | \
                                ((((u8*)(p_byte))[2]) << 8) | \
                                ((((u8*)(p_byte))[1]) << 16) | \
                                ((((u8*)(p_byte))[0]) << 24))

#define SH_GET_3_BYTE_LE(p_byte)  (((((u8*)(p_byte))[0]) << 0) | \
                       ((((u8*)(p_byte))[1]) << 8) | \
                         ((((u8*)(p_byte))[2]) << 16))

#define SH_GET_2_BYTE_LE(p_byte)  (((((u8*)(p_byte))[0]) << 0) | \
                                ((((u8*)(p_byte))[1]) << 8))
#define SH_GET_2_BYTE_BE(p_byte)  (((((u8*)(p_byte))[1]) << 0) | \
                                ((((u8*)(p_byte))[0]) << 8))
#define SH_GET_ONE_BYTE_LE(p_byte) (((u8*)(p_byte))[0])



#define SH_PUT_4_BYTE_LE(p_byte, val) \
	do \
	{ \
		(p_byte)[0] =(u8)(val); \
		(p_byte)[1] =(u8)((val) >> 8); \
		(p_byte)[2] =(u8)((val) >> 16); \
		(p_byte)[3] =(u8)((val) >> 24); \
	} while(0)
   
#define SH_PUT_4_BYTE_BE(p_byte, val) \
   do \
   { \
     (p_byte)[3] =(u8)(val); \
     (p_byte)[2] =(u8)((val) >> 8); \
     (p_byte)[1] =(u8)((val) >> 16); \
     (p_byte)[0] =(u8)((val) >> 24); \
   } while(0)

#define SH_PUT_3_BYTE_LE(p_byte, val) \
	do \
	{ \
		(p_byte)[0] =(u8)(val); \
		(p_byte)[1] =(u8)((val) >> 8); \
		(p_byte)[2] =(u8)((val) >> 16); \
	} while(0)

#define SH_PUT_2_BYTE_LE(p_byte, val) \
	do \
	{ \
		(p_byte)[0] =(u8)(val); \
		(p_byte)[1] =(u8)((val) >> 8); \
	} while(0)

#define SH_PUT_2_BYTE_BE(p_byte, val) \
	do \
	{ \
		(p_byte)[1] =(u8)(val); \
		(p_byte)[0] =(u8)((val) >> 8); \
	} while(0)


#if (FOR_LPC824)//big endian
#define SH_PUT_4_BYTE(p_byte,val) SH_PUT_4_BYTE_BE(p_byte,val)
#define SH_PUT_2_BYTE(p_byte, val) SH_PUT_2_BYTE_BE(p_byte, val)

#define SH_GET_4_BYTE(p_byte) SH_GET_4_BYTE_BE(p_byte)
#define SH_GET_2_BYTE(p_byte) SH_GET_2_BYTE_BE(p_byte)

#else// little endian
#define SH_PUT_4_BYTE(p_byte,val) SH_PUT_4_BYTE_LE(p_byte,val)
#define SH_PUT_2_BYTE(p_byte, val) SH_PUT_2_BYTE_LE(p_byte, val)

#define SH_GET_4_BYTE(p_byte) SH_GET_4_BYTE_LE(p_byte)
#define SH_GET_2_BYTE(p_byte) SH_GET_2_BYTE_LE(p_byte)

#endif

#define SH_PUT_ONE_BYTE_LE(p_byte,val)   (p_byte)[0] =(u8)((val))


/*----------------------------------------------------------------------------*/

typedef struct {
	u8 flag;
	u16 timer;
}todo;

/*----------------------------------------------------------------------------*/



//#define send2server(buf, len) uart2_sendbuf(buf, len)//wifi

#define send2server(buf, len) uart1_sendbuf(buf, len)//gprs

#define send2lock(buf, len)	uart0_sendbuf(buf, len);


void lock_addr_channel_array_init(void);

void handle_lock_packet_thread(void);
void handle_server_packet_thread(void);

u16 crc16(u8 *puchMsg,  u8 usDataLen);
void send_heartbeat_to_server(void);
void heartbeat_thread(void);
void test_heartbeat_without_timer_irq(void);

#endif
