#include "SPI.h"



void SPI1_Init(void)
{
	GPIO_InitTypeDef GPIO_Config;
	SPI_InitTypeDef SPI_Config;
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_Config.GPIO_Mode		=	GPIO_Mode_AF_PP;
	GPIO_Config.GPIO_Pin		=	GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_Config.GPIO_Speed	=	GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_Config);										//PA5作SPI1_SCK，PA7作PI1_MOSI	引脚配置为复用推挽输出
	
	GPIO_Config.GPIO_Mode		=	GPIO_Mode_IN_FLOATING;
	GPIO_Config.GPIO_Pin		=	GPIO_Pin_6;
	GPIO_Config.GPIO_Speed	=	GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_Config);										//PA6作SPI1_MISO，引脚配置为浮空输入
	

	SPI_I2S_DeInit(SPI1);
	SPI_Config.SPI_Direction					=	SPI_Direction_2Lines_FullDuplex;
	SPI_Config.SPI_Mode								=	SPI_Mode_Master;
	SPI_Config.SPI_DataSize						=	SPI_DataSize_8b;
	SPI_Config.SPI_CPOL								=	SPI_CPOL_High;
	SPI_Config.SPI_CPHA								=	SPI_CPHA_2Edge;
	SPI_Config.SPI_NSS								=	SPI_NSS_Soft;
	SPI_Config.SPI_BaudRatePrescaler	=	SPI_BaudRatePrescaler_2;
	SPI_Config.SPI_FirstBit						=	SPI_FirstBit_MSB;
	SPI_Config.SPI_CRCPolynomial			=	7;
	
	SPI_Init(SPI1, &SPI_Config);
	SPI_Cmd(SPI1, ENABLE);
	
}


u8 SPI1_ReadWriteByte(u8 txd)
{
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)!= SET);
	SPI_I2S_SendData(SPI1, txd);
	
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)!= SET);
	return SPI_I2S_ReceiveData(SPI1);
}

void SPI1_Write(u8 *wdata, u16 datalen)
{
	u16 i;
	for(i = 0; i < datalen; i++){
		SPI1_ReadWriteByte(wdata[i]);
	}
}


void SPI1_Read(u8 *rdata, u16 datalen)
{
	u16 i;
	for(i = 0; i < datalen; i++){
		rdata[i] = SPI1_ReadWriteByte(0xff);
	}
	
}




