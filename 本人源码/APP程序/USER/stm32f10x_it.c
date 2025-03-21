/**
  ******************************************************************************
  * @file    GPIO/IOToggle/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h" 
#include "usart.h"
#include "FS4G.h"
#include "MQTT.h" 
 
 
void NMI_Handler(void)
{
}
 
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}
 
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

 
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}
 
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}
 
void SVC_Handler(void)
{
}
 
void DebugMon_Handler(void)
{
}
 
void PendSV_Handler(void)
{
}
 
void SysTick_Handler(void)
{
}

/* add 2024.5.26 zd  */
#if 0
void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != 0){	//判断接收是否为串口空闲中断
		USART_GetFlagStatus(USART1, USART_FLAG_IDLE);
		USART_ReceiveData(USART1);										//这两句其实并不是为了读取数据，而是为了满足清除空闲中断的条件：“软件序列清除该位(先读USART_SR，然后读USART_DR)”
		
		/*
		 *		DMA_GetCurrDataCounter函数返回值是“the number of remaining data units”，也就是剩余的DMA接收Buffer大小
		 *		这个Buffer我们给的初始大小是DMA_BufferSize = USART1_REC_LEN + 1;
		 *		所以已接收的数据个数为(USART1_REC_LEN + 1) - DMA_GetCurrDataCounter(DMA1_Channel5)
		 */
		
		U1_CB.URxCount += (USART1_REC_LEN + 1) - DMA_GetCurrDataCounter(DMA1_Channel5);	//记录接收缓冲区内数据量
		
		U1_CB.URxDataIN->end = &USART1_RX_BUF[U1_CB.URxCount - 1];	//指向当前接收的不定长数据的末尾
		U1_CB.URxDataIN++;	//指向下一个接收缓冲区		初值是U1_CB.URxDataIN = &U1_CB.URxDataPtr[0];此处自增就相当于URxDataIN = &U1_CB.URxDataPtr[1];
		
		if(U1_CB.URxDataIN == U1_CB.URxDataEND)	//如果下一个接收缓冲区到了末尾
		{
			U1_CB.URxDataIN = &U1_CB.URxDataPtr[0];	//如果已经到缓冲末尾，则重新回到缓冲区头部
		}
		
		if((USART1_RX_SIZE - U1_CB.URxCount) >= USART1_REC_LEN)	//如果剩余空间满足一次最大的接收需求
		{
			U1_CB.URxDataIN->start = &USART1_RX_BUF[U1_CB.URxCount];	//写入指针后移
		}else{																		//如果不满足一次需求
			U1_CB.URxDataIN->start = USART1_RX_BUF;	//写入指针回到起始位置
			U1_CB.URxCount = 0;											//缓冲区数目清空
		}
		
		
		DMA_Cmd(DMA1_Channel5, DISABLE);
		DMA_InitTypeDef DMA_InitStructure;
		DMA_InitStructure.DMA_PeripheralBaseAddr = USART1_BASE + 0x04;  //DMA外设基地址
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)U1_CB.URxDataIN->start;  //DMA内存基地址
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //数据传输方向，从外设读取发送到内存
		DMA_InitStructure.DMA_BufferSize = USART1_REC_LEN + 1;  //DMA通道的DMA缓存的大小，由于采用空闲中断来获取不定长接收，所以设定接收buf大于可接收最大值，用不产生接收完成终端
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址寄存器不变
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址寄存器递增
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //数据宽度为8位
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //数据宽度为8位
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  //工作在正常缓存模式
		DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMA通道 x拥有中优先级 
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输
		
		DMA_Init(DMA1_Channel5, &DMA_InitStructure);
		
		DMA_Cmd(DMA1_Channel5, ENABLE);
		
	}
	
	
}
#endif
/********************/

/* add 2024.6.12 zd  */
void USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3, USART_IT_IDLE) != 0){	//判断接收是否为串口空闲中断
		USART_GetFlagStatus(USART3, USART_FLAG_IDLE);
		USART_ReceiveData(USART3);										//这两句其实并不是为了读取数据，而是为了满足清除空闲中断的条件：“软件序列清除该位(先读USART_SR，然后读USART_DR)”
		
		/*
		 *		DMA_GetCurrDataCounter函数返回值是“the number of remaining data units”，也就是剩余的DMA接收Buffer大小
		 *		这个Buffer我们给的初始大小是DMA_BufferSize = USART3_REC_LEN + 1;
		 *		所以已接收的数据个数为(USART3_REC_LEN + 1) - DMA_GetCurrDataCounter(DMA1_Channel3)
		 */
		
		U3_CB.URxCount += (USART3_REC_LEN + 1) - DMA_GetCurrDataCounter(DMA1_Channel3);	//记录接收缓冲区内数据量
		
		U3_CB.URxDataIN->end = &USART3_RX_BUF[U3_CB.URxCount - 1];	//指向当前接收的不定长数据的末尾
		U3_CB.URxDataIN++;	//指向下一个接收缓冲区		初值是U3_CB.URxDataIN = &U3_CB.URxDataPtr[0];此处自增就相当于URxDataIN = &U3_CB.URxDataPtr[1];
		
		if(U3_CB.URxDataIN == U3_CB.URxDataEND)	//如果下一个接收缓冲区到了末尾
		{
			U3_CB.URxDataIN = &U3_CB.URxDataPtr[0];	//如果已经到缓冲末尾，则重新回到缓冲区头部
		}
		
		if((USART3_RX_SIZE - U3_CB.URxCount) >= USART3_REC_LEN)	//如果剩余空间满足一次最大的接收需求
		{
			U3_CB.URxDataIN->start = &USART3_RX_BUF[U3_CB.URxCount];	//写入指针后移
		}else{																		//如果不满足一次需求
			U3_CB.URxDataIN->start = USART3_RX_BUF;	//写入指针回到起始位置
			U3_CB.URxCount = 0;											//缓冲区数目清空
		}
		
		
		DMA_Cmd(DMA1_Channel3, DISABLE);
		DMA_InitTypeDef DMA_InitStructure;
		DMA_InitStructure.DMA_PeripheralBaseAddr = USART3_BASE + 0x04;  //DMA外设基地址
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)U3_CB.URxDataIN->start;  //DMA内存基地址
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //数据传输方向，从外设读取发送到内存
		DMA_InitStructure.DMA_BufferSize = USART3_REC_LEN + 1;  //DMA通道的DMA缓存的大小，由于采用空闲中断来获取不定长接收，所以设定接收buf大于可接收最大值，用不产生接收完成终端
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址寄存器不变
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址寄存器递增
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //数据宽度为8位
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //数据宽度为8位
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  //工作在正常缓存模式
		DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMA通道 x拥有中优先级 
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输
		
		DMA_Init(DMA1_Channel3, &DMA_InitStructure);
		
		DMA_Cmd(DMA1_Channel3, ENABLE);
		
	}
	
	
}
/********************/

/* add 2024.6.12 zd  */
void EXTI0_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line0) == SET){
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 1){
			u1_printf("服务器已连接\r\n");

		}else{
			u1_printf("服务器已断开\r\n");
		}

		EXTI_ClearITPendingBit(EXTI_Line0);
	}
	
}
/********************/

/* add 2024.6.17 zd  */
void DMA1_Channel2_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC2) == SET){
		DMA_ClearITPendingBit(DMA1_IT_TC2);
		DMA_Cmd(DMA1_Channel2, DISABLE);
		u1_printf("串口3数据发送完成！\r\n");
		//u3_printf("\r\n");
		U3_CB.UTxState = 0;	 //发送状态设置为空闲
		
	}
	
}
/********************/

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
