#ifndef __MQTT_H
#define __MQTT_H

#include "stm32f10x.h"
#include "stdint.h"
#include "string.h"

#define CLIENTID    "a1RwXdzy6BZ.D001|securemode=2,signmethod=hmacsha256,timestamp=1719196170811|"
#define USERNAME    "D001&a1RwXdzy6BZ"
#define PASSWORD    "b72f3898e0d38b424f0dd4959f7244fcf78b8737fa46b3b67d33f28e1f293e93"

typedef struct{
	uint8_t   Pack_buff[512];     //报文数据缓冲区
	uint16_t  MessageID;          //报文标识符变量
	uint16_t  Fixed_len;          //报文固定报头长度
	uint16_t  Variable_len;       //报文可变报头长度
	uint16_t  Payload_len;        //报文负载长度
	uint16_t  Remaining_len;      //报文剩余长度
	uint8_t   CMD_buff[512];      //提取的数据缓冲区
	int size;                     //OTA下载固件大小
	int streamId;                 //OTA下载固件ID编号
	int counter;                  //OTA下载总共下载次数
	int num;                      //OTA下载当前次数
	int downlen;                  //OTA下载当前次数下载量
	uint8_t  OTA_tempver[32];     //OTA下载临时版本号缓冲区
}MQTT_CB;

extern MQTT_CB  Aliyun_mqtt;      //外部变量声明                            

void MQTT_ConnectPack(void);                                  //函数声明
void MQTT_SubcribPack(char *topic);                           //函数声明
void MQTT_DealPublishData(uint8_t *data, uint16_t data_len);  //函数声明
void MQTT_PublishDataQs0(char *topic, char *data);            //函数声明
void MQTT_PublishDataQs1(char *topic, char *data);            //函数声明

#endif
