#include "Board_Flash.h"


void STM32_EraseFlash(u16 start, u16 num)		//start��ʾҪ��������ʼҳ��ţ�num��ʾҪ����������
{
	u16 i;
	FLASH_Unlock();
	for(i = 0; i<num; i++){
		FLASH_ErasePage(0x08000000+ 1024*start+1024*i);//��ҳ������С������Ʒÿҳ1K�ֽ�
	}
	FLASH_Lock();
}


void STM32_WriteFlash(u32 startaddr, u32 *wdata, u32 wnum)	//startaddr��ʾд�����ʼ��ַ��wdata��ʾ��д���������飬num��ʾ��д����ֽڸ���
{
	FLASH_Unlock();
	while(wnum){
		FLASH_ProgramWord(startaddr, *wdata);
		wnum -= 4;
		startaddr += 4;
		wdata++;
	}
	
	FLASH_Lock();
}



