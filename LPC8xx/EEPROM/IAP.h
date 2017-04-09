/****************************************Copyright (c)****************************************************
**                            Guangzhou ZHIYUAN electronics Co.,LTD.
**
**                                 http://www.embedtools.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:           IAP.h
** Last modified Date:  2012-08-22
** Last Version:        V1.00
** Descriptions:        The IAP Head File
**
**--------------------------------------------------------------------------------------------------------
** Created by:          CaoHua
** Created date:        2012-08-22
** Version:             V1.00
** Descriptions:        Creat The File
**
**--------------------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Descriptions:        
**
** Rechecked by:        
*********************************************************************************************************/
#ifndef __IAP_H__                                                       /*  __IAP_H__                   */
#define __IAP_H__                                                       /*  __IAP_H__                   */

#ifdef  __cplusplus
    extern "C" {
#endif

#include "stdint.h"
/* 
 *  定义CCLK值大小，单位为KHz 
 */
#define     IAP_FCCLK                                   (24000)
#define     IAP_ENTER_ADR                               0x1FFF1FF1      /* IAP入口地址定义              */
/*
 *  定义IAP返回状态字
 */
#define     CMD_SUCCESS                                 0               /* 成功执行命令                 */
#define     INVALID_COMMAND                             1               /* 无效命令                     */
#define     SRC_ADDR_ERROR                              2               /* 源地址不是以字为边界         */
#define     DST_ADDR_ERROR                              3               /* 目标地址错误                 */
#define     SRC_ADDR_NOT_MAPPED                         4               /* 源地址不在存储器映射中       */
#define     DST_ADDR_NOT_MAPPED                         5               /* 目标地址不在存储器映射中     */
#define     COUNT_ERROR                                 6               /* 字节计数器不是4的倍数或者非法*/
#define     INVALID_SECTOR                              7               /* 扇区号无效                   */
#define     SECTOR_NOT_BLANK                            8               /* 扇区非空                     */
#define     SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION     9               /* 为写操作准备扇区未执行       */
#define     COMPARE_ERROR                               10              /* 源和目标数据不相同           */
#define     BUSY                                        11              /* Flash编程硬件接口忙          */

/*********************************************************************************************************
** Function name:       sectorPrepare
** Descriptions:        IAP操作扇区选择，命令代码50
** input parameters:    IAP_Prepare:    选择扇区命令字
**                      sec1:           起始扇区
**                      sec2:           终止扇区
** output parameters:   paramout[0]:    IAP操作状态码,IAP返回值
** Returned value:      paramout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
extern uint32_t sectorPrepare (uint8_t  sec1, uint8_t  sec2);

/*********************************************************************************************************
** Function name:       ramCopy
** Descriptions:        复制RAM的数据到FLASH，命令代码51
** input parameters:    IAP_RAMTOFLASH: 复制RAM内容到Flash命令字
**                      dst:            目标地址，即FLASH起始地址。以512字节为分界
**                      src:            源地址，即RAM地址。地址必须字对齐
**                      no:             复制字节个数，为512/1024/4096/8192
**                      IAP_FCCLK：     系统时钟频率
** output parameters:   paramout[0]:    IAP操作状态码,IAP返回值
** Returned value:      paramout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
extern uint32_t ramCopy (uint32_t  dst, uint32_t  src, uint32_t  no);

/*********************************************************************************************************
** Function name:       sectorErase
** Descriptions:        扇区擦除，命令代码52
** input parameters:    IAP_ERASESECTOR：擦除扇区命令字
**                      sec1：          起始扇区
**                      sec2：          终止扇区
**                      IAP_FCCLK：     系统时钟频率
** output parameters:   paramout[0]:    IAP操作状态码,IAP返回值
** Returned value:      paramout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
extern uint32_t sectorErase (uint8_t  sec1, uint8_t  sec2);

/*********************************************************************************************************
** Function name:       blankChk
** Descriptions:        扇区查空，命令代码53
** input parameters:    IAP_BLANKCHK：  扇区查空命令字
                        sec1:           起始扇区
**                      sec2:           终止扇区92
** output parameters:   paramout[0]:    IAP操作状态码,IAP返回值
** Returned value:      paramout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
extern uint32_t blankChk (uint8_t  sec1, uint8_t  sec2);

/*********************************************************************************************************
** Function name:       parIdRead
** Descriptions:        读器件ID，命令代码54
** input parameters:    IAP_READPARTID：读器件ID命令字
** output parameters:   paramout[0]:    IAP操作状态码,IAP返回值
** Returned value:      paramout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
extern uint32_t partIdRead (void);

/*********************************************************************************************************
** Function name:       codeIdBoot
** Descriptions:        读代码ID，命令代码55
** input parameters:    IAP_BOOTCODEID：读代码ID命令字
** output parameters:   paramout[0]:    IAP操作状态码,IAP返回值
** Returned value:      paramout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
extern uint32_t codeIdBoot (void);

/*********************************************************************************************************
** Function name:       dataCompare
** Descriptions:        校验数据，命令代码56
** input parameters:    IAP_COMPARE：   数据比较命令字
                        dst:            目标地址，即RAM/FLASH起始地址。地址必须字对齐
**                      src:            源地址，即FLASH/RAM地址。地址必须字对齐
**                      no:             比较字节个数，必须能被4整除
** output parameters:   paramout[0]:    IAP操作状态码,IAP返回值
** Returned value:      paramout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
extern uint32_t dataCompare (uint32_t  dst, uint32_t  src, uint32_t  no);

/*********************************************************************************************************
** Function name:       ReinvokeISP
** Descriptions:        重新调用ISP，命令代码57
** input parameters:    IAP_REINVOKEISP：重新调用ISP命令字
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
extern void ReinvokeISP (void);

/*********************************************************************************************************
** Function name:       uIdRead
** Descriptions:        读UID，命令代码58
** input parameters:    IAP_READUID:    读UID命令字
** output parameters:   paramout[0]:    IAP操作状态码,IAP返回值
** Returned value:      paramout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
extern uint32_t uIdRead (void);

/*********************************************************************************************************
** Function name:       pageErase
** Descriptions:        页擦除，命令代码59
** input parameters:    IAP_READUID:    读UID命令字
** output parameters:   paramout[0]:    IAP操作状态码,IAP返回值
** Returned value:      paramout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
extern uint32_t pageErase (uint8_t  page1, uint8_t  page2);

#ifdef  __cplusplus
    }
#endif

#endif

/*********************************************************************************************************
  End Of File
*********************************************************************************************************/
