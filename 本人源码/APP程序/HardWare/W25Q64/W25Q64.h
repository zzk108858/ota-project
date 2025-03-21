#ifndef __W25Q64_H
#define __W25Q64_H

#include "stm32f10x.h"
#include "spi.h"

#define CS_ENABLE				GPIO_ResetBits(GPIOA, GPIO_Pin_4);
#define CS_DISENABLE		GPIO_SetBits(GPIOA, GPIO_Pin_4);


void W25Q64_Init(void);
void W25Q64_WaitBusy(void);
void W25Q64_WriteEnable(void);
void W25Q64_Read(uint8_t *rbuff, uint32_t addr, uint32_t datalen);
void W25Q64_Erase64K(uint8_t blockNB);
void W25Q64_PageWrite(uint8_t *wbuff, uint16_t pageNB);

#endif
