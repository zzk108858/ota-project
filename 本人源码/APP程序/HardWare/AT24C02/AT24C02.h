#ifndef __AT24C02_H
#define __AT24C02_H

#include "stm32f10x.h"
#include "iic.h"

#define AT24C02_WADDR	0xA0
#define AT24C02_RADDR	0xA1


u8 AT24C02_WriteByte(u8 addr, u8 wdata);              //注释见C文件
u8 AT24C02_WritePage(u8 addr, u8 *wdata);             //
u8 AT24C02_ReadData(u8 addr, u8 *rdata, u16 datalen); //
u8 AT24C02_ReadByte(u8 addr,u8 *rdata);               //
u8 AT24C02_Check(void);                               //



/*	OTA相关	*/
void AT24C02_ReadOTAInfo(void);
void AT24C02_WriteOTAInfo(void);

#endif
