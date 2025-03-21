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


#define  FS4G_CONFIG_CIPMODE         	(0x00000001)        
#define	 CONNECT_OK										(0x00000002)		//��λ����CONNECT���ĳɹ�
#define  OTA_EVENT         						(0x00000004)        //��λ����OTA�¼����� 






extern OTA_InfoCB OTA_Info;
extern UpdateA_CB UpdateA;
extern u32 FS4G_Connect_State_Flag;

#endif
