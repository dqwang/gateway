#include "main.h"

#define DisableIRQ()    __disable_irq()
#define EnableIRQ()     __enable_irq()

__align(4) uint8_t ucBuff[1024];


/*********************************************************************************************************
** Function name:       eepromRead
** Descriptions:        调用该函数可以读从EEPROM_STARTSEC(EEPROM起始扇区开始的数据)
** input parameters:    eeprom_addr:    当做EEPROM存储空间的偏移量地址
**                      pucData:        接收读取数据的指针
**                      uiLen:          读取的字节数
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
uint32_t eepromRead (uint32_t uieepromAddr, uint8_t *pucData, uint32_t uiLen)
{
    if ((uieepromAddr + uiLen) >= EEPROM_SIZE) {
        return PARA_SRCADDR_ERROR;
    }
    
    while (uiLen--) {
        *(pucData++) = *((uint8_t *)(EEPROM_STARTADDR + uieepromAddr++));
    }
    
    return EE_SUCCESS;
}

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
uint32_t eepromWriteNByte (uint32_t uieepromAddr, uint8_t *pucData, uint32_t uiLen)
{
    uint8_t  uceepromSec, ucFlashSec;
    uint32_t uiOff = 0, uiChangeMax, uiChangeLen;
    uint32_t uiErr = 0;
    uint32_t uiBuffAddr = (uint32_t)ucBuff;
    
    if ((uieepromAddr + uiLen) >= EEPROM_SIZE) {
        return PARA_SRCADDR_ERROR;
    }
    
    DisableIRQ();                                                       /* 关闭中断                     */
    uceepromSec = uieepromAddr / 1024;                                  /* EEPROM 的写入起始扇区号      */
    uiOff       = uieepromAddr - 1024 * uceepromSec;                    /* 开始修改的偏移量             */
    ucFlashSec  = uceepromSec + EEPROM_STARTSEC;
    uiChangeMax = 1024 * (uceepromSec + 1) - uieepromAddr;              /* 修改的最大长度               */
    uiChangeLen = (uiChangeMax > uiLen) ? uiLen : uiChangeMax;          /* 修改的长度                   */
    
    eepromRead((uint32_t)(uceepromSec << 10), ucBuff, 1024);            /* 读取EEPROM数据               */
    
    for (; uiOff < uiChangeLen; uiOff++) {
        ucBuff[uiOff] = *pucData++;                                     /* 修改EEPROM数据               */
    }
    /*
     * 擦除扇区
     */
    uiErr = sectorPrepare(ucFlashSec, ucFlashSec);
    uiErr = sectorErase(ucFlashSec, ucFlashSec);
    uiErr = blankChk(ucFlashSec, ucFlashSec);
    /*
     * 写入修改后的数据
     */
    uiErr = sectorPrepare(ucFlashSec, ucFlashSec);
    uiErr = ramCopy(ucFlashSec << 10, uiBuffAddr, 1024);
    uiErr = dataCompare(ucFlashSec << 10, uiBuffAddr, 1024);
    
    if (uiErr != CMD_SUCCESS) {
        return IAP_ERROR;
    }
    
    if (uiChangeLen == uiChangeMax) {
        uiLen = uiLen - uiChangeMax;
        while (uiLen >= 1024) {
            ucFlashSec++;
            for (uiOff = 0; uiOff < 1024; uiOff++) {
                ucBuff[uiOff] = *pucData++;                             /* 修改EEPROM数据               */
            }
            /*
             * 擦除扇区
             */
            uiErr = sectorPrepare(ucFlashSec, ucFlashSec);
            uiErr = sectorErase(ucFlashSec, ucFlashSec);
            uiErr = blankChk(ucFlashSec, ucFlashSec);
            /*
             * 写入修改后的数据
             */
            uiErr = sectorPrepare(ucFlashSec, ucFlashSec);
            uiErr = ramCopy(ucFlashSec << 10, uiBuffAddr, 1024);
            uiErr = dataCompare(ucFlashSec << 10, uiBuffAddr, 1024);
            
            if (uiErr != CMD_SUCCESS) {
                return IAP_ERROR;
            }
            uiLen -= 1024;
        }
        ucFlashSec++;
        uceepromSec = ucFlashSec - EEPROM_STARTSEC;
        
        eepromRead(uceepromSec << 10, ucBuff, 1024);                    /* 读取EEPROM数据               */
        for (uiOff = 0; uiOff < uiLen; uiOff++) {
            ucBuff[uiOff] = *pucData++;                                 /* 修改EEPROM数据               */
        }
        /*
         * 擦除扇区
         */
        uiErr = sectorPrepare(ucFlashSec, ucFlashSec);
        uiErr = sectorErase(ucFlashSec, ucFlashSec);
        uiErr = blankChk(ucFlashSec, ucFlashSec);
        /*
         * 写入修改后的数据
         */
        uiErr = sectorPrepare(ucFlashSec, ucFlashSec);
        uiErr = ramCopy(ucFlashSec << 10, uiBuffAddr, 1024);
        uiErr = dataCompare(ucFlashSec << 10, uiBuffAddr, 1024);
        
        if (uiErr != CMD_SUCCESS) {
            return IAP_ERROR;
        }
    }
    
    EnableIRQ();                                                        /* 打开中断                     */
    
    return EE_SUCCESS;
}


#define EEPROM_MAX_BYTES_ONCE 256//write and read once , about 110ms
void test_eeprom(void)
{
	u32 uiErr = 0;
    u8  ucSrcData[EEPROM_MAX_BYTES_ONCE];
	u16 i=0;
     
    hwapi02_led2_ctrl(ON);
	
    for (i = 0; i < EEPROM_MAX_BYTES_ONCE; i++) {
        ucSrcData[i] = i;
    }
    
    uiErr = eepromWriteNByte(0, ucSrcData, EEPROM_MAX_BYTES_ONCE);
	//memset(ucSrcData, 0x0, EEPROM_MAX_BYTES_ONCE);
	uiErr = eepromRead(0, ucSrcData, EEPROM_MAX_BYTES_ONCE);

    if (uiErr == EE_SUCCESS) {
        for (i = 0; i < EEPROM_MAX_BYTES_ONCE; i++) {
            if (ucSrcData[i] != i) {
                hwapi02_led3_ctrl(ON);
                break;
            }
        }
        if (i == EEPROM_MAX_BYTES_ONCE) {                                                  /* 数据校验成功，LED闪烁一次    */
			hwapi02_led2_ctrl(OFF);
		}
    } else {
        hwapi02_led3_ctrl(ON);
    }   

}


/*********************************************************************************************************
  End Of File
*********************************************************************************************************/
