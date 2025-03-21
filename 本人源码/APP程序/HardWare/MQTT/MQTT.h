#ifndef __MQTT_H
#define __MQTT_H

#include "stm32f10x.h"
#include "stdint.h"
#include "string.h"

#define CLIENTID    "a1RwXdzy6BZ.D001|securemode=2,signmethod=hmacsha256,timestamp=1719196170811|"
#define USERNAME    "D001&a1RwXdzy6BZ"
#define PASSWORD    "b72f3898e0d38b424f0dd4959f7244fcf78b8737fa46b3b67d33f28e1f293e93"

typedef struct{
	uint8_t   Pack_buff[512];     //�������ݻ�����
	uint16_t  MessageID;          //���ı�ʶ������
	uint16_t  Fixed_len;          //���Ĺ̶���ͷ����
	uint16_t  Variable_len;       //���Ŀɱ䱨ͷ����
	uint16_t  Payload_len;        //���ĸ��س���
	uint16_t  Remaining_len;      //����ʣ�೤��
	uint8_t   CMD_buff[512];      //��ȡ�����ݻ�����
	int size;                     //OTA���ع̼���С
	int streamId;                 //OTA���ع̼�ID���
	int counter;                  //OTA�����ܹ����ش���
	int num;                      //OTA���ص�ǰ����
	int downlen;                  //OTA���ص�ǰ����������
	uint8_t  OTA_tempver[32];     //OTA������ʱ�汾�Ż�����
}MQTT_CB;

extern MQTT_CB  Aliyun_mqtt;      //�ⲿ��������                            

void MQTT_ConnectPack(void);                                  //��������
void MQTT_SubcribPack(char *topic);                           //��������
void MQTT_DealPublishData(uint8_t *data, uint16_t data_len);  //��������
void MQTT_PublishDataQs0(char *topic, char *data);            //��������
void MQTT_PublishDataQs1(char *topic, char *data);            //��������

#endif
