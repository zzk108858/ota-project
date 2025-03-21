#include "gd32f10x.h"
#include "usart.h"

uint8_t U0_RxBuff[U0_RX_SIZE];     //串口0接收缓冲区
uint8_t U0_TxBuff[U0_TX_SIZE];     //串口0发送缓冲区
UCB_CB  U0CB;                      //串口控制结构体

/*-------------------------------------------------*/
/*函数名：初始化串口0                              */
/*参  数：bandrate：波特率                         */
/*返回值：无                                       */
/*-------------------------------------------------*/
void Usart0_Init(uint32_t bandrate)
{	
	rcu_periph_clock_enable(RCU_USART0);                                    //打开串口0时钟
	rcu_periph_clock_enable(RCU_GPIOA);                                     //打开GPIOA时钟
	
	gpio_init(GPIOA,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_9);          //初始化PA9-串口0 TX引脚
	gpio_init(GPIOA,GPIO_MODE_IN_FLOATING,GPIO_OSPEED_50MHZ,GPIO_PIN_10);   //初始化PA10-串口0 RX引脚
	 
	usart_deinit(USART0);                                  //复位串口0
	usart_baudrate_set(USART0,bandrate);                   //设置波特率
	usart_parity_config(USART0,USART_PM_NONE);             //不使用校验位
	usart_word_length_set(USART0,USART_WL_8BIT);           //8位数据位
	usart_stop_bit_set(USART0,USART_STB_1BIT);             //1个停止位
	usart_transmit_config(USART0,USART_TRANSMIT_ENABLE);   //开启发送功能
	usart_receive_config(USART0,USART_RECEIVE_ENABLE);     //开启接收功能
	usart_dma_receive_config(USART0,USART_DENR_ENABLE);    //开接收DMA
	
	nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);      //设置中断分组：2
	nvic_irq_enable(USART0_IRQn,0,0);                      //打开串口0中断，并设置优先级
	usart_interrupt_enable(USART0,USART_INT_IDLE);         //打开串口0空闲中断  
	
	U0Rx_PtrInit();                                        //串口0控制结构体各个指针初始化
	DMA_Init();                                            //DMA初始化
	usart_enable(USART0);                                  //开串口0
}
/*-------------------------------------------------*/
/*函数名：DMA初始化                                */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void DMA_Init(void)
{
	dma_parameter_struct dma_init_struct;                      //DMA初始化结构体
	
	rcu_periph_clock_enable(RCU_DMA0);	                       //打开DMA0时钟
	dma_deinit(DMA0,DMA_CH4);                                  //复位DMA0的通道4-串口0接收通道的
	
	dma_init_struct.periph_addr = USART0+4;                    //设置外设地址，串口0的数据寄存器
	dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;  //外设数据宽度，8位1个字节
	dma_init_struct.memory_addr = (uint32_t)U0_RxBuff;         //设置接收缓冲区地址
	dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;      //缓冲区数据宽度，8位1个字节
	dma_init_struct.number = U0_RX_MAX+1;                      //设置最大接收量
	dma_init_struct.priority = DMA_PRIORITY_HIGH;              //设置优先级
	dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;  //外设地址增加关闭
	dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;   //内存地址增加打开
	dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;      //传输方向：外设寄存器到内存缓冲区
	dma_init(DMA0,DMA_CH4,&dma_init_struct);                   //初始化DMA0的通道4-串口0接收通道的
	dma_circulation_disable(DMA0,DMA_CH4);	                   //关闭循环模式
	dma_channel_enable(DMA0,DMA_CH4);                          //打开DMA0的通道4-串口0接收通道的
}
/*-------------------------------------------------*/
/*函数名：串口0控制结构体各个指针初始化            */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void U0Rx_PtrInit(void)
{
	U0CB.URxDataIN = &U0CB.URxDataPtr[0];           //IN指针指向SE指针对结构体数组的0号成员
	U0CB.URxDataOUT = &U0CB.URxDataPtr[0];	        //OUT指针指向SE指针对结构体数组的0号成员
	U0CB.URxDataEND = &U0CB.URxDataPtr[NUM-1];      //END指针指向SE指针对结构体数组的最后一个成员
	U0CB.URxDataIN->start = U0_RxBuff;              //使用IN指针指向的SE指针对中的S指针标记第一次接收的起始位置
	U0CB.URxCounter = 0;                            //累计接收数量清零
}
/*-------------------------------------------------*/
/*函数名：串口0 printf 函数                        */
/*参  数：char *format,...：输出格式以及参数       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void u0_printf(char *format,...)
{	
	uint16_t i;                                           //用于for循环
	
	va_list listdata;                                     //建立一个va_list变量listdata
	va_start(listdata,format);                            //向listdata加载...代表的不定长的参数
	vsprintf((char *)U0_TxBuff,format,listdata);          //格式化输出到缓冲区U0_TxBuff
	va_end(listdata);                                     //释放listdata

	for(i=0;i<strlen((const char*)U0_TxBuff);i++){        //根据U0_TxBuff缓冲区数据量，一个字节一个字节的循环发送
		while(usart_flag_get(USART0,USART_FLAG_TBE)!=1);  //等待发送寄存器空
		usart_data_transmit(USART0,U0_TxBuff[i]);         //填入数据
	}
	while(usart_flag_get(USART0,USART_FLAG_TC)!=1);       //等到最后一个字节数据发送完毕，再退出u0_printf子函数
}
