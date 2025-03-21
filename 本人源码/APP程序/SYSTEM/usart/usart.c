#include "usart.h"	  




u8 USART1_TX_BUF[USART1_TX_SIZE]; //发送缓冲
UCB_ControlBlock U1_CB;

u8 USART3_RX_BUF[USART3_RX_SIZE]; //接收缓冲
u8 USART3_TX_BUF[USART3_TX_SIZE]; //发送缓冲
UCB_ControlBlock U3_CB;


void USART1_Init(u32 baudrate)
{
	GPIO_InitTypeDef GPIO_Config;
	USART_InitTypeDef USART1_Config;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);//打开IO口和串口时钟
	
	//USART1_TX   GPIOA.9
	GPIO_Config.GPIO_Mode		=	GPIO_Mode_AF_PP;
	GPIO_Config.GPIO_Pin		=	GPIO_Pin_9;
	GPIO_Config.GPIO_Speed	=	GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_Config);										//PA9作USART1_TX，引脚配置为复用推挽输出
	
//	//USART1_RX	  GPIOA.10初始化
//	GPIO_Config.GPIO_Mode		=	GPIO_Mode_IN_FLOATING;
//	GPIO_Config.GPIO_Pin		=	GPIO_Pin_10;
//	GPIO_Config.GPIO_Speed	=	GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA, &GPIO_Config);										//PA10作USART1_RX，引脚配置为浮空输入
	
	//USART 初始化设置
	USART_DeInit(USART1);
	USART1_Config.USART_BaudRate						= baudrate;
	USART1_Config.USART_HardwareFlowControl	= USART_HardwareFlowControl_None;	//无流控制
	USART1_Config.USART_Mode								= USART_Mode_Tx;	//发送模式
	USART1_Config.USART_Parity							= USART_Parity_No;								//无校验位
	USART1_Config.USART_StopBits						= USART_StopBits_1;								//1位停止位
	USART1_Config.USART_WordLength					= USART_WordLength_8b;						//数据长度8位
	USART_Init(USART1, &USART1_Config);
	
	USART_Cmd(USART1, ENABLE);		//使能串口1 
}






void u1_printf(char *format,...)
{
	u16 i;
	va_list list_data;						//需包含"stdarg.h"头文件
	va_start(list_data, format);	//将...中的变长变量存入list_data
	vsprintf((char *)USART1_TX_BUF, format, list_data);//将根据format格式化的字符串保存到字符数组
	va_end(list_data);						//与va_start成对出现
	
	for(i = 0; i < strlen((const char *)USART1_TX_BUF); i++)		//将字符串按字节发送
	{
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) != 1);
		
		USART_SendData(USART1, USART1_TX_BUF[i]);
	}
	
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) != 1);	//等发送完成
	
}










void USART3_Init(u32 baudrate)
{
	GPIO_InitTypeDef GPIO_Config;
	USART_InitTypeDef USART3_Config;
	NVIC_InitTypeDef USART3_NVIC_Config;
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//打开IO口和串口时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	
	//USART3_TX   GPIOB.10
	GPIO_Config.GPIO_Mode		=	GPIO_Mode_AF_PP;
	GPIO_Config.GPIO_Pin		=	GPIO_Pin_10;
	GPIO_Config.GPIO_Speed	=	GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_Config);										//PA9作USART0_TX，引脚配置为复用推挽输出
	
	//USART3_RX	  GPIOB.11初始化
	GPIO_Config.GPIO_Mode		=	GPIO_Mode_IN_FLOATING;
	GPIO_Config.GPIO_Pin		=	GPIO_Pin_11;
	GPIO_Config.GPIO_Speed	=	GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_Config);										//PA10作USART0_RX，引脚配置为浮空输入
	
	//USART 初始化设置
	USART_DeInit(USART3);
	USART3_Config.USART_BaudRate						= baudrate;
	USART3_Config.USART_HardwareFlowControl	= USART_HardwareFlowControl_None;	//无流控制
	USART3_Config.USART_Mode								= USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART3_Config.USART_Parity							= USART_Parity_No;								//无校验位
	USART3_Config.USART_StopBits						= USART_StopBits_1;								//1位停止位
	USART3_Config.USART_WordLength					= USART_WordLength_8b;						//数据长度8位
	USART_Init(USART3, &USART3_Config);
	
	
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);	//使能串口3空闲中断
	USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);	//使能串口3DMA功能
	
	//USART3 NVIC 配置
	USART3_NVIC_Config.NVIC_IRQChannel										= USART3_IRQn;
	USART3_NVIC_Config.NVIC_IRQChannelCmd									= ENABLE;
	USART3_NVIC_Config.NVIC_IRQChannelPreemptionPriority	= 0;
	USART3_NVIC_Config.NVIC_IRQChannelSubPriority					= 0;
	NVIC_Init(&USART3_NVIC_Config);
	
	
	U3Rx_Ptr_Init();							//调用USART Control Block中各项初始化的函数
	U3Tx_Ptr_Init();
	DMA_USART3_RX_Init();					//调用串口3的DMA初始化的函数
	USART_Cmd(USART3, ENABLE);		//使能串口3 
	
}


