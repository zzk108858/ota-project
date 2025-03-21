#ifndef __SPI_H
#define __SPI_H
#include "stm32f10x.h"
#include "stm32f10x_spi.h"

void SPI1_Init(void);
u8 SPI1_ReadWriteByte(u8 txd);
void SPI1_Write(u8 *wdata, u16 datalen);
void SPI1_Read(u8 *rdata, u16 datalen);
#endif
















