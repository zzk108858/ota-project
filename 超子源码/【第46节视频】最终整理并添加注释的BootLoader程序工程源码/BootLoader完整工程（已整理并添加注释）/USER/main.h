#ifndef MAIN_H
#define MAIN_H

#include "stdint.h"

#define  GD32_FLASH_SADDR   0x08000000                                             //FLASH��ʼ��ַ
#define  GD32_PAGE_SIZE     1024                                                   //FLASH������С
#define  GD32_PAGE_NUM      64                                                     //FLASH����������
#define  GD32_B_PAGE_NUM    20                                                     //B����������
#define  GD32_A_PAGE_NUM    GD32_PAGE_NUM - GD32_B_PAGE_NUM                        //A����������
#define  GD32_A_START_PAGE  GD32_B_PAGE_NUM                                        //A����ʼ�������
#define  GD32_A_SADDR       GD32_FLASH_SADDR + GD32_A_START_PAGE * GD32_PAGE_SIZE  //A����ʼ��ַ

#define  UPDATA_A_FLAG      0x00000001        //״̬��־λ����λ������Ҫ����A��
#define  IAP_XMODEMC_FLAG   0x00000002        //״̬��־λ����λ����XmdoemЭ���һ�׶η��ʹ�дC       
#define  IAP_XMODEMD_FLAG   0x00000004        //״̬��־λ����λ����XmdoemЭ��ڶ��׶δ������ݰ�     
#define  SET_VERSION_FLAG   0x00000008        //״̬��־λ����λ������Ҫ����OTA�汾��
#define  CMD_5_FLAG         0x00000010        //״̬��־λ����λ������Ҫ��������5
#define  CMD5_XMODEM_FLAG   0x00000020        //״̬��־λ����λ������Ҫ����XmdoemЭ�鴦������5�Ĳ������ѳ�������д��W25Q64��Ӧ�Ŀ���
#define  CMD_6_FLAG         0x00000040        //״̬��־λ����λ������Ҫ��������6

#define OTA_SET_FLAG        0xAABB1122        //OTA_flag�Թ�״̬��Ӧ����ֵ�����OTA_flag���ڸ�ֵ��˵����ҪOTA����A��

typedef struct{          
	uint32_t OTA_flag;                        //��־�Եı���������OTA_SET_FLAG�����ֵʱ��������ҪOTA����A��
	uint32_t Firelen[11];                     //W25Q64�в�ͬ���г���̼��ĳ��ȣ�0�ų�Ա�̶���ӦW25Q64�б���0�Ŀ飬����OTA
	uint8_t  OTA_ver[32];
}OTA_InfoCB;                                  //OTA��ص���Ϣ�ṹ�壬��Ҫ���浽24C02

#define OTA_INFOCB_SIZE sizeof(OTA_InfoCB)    //OTA��ص���Ϣ�ṹ��ռ�õ��ֽڳ���
	
typedef struct{
	uint8_t  Updatabuff[GD32_PAGE_SIZE];      //����A��ʱ�����ڱ����W25Q64�ж�ȡ������
	uint32_t W25Q64_BlockNB;                  //���ڼ�¼���ĸ�W25Q64�Ŀ��ж�ȡ����
	uint32_t XmodemTimer;                     //���ڼ�¼XmdoemЭ���һ�׶η��ʹ�дC ��ʱ����                   
	uint32_t XmodemNB;                        //���ڼ�¼XmdoemЭ����յ��������ݰ���
	uint32_t XmodemCRC;	                      //���ڱ���XmdoemЭ��������CRC16У��ֵ
}UpDataA_CB;                                  

extern OTA_InfoCB OTA_Info;   //�ⲿ��������
extern UpDataA_CB UpDataA;    //�ⲿ��������
extern uint32_t BootStaFlag;  //�ⲿ��������

#endif



