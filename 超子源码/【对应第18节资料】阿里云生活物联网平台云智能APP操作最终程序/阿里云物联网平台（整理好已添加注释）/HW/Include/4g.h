#ifndef GM4G_H
#define GM4G_H

#include "stdint.h"

void GM4G_Init(void);                             //��������
void U2_Event(uint8_t *data, uint16_t datalen);   //��������
void OTA_Version(void);                           //��������
void OTA_Download(int size, int offset);          //��������

#endif
