#ifndef GM4G_H
#define GM4G_H

#include "stdint.h"

void GM4G_Init(void);                             //函数声明
void U2_Event(uint8_t *data, uint16_t datalen);   //函数声明
void OTA_Version(void);                           //函数声明
void OTA_Download(int size, int offset);          //函数声明

#endif
