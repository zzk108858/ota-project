#include "usart.h"	  



u8 USART1_RX_BUF[USART1_RX_SIZE]; //���ջ���
u8 USART1_TX_BUF[USART1_TX_SIZE]; //���ͻ���
UCB_ControlBlock U1_CB;

u8 USART3_RX_BUF[USART3_RX_SIZE]; //���ջ���
u8 USART3_TX_BUF[USART3_TX_SIZE]; //���ͻ���
UCB_ControlBlock U3_CB;


void USART1_Init(u32 baudrate)
{
	GPIO_InitTypeDef GPIO_Config;
	USART_InitTypeDef USART1_Config;
	NVIC_InitTypeDef USART1_NVIC_Config;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//�����ж����ȼ�����
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);//��IO�ںʹ���ʱ��
	
	//USART1_TX   GPIOA.9
	GPIO_Config.GPIO_Mode		=	GPIO_Mode_AF_PP;
	GPIO_Config.GPIO_Pin		=	GPIO_Pin_9;
	GPIO_Config.GPIO_Speed	=	GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_Config);										//PA9��USART1_TX����������Ϊ�����������
	
	//USART1_RX	  GPIOA.10��ʼ��
	GPIO_Config.GPIO_Mode		=	GPIO_Mode_IN_FLOATING;
	GPIO_Config.GPIO_Pin		=	GPIO_Pin_10;
	GPIO_Config.GPIO_Speed	=	GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_Config);										//PA10��USART1_RX����������Ϊ��������
	
	//USART ��ʼ������
	USART_DeInit(USART1);
	USART1_Config.USART_BaudRate						= baudrate;
	USART1_Config.USART_HardwareFlowControl	= USART_HardwareFlowControl_None;	//��������
	USART1_Config.USART_Mode								= USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART1_Config.USART_Parity							= USART_Parity_No;								//��У��λ
	USART1_Config.USART_StopBits						= USART_StopBits_1;								//1λֹͣλ
	USART1_Config.USART_WordLength					= USART_WordLength_8b;						//���ݳ���8λ
	USART_Init(USART1, &USART1_Config);
	
	
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);	//ʹ�ܴ���1�����ж�
	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);	//ʹ�ܴ���1DMA����
	
	//USART1 NVIC ����
	USART1_NVIC_Config.NVIC_IRQChannel										= USART1_IRQn;
	USART1_NVIC_Config.NVIC_IRQChannelCmd									= ENABLE;
	USART1_NVIC_Config.NVIC_IRQChannelPreemptionPriority	= 0;
	USART1_NVIC_Config.NVIC_IRQChannelSubPriority					= 0;
	NVIC_Init(&USART1_NVIC_Config);
	
	
	U1Rx_Ptr_Init();							//����USART Control Block�и����ʼ���ĺ���
	DMA_USART1_RX_Init();					//���ô���1��DMA��ʼ���ĺ���
	USART_Cmd(USART1, ENABLE);		//ʹ�ܴ���1 
}


void DMA_USART1_RX_Init(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Channel5);
	
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = USART1_BASE + 0x04; 	 					//DMA�������ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)USART1_RX_BUF; 						//DMA�ڴ����ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  											//���ݴ��䷽�򣬴������ȡ���͵��ڴ�
	DMA_InitStructure.DMA_BufferSize = USART1_REC_LEN + 1;  										//DMAͨ����DMA����Ĵ�С�����ڲ��ÿ����ж�����ȡ���������գ������趨����buf���ڿɽ������ֵ���ò�������������ն�
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 					//�����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  									//�ڴ��ַ�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  	//���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; 					//���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  														//��������������ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; 											//DMAͨ�� xӵ�������ȼ� 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; 															//DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	
	DMA_Init(DMA1_Channel5, &DMA_InitStructure);
	
	DMA_Cmd(DMA1_Channel5, ENABLE);
}

void U1Rx_Ptr_Init(void)
{
	U1_CB.URxCount = 0;
	U1_CB.URxDataIN		= &U1_CB.URxDataPtr[0];
	U1_CB.URxDataOUT 	= &U1_CB.URxDataPtr[0];
	U1_CB.URxDataEND 	= &U1_CB.URxDataPtr[NUM - 1];
	U1_CB.URxDataIN->start = USART1_RX_BUF;
	
}

void u1_printf(char *format,...)
{
	u16 i;
	va_list list_data;						//�����"stdarg.h"ͷ�ļ�
	va_start(list_data, format);	//��...�еı䳤��������list_data
	vsprintf((char *)USART1_TX_BUF, format, list_data);//������format��ʽ�����ַ������浽�ַ�����
	va_end(list_data);						//��va_start�ɶԳ���
	
	for(i = 0; i < strlen((const char *)USART1_TX_BUF); i++)		//���ַ������ֽڷ���
	{
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) != 1);
		
		USART_SendData(USART1, USART1_TX_BUF[i]);
	}
	
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) != 1);	//�ȷ������
	
}










