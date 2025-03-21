#ifndef USART_H
#define USART_H

#include "stdint.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"

#define U0_RX_SIZE 2048                 //���ջ���������
#define U0_TX_SIZE 2048                 //���ͻ���������
#define U0_RX_MAX  256                  //���ν��������
#define NUM        10                   //seָ��Խṹ�����鳤��

typedef struct{                         
	uint8_t *start;                     //s���ڱ����ʼλ��
	uint8_t *end;                       //e���ڱ�ǽ���λ��
}UCB_URxBuffptr;                        //seָ��Խṹ��

typedef struct{
	uint16_t URxCounter;                //�ۼƽ���������
	UCB_URxBuffptr URxDataPtr[NUM];     //seָ��Խṹ������
	UCB_URxBuffptr *URxDataIN;          //ָ�� ���ڱ�ǽ�������
	UCB_URxBuffptr *URxDataOUT;         //OUTָ�� ������ȡ���յ�����
	UCB_URxBuffptr *URxDataEND;         //IN��OUTָ��Ľ�β��־
}UCB_CB;                                //���ڿ��ƽṹ��

extern UCB_CB  U0CB;                    //�����ⲿ����
extern uint8_t U0_RxBuff[U0_RX_SIZE];   //�����ⲿ����

void Usart0_Init(uint32_t bandrate);    //��������
void DMA_Init(void);                    //��������
void U0Rx_PtrInit(void);                //��������
void u0_printf(char *format,...);       //��������

#endif
