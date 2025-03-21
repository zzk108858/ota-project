#include "gd32f10x.h"
#include "iic.h"
#include <gd32f10x.h>
#include <gd32f10x.h>
#include "m24c02.h"
#include "main.h"
#include "string.h"
#include "delay.h"

/*-------------------------------------------------*/
/*��������24C02д��һ�ֽ�����                      */
/*��  ����addr����ַ  wdata����Ҫд�������        */
/*����ֵ��0����ȷ ����������                       */
/*-------------------------------------------------*/
uint8_t M24C02_WriteByte(uint8_t addr, uint8_t wdata)
{
	IIC_Start();                           //������ʼ�ź�
	IIC_Send_Byte(M24C02_WADDR);           //����24C02д����������ַ
	if(IIC_Wait_Ack(100)!=0) return 1;     //�ȴ�Ӧ�𣬴���Ļ�������1
	IIC_Send_Byte(addr);                   //�����ڲ��洢�ռ��ַ
	if(IIC_Wait_Ack(100)!=0) return 2;     //�ȴ�Ӧ�𣬴���Ļ�������2
	IIC_Send_Byte(wdata);                  //��������
	if(IIC_Wait_Ack(100)!=0) return 3;     //�ȴ�Ӧ�𣬴���Ļ�������3
	IIC_Stop();                            //����ֹͣ�ź�
	return 0;	                           //��ȷ������0
}
/*-------------------------------------------------*/
/*��������24C02д��һҳ��16�ֽڣ�����              */
/*��  ����addr����ַ  wdata����Ҫд�������ָ��    */
/*����ֵ��0����ȷ ����������                       */
/*-------------------------------------------------*/
uint8_t M24C02_WritePage(uint8_t addr, uint8_t *wdata){
	
	uint8_t i;                                  //����forѭ��
	
	IIC_Start();                                //������ʼ�ź�
	IIC_Send_Byte(M24C02_WADDR);                //����24C02д����������ַ
	if(IIC_Wait_Ack(100)!=0) return 1;          //�ȴ�Ӧ�𣬴���Ļ�������1
	IIC_Send_Byte(addr);                        //�����ڲ��洢�ռ��ַ
	if(IIC_Wait_Ack(100)!=0) return 2;          //�ȴ�Ӧ�𣬴���Ļ�������2
	for(i=0;i<16;i++){                          //ѭ��16��д��һҳ
		IIC_Send_Byte(wdata[i]);                //��������
		if(IIC_Wait_Ack(100)!=0) return 3+i;    //�ȴ�Ӧ�𣬴���Ļ�������3+i
	}
	IIC_Stop();                                 //����ֹͣ�ź�
	return 0;	                                //��ȷ������0
}
/*---------------------------------------------------------*/
/*��������24C02��ȡ����                                    */
/*��  ����addr����ַ  rdata�����ջ����� datalen:��ȡ����   */
/*����ֵ��0����ȷ ����������                               */
/*---------------------------------------------------------*/
uint8_t M24C02_ReadData(uint8_t addr, uint8_t *rdata, uint16_t datalen){
	
	uint8_t i;                                  //����forѭ��
	
	IIC_Start();                                //������ʼ�ź�
	IIC_Send_Byte(M24C02_WADDR);                //����24C02д����������ַ
	if(IIC_Wait_Ack(100)!=0) return 1;          //�ȴ�Ӧ�𣬴���Ļ�������1
	IIC_Send_Byte(addr);                        //�����ڲ��洢�ռ��ַ
	if(IIC_Wait_Ack(100)!=0) return 2;          //�ȴ�Ӧ�𣬴���Ļ�������2
	IIC_Start();                                //�ٴη�����ʼ�ź�
	IIC_Send_Byte(M24C02_RADDR);                //����24C02������������ַ
	if(IIC_Wait_Ack(100)!=0) return 3;          //�ȴ�Ӧ�𣬴���Ļ�������3
	for(i=0;i<datalen-1;i++){                   //ѭ��datalen-1�Σ�һ���ֽ�һ���ֽڽ�������
		rdata[i] = IIC_Read_Byte(1);            //�������ݣ�����Ӧ����ӻ�
	}
	rdata[datalen-1] = IIC_Read_Byte(0);        //�������һ���ֽڣ�������Ӧ���ź�
	IIC_Stop();                                 //����ֹͣ�ź�
	return 0;		                            //��ȷ������0
}
/*-------------------------------------------------*/
/*����������ȡ24C02�ṹ�嵽OTA_Info�ṹ�建����    */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void M24C02_ReadOTAInfo(void)
{
	memset(&OTA_Info,0,OTA_INFOCB_SIZE);                         //���OTA_Info�ṹ�建����
	M24C02_ReadData(0,(uint8_t *)&OTA_Info,OTA_INFOCB_SIZE);     //��24C02��ȡ���ݣ���ŵ�OTA_Info�ṹ��
}
/*-------------------------------------------------*/
/*����������OTA_Info�ṹ�建�������ݱ��浽24C02    */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void M24C02_WriteOTAInfo(void)
{
	uint8_t i;                               //����forѭ��
	uint8_t *wptr;                           //uint8_t����ָ��
	
	wptr = (uint8_t *)&OTA_Info;             //wptrָ��OTA_Info�ṹ���׵�ַ
	for(i=0;i<OTA_INFOCB_SIZE/16;i++){       //ÿ��д��һҳ16���ֽ�
		M24C02_WritePage(i*16,wptr+i*16);    //д��һҳ����
		Delay_Ms(5);                         //��ʱ
	}		
}

