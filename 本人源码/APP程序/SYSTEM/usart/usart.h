#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f10x.h"
#include "stdarg.h"
#include "string.h"

#define USART1_RX_SIZE				2048	//������ջ������ܴ�С
#define USART1_TX_SIZE				2048	//���巢�ͻ������ܴ�С
#define USART1_REC_LEN  			256  	//���嵥���������ֽ��� 256
#define NUM									10		//������ջ���θ���

#define USART3_RX_SIZE				1024*5	//������ջ������ܴ�С
#define USART3_TX_SIZE				1024*5	//���巢�ͻ������ܴ�С
#define USART3_REC_LEN  			512  	//���嵥���������ֽ��� 256


typedef struct{
	u8 *start;
	u8 *end;
}UCB_URxBufptr;										//������¼һ�λ���������β��ַ
	  	
typedef struct{
	u16 UTxCount;										//��¼���ͻ�������������
	u16 URxCount;										//��¼���ջ�������������
	u16 UTxState;                  //0:���Ϳ���  1��æµ
	
	UCB_URxBufptr URxDataPtr[NUM];	//��������飬��������Ϊÿһ�����ݵ���β��ַ
	UCB_URxBufptr UTxDataPtr[NUM];
	
	UCB_URxBufptr *URxDataIN;				//дλ��
	UCB_URxBufptr *URxDataOUT;			//��λ��
	UCB_URxBufptr *URxDataEND;			//���һ�λ������ĵ�ַ�������ж��Ƿ���Ҫ��ͷ
	
	UCB_URxBufptr *UTxDataIN;				//дλ��
	UCB_URxBufptr *UTxDataOUT;			//��λ��
	UCB_URxBufptr *UTxDataEND;			//���һ�λ������ĵ�ַ�������ж��Ƿ���Ҫ��ͷ
	
	
}UCB_ControlBlock;								//USART Control Block	������¼���յ���ÿһ�������ݵ���βλ�úͶ�дλ��


void USART1_Init(u32 baudrate);		//����1��ʼ����baudrate���ò�����
void DMA_USART1_RX_Init(void);		//����1��DMA��ʼ��
void U1Rx_Ptr_Init(void);					//USART Control Block�и����ʼ��
void u1_printf(char *format,...);	//����1��ӡ����

void USART3_Init(u32 baudrate);		//����3��ʼ����baudrate���ò�����
void DMA_USART3_RX_Init(void);		//����3��DMA��ʼ��
void U3Rx_Ptr_Init(void);					//USART Control Block�и����ʼ��
void U3Tx_Ptr_Init(void);
void u3_printf(char *format,...);	//����3��ӡ����
void u3_sdata(uint8_t *data, uint16_t data_len);

extern UCB_ControlBlock U1_CB;				//����Ϊ�ⲿ��������stm32f10x_it.c�е�USART1_IRQHandler�������õ�
extern u8 USART1_RX_BUF[USART1_RX_SIZE];//����Ϊ�ⲿ��������stm32f10x_it.c�е�USART1_IRQHandler�������õ�


extern UCB_ControlBlock U3_CB;				//����Ϊ�ⲿ��������stm32f10x_it.c�е�USART1_IRQHandler�������õ�
extern u8 USART3_RX_BUF[USART3_RX_SIZE];//����Ϊ�ⲿ��������stm32f10x_it.c�е�USART1_IRQHandler�������õ�
#endif


