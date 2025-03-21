#include "gd32f10x.h"
#include "usart.h"

uint8_t U0_RxBuff[U0_RX_SIZE];     //����0���ջ�����
uint8_t U0_TxBuff[U0_TX_SIZE];     //����0���ͻ�����
UCB_CB  U0CB;                      //���ڿ��ƽṹ��

/*-------------------------------------------------*/
/*����������ʼ������0                              */
/*��  ����bandrate��������                         */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void Usart0_Init(uint32_t bandrate)
{	
	rcu_periph_clock_enable(RCU_USART0);                                    //�򿪴���0ʱ��
	rcu_periph_clock_enable(RCU_GPIOA);                                     //��GPIOAʱ��
	
	gpio_init(GPIOA,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_9);          //��ʼ��PA9-����0 TX����
	gpio_init(GPIOA,GPIO_MODE_IN_FLOATING,GPIO_OSPEED_50MHZ,GPIO_PIN_10);   //��ʼ��PA10-����0 RX����
	 
	usart_deinit(USART0);                                  //��λ����0
	usart_baudrate_set(USART0,bandrate);                   //���ò�����
	usart_parity_config(USART0,USART_PM_NONE);             //��ʹ��У��λ
	usart_word_length_set(USART0,USART_WL_8BIT);           //8λ����λ
	usart_stop_bit_set(USART0,USART_STB_1BIT);             //1��ֹͣλ
	usart_transmit_config(USART0,USART_TRANSMIT_ENABLE);   //�������͹���
	usart_receive_config(USART0,USART_RECEIVE_ENABLE);     //�������չ���
	usart_dma_receive_config(USART0,USART_DENR_ENABLE);    //������DMA
	
	nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);      //�����жϷ��飺2
	nvic_irq_enable(USART0_IRQn,0,0);                      //�򿪴���0�жϣ����������ȼ�
	usart_interrupt_enable(USART0,USART_INT_IDLE);         //�򿪴���0�����ж�  
	
	U0Rx_PtrInit();                                        //����0���ƽṹ�����ָ���ʼ��
	DMA_Init();                                            //DMA��ʼ��
	usart_enable(USART0);                                  //������0
}
/*-------------------------------------------------*/
/*��������DMA��ʼ��                                */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void DMA_Init(void)
{
	dma_parameter_struct dma_init_struct;                      //DMA��ʼ���ṹ��
	
	rcu_periph_clock_enable(RCU_DMA0);	                       //��DMA0ʱ��
	dma_deinit(DMA0,DMA_CH4);                                  //��λDMA0��ͨ��4-����0����ͨ����
	
	dma_init_struct.periph_addr = USART0+4;                    //���������ַ������0�����ݼĴ���
	dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;  //�������ݿ�ȣ�8λ1���ֽ�
	dma_init_struct.memory_addr = (uint32_t)U0_RxBuff;         //���ý��ջ�������ַ
	dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;      //���������ݿ�ȣ�8λ1���ֽ�
	dma_init_struct.number = U0_RX_MAX+1;                      //������������
	dma_init_struct.priority = DMA_PRIORITY_HIGH;              //�������ȼ�
	dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;  //�����ַ���ӹر�
	dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;   //�ڴ��ַ���Ӵ�
	dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;      //���䷽������Ĵ������ڴ滺����
	dma_init(DMA0,DMA_CH4,&dma_init_struct);                   //��ʼ��DMA0��ͨ��4-����0����ͨ����
	dma_circulation_disable(DMA0,DMA_CH4);	                   //�ر�ѭ��ģʽ
	dma_channel_enable(DMA0,DMA_CH4);                          //��DMA0��ͨ��4-����0����ͨ����
}
/*-------------------------------------------------*/
/*������������0���ƽṹ�����ָ���ʼ��            */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void U0Rx_PtrInit(void)
{
	U0CB.URxDataIN = &U0CB.URxDataPtr[0];           //INָ��ָ��SEָ��Խṹ�������0�ų�Ա
	U0CB.URxDataOUT = &U0CB.URxDataPtr[0];	        //OUTָ��ָ��SEָ��Խṹ�������0�ų�Ա
	U0CB.URxDataEND = &U0CB.URxDataPtr[NUM-1];      //ENDָ��ָ��SEָ��Խṹ����������һ����Ա
	U0CB.URxDataIN->start = U0_RxBuff;              //ʹ��INָ��ָ���SEָ����е�Sָ���ǵ�һ�ν��յ���ʼλ��
	U0CB.URxCounter = 0;                            //�ۼƽ�����������
}
/*-------------------------------------------------*/
/*������������0 printf ����                        */
/*��  ����char *format,...�������ʽ�Լ�����       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void u0_printf(char *format,...)
{	
	uint16_t i;                                           //����forѭ��
	
	va_list listdata;                                     //����һ��va_list����listdata
	va_start(listdata,format);                            //��listdata����...����Ĳ������Ĳ���
	vsprintf((char *)U0_TxBuff,format,listdata);          //��ʽ�������������U0_TxBuff
	va_end(listdata);                                     //�ͷ�listdata

	for(i=0;i<strlen((const char*)U0_TxBuff);i++){        //����U0_TxBuff��������������һ���ֽ�һ���ֽڵ�ѭ������
		while(usart_flag_get(USART0,USART_FLAG_TBE)!=1);  //�ȴ����ͼĴ�����
		usart_data_transmit(USART0,U0_TxBuff[i]);         //��������
	}
	while(usart_flag_get(USART0,USART_FLAG_TC)!=1);       //�ȵ����һ���ֽ����ݷ�����ϣ����˳�u0_printf�Ӻ���
}
