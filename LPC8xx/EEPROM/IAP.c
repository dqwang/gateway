/****************************************Copyright (c)****************************************************
**                            Guangzhou ZHIYUAN electronics Co.,LTD.
**
**                                 http://www.embedtools.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:           IAP.c
** Last modified Date:  2012-08-22
** Last Version:        V1.00
** Descriptions:        The IAP.c File
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
#include "IAP.h"


/* 
 *  定义IAP命令字
 */
#define     IAP_Prepare                                 50              /* 选择扇区                     */
                                                                        /* 【起始扇区号、结束扇区号】   */
#define     IAP_RAMTOFLASH                              51              /* 拷贝数据 FLASH目标地址       */
                                                                        /* 【目标Flash地址、RAM源地址、 */
                                                                        /* 写入字节数、系统时钟频率】   */
#define     IAP_ERASESECTOR                             52              /* 擦除扇区                     */
                                                                        /* 【起始扇区号、结束扇区号、   */
                                                                        /* 系统时钟频率】               */
#define     IAP_BLANKCHK                                53              /* 查空扇区                     */
                                                                        /* 【起始扇区号、结束扇区号】   */
#define     IAP_READPARTID                              54              /* 读器件ID    【无】           */
#define     IAP_BOOTCODEID                              55              /* 读Boot版本号【无】           */
#define     IAP_COMPARE                                 56              /* 比较命令                     */
                                                                        /* 【Flash起始地址、RAM起始地址 */
                                                                        /* 需要比较的字节数             */
#define     IAP_REINVOKEISP                             57              /* 重新调用ISP 【无】           */
#define     IAP_READUID                                 58              /* 读UID       【无】           */
#define     IAP_ERASEPAGE                               59              /* 擦除页                       */
                                                                        /* 【起始页号、结束页号】       */


uint32_t  __Guiparamin[5];                                              /* IAP入口参数缓冲区            */
uint32_t  __Guiparamout[5];                                             /* IAP出口参数缓冲区            */

typedef void (*IAP)(uint32_t [], uint32_t []);
IAP __GIAP_Entry = (IAP)IAP_ENTER_ADR;

/*********************************************************************************************************
** Function name:       sectorPrepare
** Descriptions:        IAP操作扇区选择，命令代码50
** input parameters:    IAP_Prepare:    选择扇区命令字
**                      sec1:           起始扇区
**                      sec2:           终止扇区
** output parameters:   __Guiparamout[0]:    IAP操作状态码,IAP返回值
** Returned value:      __Guiparamout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
uint32_t sectorPrepare (uint8_t  sec1, uint8_t  sec2)
{
    __Guiparamin[0] = IAP_Prepare;                                      /* 设置命令字                   */
    __Guiparamin[1] = sec1;                                             /* 设置参数                     */
    __Guiparamin[2] = sec2;                            
    
    (*__GIAP_Entry)(__Guiparamin, __Guiparamout);                       /* 调用IAP服务程序              */
    
    return (__Guiparamout[0]);                                          /* 返回状态码                   */
}

/*********************************************************************************************************
** Function name:       ramCopy
** Descriptions:        复制RAM的数据到FLASH，命令代码51
** input parameters:    IAP_RAMTOFLASH: 复制RAM内容到Flash命令字
**                      dst:            目标地址，即FLASH起始地址。以512字节为分界
**                      src:            源地址，即RAM地址。地址必须字对齐
**                      no:             复制字节个数，为64/128/256/512/1024
**                      IAP_FCCLK：     系统时钟频率
** output parameters:   __Guiparamout[0]:    IAP操作状态码,IAP返回值
** Returned value:      __Guiparamout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
uint32_t ramCopy (uint32_t  dst, uint32_t  src, uint32_t  no)
{
    __Guiparamin[0] = IAP_RAMTOFLASH;                                   /* 设置命令字                   */
    __Guiparamin[1] = dst;                                              /* 设置参数                     */
    __Guiparamin[2] = src;
    __Guiparamin[3] = no;
    
    (*__GIAP_Entry)(__Guiparamin, __Guiparamout);                       /* 调用IAP服务程序              */
    
    return (__Guiparamout[0]);                                          /* 返回状态码                   */
}

/*********************************************************************************************************
** Function name:       sectorErase
** Descriptions:        扇区擦除，命令代码52
** input parameters:    IAP_ERASESECTOR：擦除扇区命令字
**                      sec1：          起始扇区
**                      sec2：          终止扇区
**                      IAP_FCCLK：     系统时钟频率
** output parameters:   __Guiparamout[0]:    IAP操作状态码,IAP返回值
** Returned value:      __Guiparamout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
uint32_t sectorErase (uint8_t  sec1, uint8_t  sec2)
{
    __Guiparamin[0] = IAP_ERASESECTOR;                                  /* 设置命令字                   */
    __Guiparamin[1] = sec1;                                             /* 设置参数                     */
    __Guiparamin[2] = sec2;
    
    (*__GIAP_Entry)(__Guiparamin, __Guiparamout);                       /* 调用IAP服务程序              */
    
    return (__Guiparamout[0]);                                          /* 返回状态码                   */
}

