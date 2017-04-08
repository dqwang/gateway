#ifndef _PACKET_DEF_H
#define _PACKET_DEF_H

#include "type.h"

#define FOR_LPC824 1

/*************************************
 define the server packet offset
*************************************/

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



/*----------------------------------------------------------------------------*/

#pragma pack (1)/*byte alignment*/

typedef struct lock_protocol_payload1{
	u8 x1;
	u8 x2;
	u8 reserved[1];	
}lpp_cmd1;

typedef struct lock_protocol_payload2{
	u8 x1[1];
	u8 x2[1];
	u8 reserved[1];
}lpp_cmd2;


typedef union{
	u8       lpp0[LOCK_DATA_PAYLOAD_SIZE_MAX];
	lpp_cmd1 lpp1;
	lpp_cmd2 lpp2;
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


#endif

