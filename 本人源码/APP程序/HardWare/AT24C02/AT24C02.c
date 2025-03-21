#include "AT24C02.h"
#include "usart.h"
#include "delay.h"
#include <string.h>
#include "main.h"

/**
	*	@param	addr:		待写入的地址，取值区间0~255	at24c02空间大小2k字节	
	*	@param	wdata:	待写值，8位
	*	@retval	0：正常
						其他：异常
	*/

u8 AT24C02_WriteByte(u8 addr, u8 wdata)
{
	IIC_Start();
	IIC_Send_Byte(AT24C02_WADDR);					//写设备地址
	if(IIC_Wait_Ack(100) != 0)	return 1;//等待应答信号如果出错，则返回
	IIC_Send_Byte(addr);									//写存储地址
	if(IIC_Wait_Ack(100) != 0)	return 2;	//等待应答信号如果出错，则返回
	IIC_Send_Byte(wdata);									//写数据
	if(IIC_Wait_Ack(100) != 0)	return 3;	//等待应答信号如果出错，则返回
	IIC_Stop();	
	return 0;
}


/**
	*	@param	addr:		待写入的起始地址，取值区间0~255，应由待写入的页号*8得到。at24c02支持最大8字节一页写入	
	*	@param	wdata:	待写数组，应不超过8字节，超出部分无效
	*	@retval	0：正常
						其他：异常
	*/
u8 AT24C02_WritePage(u8 addr, u8 *wdata)	//按页写入 “The 1K/2K EEPROM is capable of an 8-byte page write”
{
	u8 i;
	
	IIC_Start();
	IIC_Send_Byte(AT24C02_WADDR);					//写设备地址
	if(IIC_Wait_Ack(100) != 0)	return 1;	//等待应答信号如果出错，则返回
	IIC_Send_Byte(addr);									//写存储地址
	if(IIC_Wait_Ack(100) != 0)	return 2;	//等待应答信号如果出错，则返回
	
	for(i = 0;i < 8; i++){
		IIC_Send_Byte(wdata[i]);									//写数据
		if(IIC_Wait_Ack(100) != 0)	return 3+i;	//等待应答信号如果出错，则返回
	}
	
	IIC_Stop();	
	return 0;
}

/**
	*	@param	addr:		待读取的地址，取值区间0~255
	*	@param	rdata:	读取值存储地址
	*	@retval	0：正常
						其他：异常
	*/
u8 AT24C02_ReadByte(u8 addr, u8 *rdata)	//
{
	IIC_Start();
	IIC_Send_Byte(AT24C02_WADDR);					//写设备地址
	if(IIC_Wait_Ack(100) != 0)	return 1;	//等待应答信号如果出错，则返回
	IIC_Send_Byte(addr);									//写存储地址
	if(IIC_Wait_Ack(100) != 0)	return 2;	//等待应答信号如果出错，则返回
	IIC_Start();
	IIC_Send_Byte(AT24C02_RADDR);	
	if(IIC_Wait_Ack(100) != 0)	return 3;
	

	*rdata = IIC_Read_Byte(0);
	
	IIC_Stop();	
	return 0;
}


/*
	*	2024.6.5 zd Add
	*	指定地址指定长度读取函数一直有问题，仅能读取第一个字节，封存，待日后找bug，暂时用ReadByte循环来实现本功能

u8 AT24C02_ReadData(u8 addr, u8 *rdata, u16 datalen)	//
{
	u8 i;
	IIC_Start();
	IIC_Send_Byte(AT24C02_WADDR);					//写设备地址
	if(IIC_Wait_Ack(100) != 0)	return 1;	//等待应答信号如果出错，则返回
	IIC_Send_Byte(addr);									//写存储地址
	if(IIC_Wait_Ack(100) != 0)	return 2;	//等待应答信号如果出错，则返回
	IIC_Start();
	IIC_Send_Byte(AT24C02_RADDR);	
	if(IIC_Wait_Ack(100) != 0)	return 3;
	
	for(i = 0;i < datalen - 1; i++){
		rdata[i] = IIC_Read_Byte(1);
	}
	rdata[datalen-1] = IIC_Read_Byte(0);
	
	IIC_Stop();	
	return 0;

}
*/

/**
	*	@note		2024.6.5 zd Add
	*					指定地址指定长度读取函数一直有问题，仅能读取第一个字节，封存，待日后找bug，暂时用ReadByte循环来实现本功能
	*	
	*	@param	addr:		待读取的起始地址，取值区间0~255
	*	@param	rdata:	读取存储数组，不超过256字节，at24c02最大256字节
	*	@param	datalen:待读取长度
	*/
u8 AT24C02_ReadData(u8 addr, u8 *rdata, u16 datalen)
{ 
	while(datalen)
	{
		AT24C02_ReadByte(addr++, rdata);
		rdata++;
		datalen--;
	}
	return 0;
}

/**
	*	@brief	检测AT24C02是否可以正常读写
	*	
	*	@retval	0：正常
						1：异常
	*/
u8 AT24C02_Check(void)
{
	u8 buf;
	AT24C02_ReadByte(255, &buf);//避免每次开机都写AT24CXX			   
	if((buf)==0X55){
		//u1_printf("1、rdata的数据：0x%x\r\n",buf);
		return 0;
	}else{				//排除第一次初始化的情况
		AT24C02_WriteByte(255,0X55);
		delay_ms(5);
		AT24C02_ReadByte(255, &buf);
		//u1_printf("2、rdata的数据：0x%x\r\n",buf);		
		if((buf)==0X55)
			return 0;
	}
	return 1;											  
}


void AT24C02_ReadOTAInfo(void)
{
	memset(&OTA_Info ,0 , OTA_INFOCB_SIZE);
	AT24C02_ReadData(0, (u8 *)&OTA_Info, OTA_INFOCB_SIZE);
}

void AT24C02_WriteOTAInfo(void)
{
	for(u8 i = 0;i < OTA_INFOCB_SIZE/8; i++){
		AT24C02_WritePage(i*8, ((u8 *)&OTA_Info) + i*8);
		delay_ms(5);
	}
}


