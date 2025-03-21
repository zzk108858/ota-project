#ifndef __MAIN_H
#define __MAIN_H
#include "stm32f10x.h"


#define STM32_FLASH_STARTADDR		(0x08000000)                                                 //STM32 Flash起始地址
#define STM32_PAGE_SIZE					(1024)                                                       //一页（扇区）大小
#define STM32_PAGE_NUM					(64)                                                         //总页数（扇区数）
#define STM32_B_PAGE_NUM				(20)                                                         //bootloader区大小
#define STM32_A_PAGE_NUM				(STM32_PAGE_NUM - STM32_B_PAGE_NUM)                          //程序块大小
#define STM32_A_START_PAGE			STM32_B_PAGE_NUM                                             //程序块起始页编号（扇区编号）
#define STM32_A_START_ADDR			(STM32_FLASH_STARTADDR + STM32_B_PAGE_NUM * STM32_PAGE_SIZE)	 //程序块起始地址


typedef struct{
	u32 OTA_flag;
	u32 FirmwareLen[11];
	u8	OTA_Version[32];
}OTA_InfoCB;

typedef struct{
	u8 Updata_A_Buff[STM32_PAGE_SIZE];	//更新A区时，用于保存从W25Q64中读取的数据,c8t6支持的最大flash写入为1k，所以缓存区给1k大小
	u32 Updata_A_from_W25Q64_Num;
	u32 XModem_Timer_Count;
	u16 XModemCRC;
	u32 XModem_Count;
}UpdateA_CB;

#define OTA_INFOCB_SIZE					(sizeof(OTA_InfoCB))
#define OTA_SET_FLAG	          (0xAABB1122)


#define  FS4G_CONFIG_CIPMODE         	(0x00000001)        
#define	 CONNECT_OK										(0x00000002)		//置位表明CONNECT报文成功
#define  OTA_EVENT         						(0x00000004)        //置位表明OTA事件发生 






extern OTA_InfoCB OTA_Info;
extern UpdateA_CB UpdateA;
extern u32 FS4G_Connect_State_Flag;

#endif