void USART3_Init(u32 baudrate)
{
	GPIO_InitTypeDef GPIO_Config;
	USART_InitTypeDef USART3_Config;
	NVIC_InitTypeDef USART3_NVIC_Config;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//�����ж����ȼ�����
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//��IO�ںʹ���ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	
	//USART3_TX   GPIOB.10
	GPIO_Config.GPIO_Mode		=	GPIO_Mode_AF_PP;
	GPIO_Config.GPIO_Pin		=	GPIO_Pin_10;
	GPIO_Config.GPIO_Speed	=	GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_Config);										//PA9��USART0_TX����������Ϊ�����������
	
	//USART3_RX	  GPIOB.11��ʼ��
	GPIO_Config.GPIO_Mode		=	GPIO_Mode_IN_FLOATING;
	GPIO_Config.GPIO_Pin		=	GPIO_Pin_11;
	GPIO_Config.GPIO_Speed	=	GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_Config);										//PA10��USART0_RX����������Ϊ��������
	
	//USART ��ʼ������
	USART_DeInit(USART3);
	USART3_Config.USART_BaudRate						= baudrate;
	USART3_Config.USART_HardwareFlowControl	= USART_HardwareFlowControl_None;	//��������
	USART3_Config.USART_Mode								= USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART3_Config.USART_Parity							= USART_Parity_No;								//��У��λ
	USART3_Config.USART_StopBits						= USART_StopBits_1;								//1λֹͣλ
	USART3_Config.USART_WordLength					= USART_WordLength_8b;						//���ݳ���8λ
	USART_Init(USART3, &USART3_Config);
	
	
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);	//ʹ�ܴ���3�����ж�
	USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);	//ʹ�ܴ���3DMA����
	
	//USART3 NVIC ����
	USART3_NVIC_Config.NVIC_IRQChannel										= USART3_IRQn;
	USART3_NVIC_Config.NVIC_IRQChannelCmd									= ENABLE;
	USART3_NVIC_Config.NVIC_IRQChannelPreemptionPriority	= 0;
	USART3_NVIC_Config.NVIC_IRQChannelSubPriority					= 0;
	NVIC_Init(&USART3_NVIC_Config);
	
	
	U3Rx_Ptr_Init();							//����USART Control Block�и����ʼ���ĺ���
	DMA_USART3_RX_Init();					//���ô���1��DMA��ʼ���ĺ���
	USART_Cmd(USART3, ENABLE);		//ʹ�ܴ���1 
}


void DMA_USART3_RX_Init(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Channel3);
	
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = USART3_BASE + 0x04; 	 					//DMA�������ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)USART3_RX_BUF; 						//DMA�ڴ����ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  											//���ݴ��䷽�򣬴������ȡ���͵��ڴ�
	DMA_InitStructure.DMA_BufferSize = USART3_REC_LEN + 1;  										//DMAͨ����DMA����Ĵ�С�����ڲ��ÿ����ж�����ȡ���������գ������趨����buf���ڿɽ������ֵ���ò�������������ն�
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 					//�����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  									//�ڴ��ַ�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  	//���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; 					//���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  														//��������������ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; 											//DMAͨ�� xӵ�������ȼ� 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; 															//DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	
	DMA_Init(DMA1_Channel3, &DMA_InitStructure);
	
	DMA_Cmd(DMA1_Channel3, ENABLE);
}

void U3Rx_Ptr_Init(void)
{
	U3_CB.URxCount = 0;
	U3_CB.URxDataIN		= &U3_CB.URxDataPtr[0];
	U3_CB.URxDataOUT 	= &U3_CB.URxDataPtr[0];
	U3_CB.URxDataEND 	= &U3_CB.URxDataPtr[NUM - 1];
	U3_CB.URxDataIN->start = USART3_RX_BUF;
	
}

void u3_printf(char *format,...)
{
	u16 i;
	va_list list_data;						//�����"stdarg.h"ͷ�ļ�
	va_start(list_data, format);	//��...�еı䳤��������list_data
	vsprintf((char *)USART3_TX_BUF, format, list_data);//������format��ʽ�����ַ������浽�ַ�����
	va_end(list_data);						//��va_start�ɶԳ���
	
	for(i = 0; i < strlen((const char *)USART3_TX_BUF); i++)		//���ַ������ֽڷ���
	{
		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) != 1);
		
		USART_SendData(USART3, USART3_TX_BUF[i]);
	}
	
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) != 1);	//�ȷ������
	
}

