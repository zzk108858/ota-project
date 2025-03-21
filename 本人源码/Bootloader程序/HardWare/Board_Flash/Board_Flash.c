#include "Board_Flash.h"


void STM32_EraseFlash(u16 start, u16 num)		//start表示要擦除的起始页编号，num表示要连续擦几块
{
	u16 i;
	FLASH_Unlock();
	for(i = 0; i<num; i++){
		FLASH_ErasePage(0x08000000+ 1024*start+1024*i);//按页擦除，小容量产品每页1K字节
	}
	FLASH_Lock();
}


void STM32_WriteFlash(u32 startaddr, u32 *wdata, u32 wnum)	//startaddr表示写入的起始地址，wdata表示待写入数据数组，num表示待写入的字节个数
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



