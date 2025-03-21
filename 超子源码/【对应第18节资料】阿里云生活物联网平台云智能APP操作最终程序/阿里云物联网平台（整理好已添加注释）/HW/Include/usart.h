#ifndef USART_H
#define USART_H

#include "stdint.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"

#define U0_TX_SIZE 256                  //���ͻ���������
#define U0_RX_MAX  256                  //���ν��������

#define U2_RX_SIZE 1024*5               //���ջ���������
#define U2_TX_SIZE 1024*5               //���ͻ���������
#define U2_RX_MAX  512                  //���ν��������

#define NUM        10                   //seָ��Խṹ�����鳤��

typedef struct{                         
	uint8_t *start;                     //s���ڱ����ʼλ��
	uint8_t *end;                       //e���ڱ�ǽ���λ��
}UCB_URxBuffptr;                        //seָ��Խṹ��

typedef struct{
	uint16_t URxCounter;                //�ۼƽ���������
	uint16_t UTxCounter;                //�ۼƽ���������
	uint16_t UTxState;                  //0:���Ϳ���  1��æµ
	UCB_URxBuffptr URxDataPtr[NUM];     //seָ��Խṹ������
	UCB_URxBuffptr UTxDataPtr[NUM];     //seָ��Խṹ������
	UCB_URxBuffptr *URxDataIN;          //ָ�� ���ڱ�ǽ�������
	UCB_URxBuffptr *URxDataOUT;         //OUTָ�� ������ȡ���յ�����
	UCB_URxBuffptr *URxDataEND;         //IN��OUTָ��Ľ�β��־
	UCB_URxBuffptr *UTxDataIN;          //ָ�� ���ڱ�Ƿ�������         
	UCB_URxBuffptr *UTxDataOUT;         //OUTָ�� ������ȡ���͵�����        
	UCB_URxBuffptr *UTxDataEND;         //IN��OUTָ��Ľ�β��־      
}UCB_CB;                                //���ڿ��ƽṹ��

extern UCB_CB  U0CB;                    //�����ⲿ����

extern UCB_CB  U2CB;                    //�����ⲿ����
extern uint8_t U2_RxBuff[U2_RX_SIZE];   //�����ⲿ����

void Usart0_Init(uint32_t bandrate);    //��������
void Usart2_Init(uint32_t bandrate);    //��������
void DMA_Init(void);                    //��������
void u0_printf(char *format,...);       //��������
void U2Rx_PtrInit(void);                //��������
void U2Tx_PtrInit(void);                //��������
void u2_sdata(uint8_t *data, uint16_t data_len);       //��������

#endif
