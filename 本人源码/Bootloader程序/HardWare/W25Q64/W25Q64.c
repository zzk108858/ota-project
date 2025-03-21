#include "W25Q64.h"



void W25Q64_Init(void)
{
	GPIO_InitTypeDef GPIO_Config;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_Config.GPIO_Mode		=	GPIO_Mode_Out_PP;
	GPIO_Config.GPIO_Pin		=	GPIO_Pin_4;
	GPIO_Config.GPIO_Speed	=	GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_Config);										//PA4作普通IO控制片选
	
	CS_DISENABLE;
	SPI1_Init();
	
}

void W25Q64_WaitBusy(void)
{
	u8 status;
	do{
		CS_ENABLE;
		SPI1_ReadWriteByte(0x05);
		status = SPI1_ReadWriteByte(0xff);
		CS_DISENABLE;
	}while((status & 0x01) == 0x01);
}

void W25Q64_WriteEnable(void)
{
	W25Q64_WaitBusy();
	CS_ENABLE;
	SPI1_ReadWriteByte(0x06);
	CS_DISENABLE;
}

void W25Q64_Read(uint8_t *rbuff, uint32_t addr, uint32_t datalen)	//地址只用24位
{
	W25Q64_WaitBusy();
	CS_ENABLE;
	SPI1_ReadWriteByte(0x03);
	SPI1_ReadWriteByte((u8)(addr >> 16));
	SPI1_ReadWriteByte((u8)(addr >> 8));
	SPI1_ReadWriteByte((u8)(addr >> 0));
	SPI1_Read(rbuff,datalen);
	CS_DISENABLE;
}

void W25Q64_Erase64K(uint8_t blockNB){
	uint8_t wdata[4];
	
	wdata[0] = 0xD8;
	wdata[1] = (blockNB*64*1024)>>16;
	wdata[2] = (blockNB*64*1024)>>8;
	wdata[3] = (blockNB*64*1024)>>0;
	
	W25Q64_WaitBusy();
	W25Q64_WriteEnable();
	CS_ENABLE;
	SPI1_Write(wdata,4);
	CS_DISENABLE;
	W25Q64_WaitBusy();
}


/*	最大一次写入256字节	*/
void W25Q64_PageWrite(uint8_t *wbuff, uint16_t pageNB){
	uint8_t wdata[4];
	
	wdata[0] = 0x02;
	wdata[1] = (pageNB*256)>>16;
	wdata[2] = (pageNB*256)>>8;
	wdata[3] = (pageNB*256)>>0;
	
	W25Q64_WaitBusy();
	W25Q64_WriteEnable();
	CS_ENABLE;
	SPI1_Write(wdata,4);
	SPI1_Write(wbuff,256);
	CS_DISENABLE;
}
