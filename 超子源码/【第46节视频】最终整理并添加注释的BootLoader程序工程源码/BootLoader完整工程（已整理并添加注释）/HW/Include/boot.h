#ifndef BOOT_H
#define BOOT_H

#include "stdint.h"

typedef void (*load_a)(void);                            //����ָ����������

void BootLoader_Brance(void);                            //��������
__asm void MSR_SP(uint32_t addr);                        //��������
void LOAD_A(uint32_t addr);                              //��������
void BootLoader_Clear(void);                             //��������
uint8_t BootLoader_Enter(uint8_t timeout);               //��������
void BootLoader_Info(void);                              //��������
void BootLoader_Event(uint8_t *data, uint16_t datalen);  //��������
uint16_t Xmodem_CRC16(uint8_t *data, uint16_t datalen);  //��������

#endif



