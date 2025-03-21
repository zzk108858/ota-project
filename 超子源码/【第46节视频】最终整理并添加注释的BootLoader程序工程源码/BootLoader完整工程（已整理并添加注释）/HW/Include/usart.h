#ifndef USART_H
#define USART_H

#include "stdint.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"

#define U0_RX_SIZE 2048                 //接收缓冲区长度
#define U0_TX_SIZE 2048                 //发送缓冲区长度
#define U0_RX_MAX  256                  //单次接收最大量
#define NUM        10                   //se指针对结构体数组长度

typedef struct{                         
	uint8_t *start;                     //s用于标记起始位置
	uint8_t *end;                       //e用于标记结束位置
}UCB_URxBuffptr;                        //se指针对结构体

typedef struct{
	uint16_t URxCounter;                //累计接收数据量
	UCB_URxBuffptr URxDataPtr[NUM];     //se指针对结构体数组
	UCB_URxBuffptr *URxDataIN;          //指针 用于标记接收数据
	UCB_URxBuffptr *URxDataOUT;         //OUT指针 用于提取接收的数据
	UCB_URxBuffptr *URxDataEND;         //IN和OUT指针的结尾标志
}UCB_CB;                                //串口控制结构体

extern UCB_CB  U0CB;                    //变量外部声明
extern uint8_t U0_RxBuff[U0_RX_SIZE];   //变量外部声明

void Usart0_Init(uint32_t bandrate);    //函数声明
void DMA_Init(void);                    //函数声明
void U0Rx_PtrInit(void);                //函数声明
void u0_printf(char *format,...);       //函数声明

#endif