/*********************************************************************************************************
** Function name:       blankChk
** Descriptions:        扇区查空，命令代码53
** input parameters:    IAP_BLANKCHK：  扇区查空命令字
                        sec1:           起始扇区
**                      sec2:           终止扇区92
** output parameters:   __Guiparamout[0]:    IAP操作状态码,IAP返回值
** Returned value:      __Guiparamout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
uint32_t blankChk (uint8_t  sec1, uint8_t  sec2)
{
    __Guiparamin[0] = IAP_BLANKCHK;                                     /* 设置命令字                   */
    __Guiparamin[1] = sec1;                                             /* 设置参数                     */
    __Guiparamin[2] = sec2;
    
    (*__GIAP_Entry)(__Guiparamin, __Guiparamout);                       /* 调用IAP服务程序              */
    
    return (__Guiparamout[0]);                                          /* 返回状态码                   */
}

/*********************************************************************************************************
** Function name:       parIdRead
** Descriptions:        读器件ID，命令代码54
** input parameters:    IAP_READPARTID：读器件ID命令字
** output parameters:   __Guiparamout[0]:    IAP操作状态码,IAP返回值
** Returned value:      __Guiparamout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
uint32_t partIdRead (void)
{
    __Guiparamin[0] = IAP_READPARTID;                                   /* 设置命令字                   */
    
    (*__GIAP_Entry)(__Guiparamin, __Guiparamout);                       /* 调用IAP服务程序              */
    
    return (__Guiparamout[0]);                                          /* 返回状态码                   */
}

/*********************************************************************************************************
** Function name:       codeIdBoot
** Descriptions:        读代码ID，命令代码55
** input parameters:    IAP_BOOTCODEID：读代码ID命令字
** output parameters:   __Guiparamout[0]:    IAP操作状态码,IAP返回值
** Returned value:      __Guiparamout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
uint32_t codeIdBoot (void)
{
    __Guiparamin[0] = IAP_BOOTCODEID;                                   /* 设置命令字                   */
    
    (*__GIAP_Entry)(__Guiparamin, __Guiparamout);                       /* 调用IAP服务程序              */
    
    return (__Guiparamout[0]);                                          /* 返回状态码                   */
}

/*********************************************************************************************************
** Function name:       dataCompare
** Descriptions:        校验数据，命令代码56
** input parameters:    IAP_COMPARE：   数据比较命令字
                        dst:            目标地址，即RAM/FLASH起始地址。地址必须字对齐
**                      src:            源地址，即FLASH/RAM地址。地址必须字对齐
**                      no:             比较字节个数，必须能被4整除
** output parameters:   __Guiparamout[0]:    IAP操作状态码,IAP返回值
** Returned value:      __Guiparamout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
uint32_t dataCompare (uint32_t  dst, uint32_t  src, uint32_t  no)
{
    __Guiparamin[0] = IAP_COMPARE;                                      /* 设置命令字                   */
    __Guiparamin[1] = dst;                                              /* 设置参数                     */
    __Guiparamin[2] = src;
    __Guiparamin[3] = no;
    
    (*__GIAP_Entry)(__Guiparamin, __Guiparamout);                       /* 调用IAP服务程序              */
    
    return (__Guiparamout[0]);                                          /* 返回状态码                   */
}

/*********************************************************************************************************
** Function name:       ReinvokeISP
** Descriptions:        重新调用ISP，命令代码57
** input parameters:    IAP_REINVOKEISP：重新调用ISP命令字
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void ReinvokeISP (void)
{
    __Guiparamin[0] = IAP_REINVOKEISP;                                  /* 设置命令字                   */
    
    (*__GIAP_Entry)(__Guiparamin, __Guiparamout);                       /* 调用IAP服务程序              */
}

/*********************************************************************************************************
** Function name:       uIdRead
** Descriptions:        读UID，命令代码58
** input parameters:    IAP_READUID:    读UID命令字
** output parameters:   __Guiparamout[0]:    IAP操作状态码,IAP返回值
** Returned value:      __Guiparamout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
uint32_t uIdRead (void)
{
    __Guiparamin[0] = IAP_READUID;                                      /* 设置命令字                   */
    
    (*__GIAP_Entry)(__Guiparamin, __Guiparamout);                       /* 调用IAP服务程序              */

    return __Guiparamout[0];
}

/*********************************************************************************************************
** Function name:       pageErase
** Descriptions:        页擦除，命令代码59
** input parameters:    IAP_READUID:    读UID命令字
** output parameters:   paramout[0]:    IAP操作状态码,IAP返回值
** Returned value:      paramout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
uint32_t pageErase (uint8_t  page1, uint8_t  page2)
{
    __Guiparamin[0] = IAP_ERASEPAGE;                                    /* 设置命令字                   */
    __Guiparamin[1] = page1;                                            /* 设置参数                     */
    __Guiparamin[2] = page2;
    
    (*__GIAP_Entry)(__Guiparamin, __Guiparamout);                       /* 调用IAP服务程序              */
    
    return (__Guiparamout[0]);                                          /* 返回状态码                   */
}

/*********************************************************************************************************
  End Of File
*********************************************************************************************************/
