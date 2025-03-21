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
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != 0){	//�жϽ����Ƿ�Ϊ���ڿ����ж�
		USART_GetFlagStatus(USART1, USART_FLAG_IDLE);
		USART_ReceiveData(USART1);										//��������ʵ������Ϊ�˶�ȡ���ݣ�����Ϊ��������������жϵ���������������������λ(�ȶ�USART_SR��Ȼ���USART_DR)��
		
		/*
		 *		DMA_GetCurrDataCounter��������ֵ�ǡ�the number of remaining data units����Ҳ����ʣ���DMA����Buffer��С
		 *		���Buffer���Ǹ��ĳ�ʼ��С��DMA_BufferSize = USART1_REC_LEN + 1;
		 *		�����ѽ��յ����ݸ���Ϊ(USART1_REC_LEN + 1) - DMA_GetCurrDataCounter(DMA1_Channel5)
		 */
		
		U1_CB.URxCount += (USART1_REC_LEN + 1) - DMA_GetCurrDataCounter(DMA1_Channel5);	//��¼���ջ�������������
		
		U1_CB.URxDataIN->end = &USART1_RX_BUF[U1_CB.URxCount - 1];	//ָ��ǰ���յĲ��������ݵ�ĩβ
		U1_CB.URxDataIN++;	//ָ����һ�����ջ�����		��ֵ��U1_CB.URxDataIN = &U1_CB.URxDataPtr[0];�˴��������൱��URxDataIN = &U1_CB.URxDataPtr[1];
		
		if(U1_CB.URxDataIN == U1_CB.URxDataEND)	//�����һ�����ջ���������ĩβ
		{
			U1_CB.URxDataIN = &U1_CB.URxDataPtr[0];	//����Ѿ�������ĩβ�������»ص�������ͷ��
		}
		
		if((USART1_RX_SIZE - U1_CB.URxCount) >= USART1_REC_LEN)	//���ʣ��ռ�����һ�����Ľ�������
		{
			U1_CB.URxDataIN->start = &USART1_RX_BUF[U1_CB.URxCount];	//д��ָ�����
		}else{																		//���������һ������
			U1_CB.URxDataIN->start = USART1_RX_BUF;	//д��ָ��ص���ʼλ��
			U1_CB.URxCount = 0;											//��������Ŀ���
		}
		
		
		DMA_Cmd(DMA1_Channel5, DISABLE);
		DMA_InitTypeDef DMA_InitStructure;
		DMA_InitStructure.DMA_PeripheralBaseAddr = USART1_BASE + 0x04;  //DMA�������ַ
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)U1_CB.URxDataIN->start;  //DMA�ڴ����ַ
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //���ݴ��䷽�򣬴������ȡ���͵��ڴ�
		DMA_InitStructure.DMA_BufferSize = USART1_REC_LEN + 1;  //DMAͨ����DMA����Ĵ�С�����ڲ��ÿ����ж�����ȡ���������գ������趨����buf���ڿɽ������ֵ���ò�������������ն�
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //�����ַ�Ĵ�������
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�ڴ��ַ�Ĵ�������
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //���ݿ��Ϊ8λ
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //���ݿ��Ϊ8λ
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  //��������������ģʽ
		DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMAͨ�� xӵ�������ȼ� 
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
		
		DMA_Init(DMA1_Channel5, &DMA_InitStructure);
		
		DMA_Cmd(DMA1_Channel5, ENABLE);
		
	}
	
	
}
#endif
/********************/

/* add 2024.6.12 zd  */
void USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3, USART_IT_IDLE) != 0){	//�жϽ����Ƿ�Ϊ���ڿ����ж�
		USART_GetFlagStatus(USART3, USART_FLAG_IDLE);
		USART_ReceiveData(USART3);										//��������ʵ������Ϊ�˶�ȡ���ݣ�����Ϊ��������������жϵ���������������������λ(�ȶ�USART_SR��Ȼ���USART_DR)��
		
		/*
		 *		DMA_GetCurrDataCounter��������ֵ�ǡ�the number of remaining data units����Ҳ����ʣ���DMA����Buffer��С
		 *		���Buffer���Ǹ��ĳ�ʼ��С��DMA_BufferSize = USART3_REC_LEN + 1;
		 *		�����ѽ��յ����ݸ���Ϊ(USART3_REC_LEN + 1) - DMA_GetCurrDataCounter(DMA1_Channel3)
		 */
		
		U3_CB.URxCount += (USART3_REC_LEN + 1) - DMA_GetCurrDataCounter(DMA1_Channel3);	//��¼���ջ�������������
		
		U3_CB.URxDataIN->end = &USART3_RX_BUF[U3_CB.URxCount - 1];	//ָ��ǰ���յĲ��������ݵ�ĩβ
		U3_CB.URxDataIN++;	//ָ����һ�����ջ�����		��ֵ��U3_CB.URxDataIN = &U3_CB.URxDataPtr[0];�˴��������൱��URxDataIN = &U3_CB.URxDataPtr[1];
		
		if(U3_CB.URxDataIN == U3_CB.URxDataEND)	//�����һ�����ջ���������ĩβ
		{
			U3_CB.URxDataIN = &U3_CB.URxDataPtr[0];	//����Ѿ�������ĩβ�������»ص�������ͷ��
		}
		
		if((USART3_RX_SIZE - U3_CB.URxCount) >= USART3_REC_LEN)	//���ʣ��ռ�����һ�����Ľ�������
		{
			U3_CB.URxDataIN->start = &USART3_RX_BUF[U3_CB.URxCount];	//д��ָ�����
		}else{																		//���������һ������
			U3_CB.URxDataIN->start = USART3_RX_BUF;	//д��ָ��ص���ʼλ��
			U3_CB.URxCount = 0;											//��������Ŀ���
		}
		
		
		DMA_Cmd(DMA1_Channel3, DISABLE);
		DMA_InitTypeDef DMA_InitStructure;
		DMA_InitStructure.DMA_PeripheralBaseAddr = USART3_BASE + 0x04;  //DMA�������ַ
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)U3_CB.URxDataIN->start;  //DMA�ڴ����ַ
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //���ݴ��䷽�򣬴������ȡ���͵��ڴ�
		DMA_InitStructure.DMA_BufferSize = USART3_REC_LEN + 1;  //DMAͨ����DMA����Ĵ�С�����ڲ��ÿ����ж�����ȡ���������գ������趨����buf���ڿɽ������ֵ���ò�������������ն�
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //�����ַ�Ĵ�������
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�ڴ��ַ�Ĵ�������
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //���ݿ��Ϊ8λ
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //���ݿ��Ϊ8λ
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  //��������������ģʽ
		DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMAͨ�� xӵ�������ȼ� 
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
		
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
			u1_printf("������������\r\n");

		}else{
			u1_printf("�������ѶϿ�\r\n");
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
		u1_printf("����3���ݷ�����ɣ�\r\n");
		//u3_printf("\r\n");
		U3_CB.UTxState = 0;	 //����״̬����Ϊ����
		
	}
	
}
/********************/

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
