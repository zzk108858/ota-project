#ifndef __FS4G_H
#define __FS4G_H

#include "stm32f10x.h"
#include "main.h"
#include "usart.h"
#include "delay.h"
#include "boot.h"

void FS4G_Init(void);
void U3_Event_Process(u8 *data, u16 datalen);

#endif
