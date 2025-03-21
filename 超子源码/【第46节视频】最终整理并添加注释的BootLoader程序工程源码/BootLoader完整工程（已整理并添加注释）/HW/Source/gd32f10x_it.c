/*!
    \file    gd32f10x_it.c
    \brief   interrupt service routines

    \version 2014-12-26, V1.0.0, firmware for GD32F10x
    \version 2017-06-20, V2.0.0, firmware for GD32F10x
    \version 2018-07-31, V2.1.0, firmware for GD32F10x
    \version 2020-09-30, V2.2.0, firmware for GD32F10x
*/

/*
    Copyright (c) 2020, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/
#include "gd32f10x_it.h"
#include "usart.h"

/*-------------------------------------------------*/
/*������������0�жϷ�����                        */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void USART0_IRQHandler(void)
{
	if(usart_interrupt_flag_get(USART0,USART_INT_FLAG_IDLE) != 0){                       //���USART_INT_FLAG_IDLE��λ����ʾ�����жϷ���������if
		usart_flag_get(USART0,USART_FLAG_IDLEF);                                         //��������жϱ�־λ����1����ȡ״̬��־�Ĵ���
		usart_data_receive(USART0);                                                      //��������жϱ�־λ����2����ȡ���ݼĴ���
		U0CB.URxCounter += (U0_RX_MAX+1) - dma_transfer_number_get(DMA0,DMA_CH4);        //+=�����������εĽ����������ۼƵ�URxCounter����
		U0CB.URxDataIN->end = &U0_RxBuff[U0CB.URxCounter-1];                             //INָ��ָ��Ľṹ���е�eָ���¼���ν��յĽ���λ��
		U0CB.URxDataIN++;                                                                //INָ�����
		if(U0CB.URxDataIN == U0CB.URxDataEND){                                           //��κ��Ƶ���END��ǵ�λ�ã�����if
			U0CB.URxDataIN = &U0CB.URxDataPtr[0];                                        //�ؾ�����ָ��0�ų�Ա
		}
		if(U0_RX_SIZE - U0CB.URxCounter >= U0_RX_MAX){                                   //�жϣ����ʣ��Ŀռ���ڵ��ڵ��ν��������������if
			U0CB.URxDataIN->start = &U0_RxBuff[U0CB.URxCounter];                         //INָ��ָ��Ľṹ���е�sָ���¼�´ν��յ���ʼλ��
		}else{                                                                           //�жϣ����ʣ��Ŀռ�С�ڵ��ν��������������else
			U0CB.URxDataIN->start = U0_RxBuff;                                           //ʣ��Ŀռ䲻���ˣ��ؾ��´εĽ���λ�÷��ػ�������ʼλ��
			U0CB.URxCounter = 0;                                                         //�ۼ�ֵҲ����
		}
		dma_channel_disable(DMA0,DMA_CH4);                                               //�ر�ͨ��4
		dma_transfer_number_config(DMA0,DMA_CH4,U0_RX_MAX+1);                            //�������ý�����
		dma_memory_address_config(DMA0,DMA_CH4,(uint32_t)U0CB.URxDataIN->start);         //�������ý���λ��
		dma_channel_enable(DMA0,DMA_CH4);                                                //����ͨ��4
	}
}


























/*!
    \brief      this function handles NMI exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void NMI_Handler(void)
{
}

/*!
    \brief      this function handles HardFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HardFault_Handler(void)
{
    /* if Hard Fault exception occurs, go to infinite loop */
    while(1){
    }
}

/*!
    \brief      this function handles MemManage exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void MemManage_Handler(void)
{
    /* if Memory Manage exception occurs, go to infinite loop */
    while(1){
    }
}

/*!
    \brief      this function handles BusFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void BusFault_Handler(void)
{
    /* if Bus Fault exception occurs, go to infinite loop */
    while(1){
    }
}

/*!
    \brief      this function handles UsageFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void UsageFault_Handler(void)
{
    /* if Usage Fault exception occurs, go to infinite loop */
    while(1){
    }
}

/*!
    \brief      this function handles SVC exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SVC_Handler(void)
{
}

/*!
    \brief      this function handles DebugMon exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DebugMon_Handler(void)
{
}

/*!
    \brief      this function handles PendSV exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void PendSV_Handler(void)
{
}

/*!
    \brief      this function handles SysTick exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SysTick_Handler(void)
{

}
