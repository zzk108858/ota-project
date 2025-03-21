#ifndef MQTT_H
#define MQTT_H

#include "stdint.h"

#define CLIENTID    "a1HvxPcHnkX.D001|securemode=2,signmethod=hmacsha256,timestamp=1682319552302|"
#define USERNAME    "D001&a1HvxPcHnkX"
#define PASSWORD    "7a245c0a5f23ca020e2c6ae6ac6e3d426ce0154e7267828d43132ea104850cfa"

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