void DMA_USART3_RX_Init(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef USART3_RX_NVIC_Config;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	/*	串口3接收通道	*/
	DMA_DeInit(DMA1_Channel3);
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = USART3_BASE + 0x04; 	 					//DMA外设基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)USART3_RX_BUF; 						//DMA内存基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  											//数据传输方向，从外设读取发送到内存
	DMA_InitStructure.DMA_BufferSize = USART3_REC_LEN + 1;  										//DMA通道的DMA缓存的大小，由于采用空闲中断来获取不定长接收，所以设定接收buf大于可接收最大值，用不产生接收完成终端
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 					//外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  									//内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  	//数据宽度为8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; 					//数据宽度为8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  														//工作在正常缓存模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; 											//DMA通道 x拥有中优先级 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; 															//DMA通道x没有设置为内存到内存传输
	
	DMA_Init(DMA1_Channel3, &DMA_InitStructure);
	
	DMA_Cmd(DMA1_Channel3, ENABLE);
	
	/*	串口3发送通道	*/
	DMA_DeInit(DMA1_Channel2);
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = USART3_BASE + 0x04; 	 					//DMA外设基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)USART3_TX_BUF; 					//DMA内存基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  											//数据传输方向，从内存发送到外设
	DMA_InitStructure.DMA_BufferSize = 0;  																		//DMA通道的DMA缓存的大小，
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 					//外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  									//内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  	//数据宽度为8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; 					//数据宽度为8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  														//工作在正常缓存模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; 											//DMA通道 x拥有中优先级 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; 															//DMA通道x没有设置为内存到内存传输
	
	DMA_Init(DMA1_Channel2, &DMA_InitStructure);
	
	DMA_Cmd(DMA1_Channel2, DISABLE);
	
	DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);
	USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);	//使能串口3DMA功能
	
	//USART3 NVIC 配置
	USART3_RX_NVIC_Config.NVIC_IRQChannel										= DMA1_Channel2_IRQn;
	USART3_RX_NVIC_Config.NVIC_IRQChannelCmd								= ENABLE;
	USART3_RX_NVIC_Config.NVIC_IRQChannelPreemptionPriority	= 0;
	USART3_RX_NVIC_Config.NVIC_IRQChannelSubPriority				= 0;
	NVIC_Init(&USART3_RX_NVIC_Config);
}

void U3Rx_Ptr_Init(void)
{
	U3_CB.URxCount = 0;
	U3_CB.URxDataIN		= &U3_CB.URxDataPtr[0];
	U3_CB.URxDataOUT 	= &U3_CB.URxDataPtr[0];
	U3_CB.URxDataEND 	= &U3_CB.URxDataPtr[NUM - 1];
	U3_CB.URxDataIN->start = USART3_RX_BUF;
	
}

void U3Tx_Ptr_Init(void)
{
	U3_CB.UTxCount = 0;
	U3_CB.UTxDataIN		= &U3_CB.UTxDataPtr[0];
	U3_CB.UTxDataOUT 	= &U3_CB.UTxDataPtr[0];
	U3_CB.UTxDataEND 	= &U3_CB.UTxDataPtr[NUM - 1];
	U3_CB.UTxDataIN->start = USART3_TX_BUF;
	U3_CB.UTxState = 0;
}

void u3_printf(char *format,...)
{
	u16 i;
	va_list list_data;						//需包含"stdarg.h"头文件
	va_start(list_data, format);	//将...中的变长变量存入list_data
	vsprintf((char *)USART3_TX_BUF, format, list_data);//将根据format格式化的字符串保存到字符数组
	va_end(list_data);						//与va_start成对出现
	
	for(i = 0; i < strlen((const char *)USART3_TX_BUF); i++)		//将字符串按字节发送
	{
		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) != 1);
		
		USART_SendData(USART3, USART3_TX_BUF[i]);
	}
	
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) != 1);	//等发送完成
	
}

/*-------------------------------------------------*/
/*函数名：串口3 发送数据 函数                      */
/*返回值：无                                       */
/*-------------------------------------------------*/
void u3_sdata(uint8_t *data, uint16_t data_len)
{
	if(USART3_TX_SIZE - U3_CB.UTxCount >= data_len){                //计算缓冲器内空余位置内否存放本次发送的数据量，能够存放进入if
		U3_CB.UTxDataIN->start = &USART3_TX_BUF[U3_CB.UTxCount];     //标记起始位置
	}else{                                                       //计算缓冲器内空余位置内否存放本次发送的数据量，不够存放进入else
		U3_CB.UTxCount = 0;                                     //不够存放缓冲器回卷，累计发送数量清零
		U3_CB.UTxDataIN->start = USART3_TX_BUF;                       //不够存放缓冲器回卷，起始位置回到数组缓冲器起始位置
	}
	memcpy(U3_CB.UTxDataIN->start,data,data_len);                 //从起始位置开始拷贝本次要发送的数据
	U3_CB.UTxCount += data_len;                                 //累计发送数量累计本次发送的数据量
	U3_CB.UTxDataIN->end = &USART3_TX_BUF[U3_CB.UTxCount-1];         //标记结束位置
	
	U3_CB.UTxDataIN++;                                            //IN指针下移
	if(U3_CB.UTxDataIN == U3_CB.UTxDataEND){                       //如果IN指针下移到END标记到结尾，进入if
		U3_CB.UTxDataIN = &U3_CB.UTxDataPtr[0];                    //IN指针返回起始位置
	}
}
