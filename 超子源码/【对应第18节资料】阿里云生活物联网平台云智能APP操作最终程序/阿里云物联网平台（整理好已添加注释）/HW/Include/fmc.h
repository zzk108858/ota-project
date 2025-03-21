#ifndef FMC_H
#define FMC_H

#include "stdint.h"

void GD32_EraseFlash(uint16_t start, uint16_t num);                     //函数声明
void GD32_WriteFlash(uint32_t saddr, uint32_t *wdata, uint32_t wnum);   //函数声明

#endif



