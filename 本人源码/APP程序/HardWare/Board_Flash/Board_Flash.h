#ifndef __BOARD_FLASH_H
#define __BOARD_FLASH_H
#include "stm32f10x.h"
#include "stm32f10x_flash.h"



void STM32_EraseFlash(u16 start, u16 num);		//start��ʾҪ��������ʼҳ��ţ�num��ʾҪ����������
void STM32_WriteFlash(u32 startaddr, u32 *wdata, u32 wnum);	//startaddr��ʾд�����ʼ��ַ��wdata��ʾ��д���������飬num��ʾ��д����ֽڸ���

#endif
















