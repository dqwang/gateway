#ifndef __EEPROM_H__                                                    /*  __EEPROM_H__                */
#define __EEPROM_H__                                                    /*  __EEPROM_H__                */

#ifdef  __cplusplus
    extern "C" {
#endif

#include "stdint.h"

/*
 * 作为EEPROM的Flash配置（4 kB）：
 * Sector：     8   ~ 15
 * Page：       128 ~ 255
 * Addr：    0x2000 ~ 0x3FFF
 */
#define EEPROM_STARTSEC             28
#define EEPROM_ENDSEC               31
#define EEPROM_STARTPAGE            436
#define EEPROM_ENDPAGE              499
#define EEPROM_STARTADDR            0x7000
#define EEPROM_ENDADDR              0x7FFF
#define EEPROM_SIZE                 0x1000// 4 KB

/*
 *  定义EEPROM返回状态字
 */
#define EE_SUCCESS                  0                                   /* EEPROM写操作成功完成         */
#define IAP_ERROR                   1                                   /* 内部IAP函数调用出错          */
#define PARA_BANK_ERROR             2                                   /* 操作的板块错误               */
#define PARA_SRCADDR_ERROR          3                                   /* 写数据时的源地址错误(z字对齐)*/
#define PARA_OVERFLOW_ERROR         4                                   /* 写入溢出                     */


#define EEPROM_ADDR_GATEWAY_RF433M_ADDR_CHANNEL 0// 0-4:5B 

#define EEPROM_ADDR_GATEWAY_RF433M_READ_CONFIG_FLAG 5// 5:1B


/*********************************************************************************************************
** Function name:       eepromWriteNByte
** Descriptions:        调用该函数可以模拟写EEPROM操作，最大写数据256字节
** input parameters:    uieepromAddr:   EEPROM存储空间的偏移量地址
**                      pucData：       保存写入数据的指针
**                      uiLen：         写入的字节数
** output parameters:   状态信息
** Returned value:      无
注意：调用该函数将会使定义为EEPROM的Flash全部擦除，即会使前面写的所有数据丢失
*********************************************************************************************************/
extern uint32_t eepromWriteNByte(uint32_t uieepromAddr, uint8_t *pucData, uint32_t uiLen);

/*********************************************************************************************************
** Function name:       eepromRead
** Descriptions:        调用该函数可以读从EEPROM_STARTSEC(EEPROM起始扇区开始的数据)
** input parameters:    eeprom_addr:    当做EEPROM存储空间的偏移量地址
**                      pucData:        接收读取数据的指针
**                      uiLen:          读取的字节数
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
extern uint32_t eepromRead(uint32_t uieepromAddr, uint8_t *pucData, uint32_t uiLen);



void test_eeprom(void);


#ifdef  __cplusplus
    }
#endif

#endif

/*********************************************************************************************************
  End Of File
*********************************************************************************************************/
