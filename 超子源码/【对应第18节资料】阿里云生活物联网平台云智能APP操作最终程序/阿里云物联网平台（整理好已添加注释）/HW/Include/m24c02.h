#ifndef M24C02_H
#define M24C02_H

#include "stdint.h"

#define M24C02_WADDR  0xA0     //24C02д����������ַ
#define M24C02_RADDR  0xA1     //24C02������������ַ

uint8_t M24C02_WriteByte(uint8_t addr, uint8_t wdata);                     //��������
uint8_t M24C02_WritePage(uint8_t addr, uint8_t *wdata);                    //��������
uint8_t M24C02_ReadData(uint8_t addr, uint8_t *rdata, uint16_t datalen);   //��������
void M24C02_ReadOTAInfo(void);                                             //��������
void M24C02_WriteOTAInfo(void);                                            //��������

#endif



