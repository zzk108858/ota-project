#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f10x.h"
#include "stdarg.h"
#include "string.h"

#define USART1_RX_SIZE				2048	//定义接收缓冲区总大小
#define USART1_TX_SIZE				2048	//定义发送缓冲区总大小
#define USART1_REC_LEN  			256  	//定义单次最大接收字节数 256
#define NUM									10		//定义接收缓冲段个数

#define USART3_RX_SIZE				2048	//定义接收缓冲区总大小
#define USART3_TX_SIZE				2048	//定义发送缓冲区总大小
#define USART3_REC_LEN  			256  	//定义单次最大接收字节数 256


typedef struct{
	u8 *start;
	u8 *end;
}UCB_URxBufptr;										//用来记录一段缓冲区的首尾地址
	  	
typedef struct{
	u16 URxCount;										//记录接收缓冲区内数据量
	UCB_URxBufptr URxDataPtr[NUM];	//缓冲段数组，数组内容为每一段数据的首尾地址
	UCB_URxBufptr *URxDataIN;				//写位置
	UCB_URxBufptr *URxDataOUT;			//读位置
	UCB_URxBufptr *URxDataEND;			//最后一段缓冲区的地址，用来判断是否需要回头
}UCB_ControlBlock;								//USART Control Block	用来记录接收到的每一部分数据的首尾位置和读写位置


void USART1_Init(u32 baudrate);		//串口1初始化，baudrate设置波特率
void DMA_USART1_RX_Init(void);		//串口1的DMA初始化
void U1Rx_Ptr_Init(void);					//USART Control Block中各项初始化
void u1_printf(char *format,...);	//串口1打印函数

void USART3_Init(u32 baudrate);		//串口3初始化，baudrate设置波特率
void DMA_USART3_RX_Init(void);		//串口3的DMA初始化
void U3Rx_Ptr_Init(void);					//USART Control Block中各项初始化
void u3_printf(char *format,...);	//串口3打印函数


extern UCB_ControlBlock U1_CB;				//声明为外部变量，在stm32f10x_it.c中的USART1_IRQHandler函数中用到
extern u8 USART1_RX_BUF[USART1_RX_SIZE];//声明为外部变量，在stm32f10x_it.c中的USART1_IRQHandler函数中用到


extern UCB_ControlBlock U3_CB;				//声明为外部变量，在stm32f10x_it.c中的USART1_IRQHandler函数中用到
extern u8 USART3_RX_BUF[USART3_RX_SIZE];//声明为外部变量，在stm32f10x_it.c中的USART1_IRQHandler函数中用到
#endif


