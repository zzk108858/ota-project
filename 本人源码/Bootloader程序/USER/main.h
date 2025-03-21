#ifndef __MAIN_H
#define __MAIN_H
#include "stm32f10x.h"


#define STM32_FLASH_STARTADDR		(0x08000000)                                                 //STM32 Flash��ʼ��ַ
#define STM32_PAGE_SIZE					(1024)                                                       //һҳ����������С
#define STM32_PAGE_NUM					(64)                                                         //��ҳ������������
#define STM32_B_PAGE_NUM				(20)                                                         //bootloader����С
#define STM32_A_PAGE_NUM				(STM32_PAGE_NUM - STM32_B_PAGE_NUM)                          //������С
#define STM32_A_START_PAGE			STM32_B_PAGE_NUM                                             //�������ʼҳ��ţ�������ţ�
#define STM32_A_START_ADDR			(STM32_FLASH_STARTADDR + STM32_B_PAGE_NUM * STM32_PAGE_SIZE)	 //�������ʼ��ַ


typedef struct{
	u32 OTA_flag;
	u32 FirmwareLen[11];
	u8	OTA_Version[32];
}OTA_InfoCB;

typedef struct{
	u8 Updata_A_Buff[STM32_PAGE_SIZE];	//����A��ʱ�����ڱ����W25Q64�ж�ȡ������,c8t6֧�ֵ����flashд��Ϊ1k�����Ի�������1k��С
	u32 Updata_A_from_W25Q64_Num;
	u32 XModem_Timer_Count;
	u16 XModemCRC;
	u32 XModem_Count;
}UpdateA_CB;

#define OTA_INFOCB_SIZE					(sizeof(OTA_InfoCB))
#define OTA_SET_FLAG	          (0xAABB1122)


#define  UPDATA_A_FLAG      		(0x00000001)        //״̬��־λ����λ������Ҫ����A��
#define  IAP_XModem_C_FLAG      (0x00000002)
#define  IAP_XModem_DATA_FLAG   (0x00000004)
#define  IAP_WRITE_VERSION_FLAG (0x00000008)
#define  CMD_5_FLAG 						(0x00000010)
#define  CMD_5_XMODEM_FLAG 			(0x00000020)
#define  CMD_6_FLAG 						(0x00000040)
#define  CMD_8_FLAG 						(0x00000080)

extern OTA_InfoCB OTA_Info;
extern UpdateA_CB UpdateA;
extern u32 BootStateFlag;

#endif
