#include "gd32f10x.h"
#include "usart.h"

uint8_t U0_TxBuff[U0_TX_SIZE];     //����0���ͻ�����
UCB_CB  U0CB;                      //���ڿ��ƽṹ��

uint8_t U2_RxBuff[U2_RX_SIZE];     //����2���ջ�����
uint8_t U2_TxBuff[U2_TX_SIZE];     //����2���ͻ�����
UCB_CB  U2CB;                      //���ڿ��ƽṹ��

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
	 
	usart_deinit(USART0);                                  //��λ����0
	usart_baudrate_set(USART0,bandrate);                   //���ò�����
	usart_parity_config(USART0,USART_PM_NONE);             //��ʹ��У��λ
	usart_word_length_set(USART0,USART_WL_8BIT);           //8λ����λ
	usart_stop_bit_set(USART0,USART_STB_1BIT);             //1��ֹͣλ
	usart_transmit_config(USART0,USART_TRANSMIT_ENABLE);   //�������͹��� 
	
	usart_enable(USART0);                                  //������0
}
/*-------------------------------------------------*/
/*����������ʼ������2                              */
/*��  ����bandrate��������                         */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void Usart2_Init(uint32_t bandrate)
{	
	rcu_periph_clock_enable(RCU_USART2);                                    //�򿪴���2ʱ��
	rcu_periph_clock_enable(RCU_GPIOB);                                     //��GPIOBʱ��
	
	gpio_init(GPIOB,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_10);         //��ʼ��PB10-����2 TX����
	gpio_init(GPIOB,GPIO_MODE_IN_FLOATING,GPIO_OSPEED_50MHZ,GPIO_PIN_11);   //��ʼ��PB11-����2 RX����
	 
	usart_deinit(USART2);                                  //��λ����2
	usart_baudrate_set(USART2,bandrate);                   //���ò�����
	usart_parity_config(USART2,USART_PM_NONE);             //��ʹ��У��λ
	usart_word_length_set(USART2,USART_WL_8BIT);           //8λ����λ
	usart_stop_bit_set(USART2,USART_STB_1BIT);             //1��ֹͣλ
	usart_transmit_config(USART2,USART_TRANSMIT_ENABLE);   //�������͹���
	usart_receive_config(USART2,USART_RECEIVE_ENABLE);     //�������չ���
	usart_dma_receive_config(USART2,USART_DENR_ENABLE);    //������DMA
	usart_dma_transmit_config(USART2,USART_DENT_ENABLE);   //������DMA
	
	nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);      //�����жϷ��飺2
	nvic_irq_enable(USART2_IRQn,0,0);                      //�򿪴���2�жϣ����������ȼ�
	usart_interrupt_enable(USART2,USART_INT_IDLE);         //�򿪴���2�����ж�  
	
	U2Rx_PtrInit();                                        //����2���ƽṹ�����ָ���ʼ��
	U2Tx_PtrInit();                                        //����2���ƽṹ�����ָ���ʼ��	
	DMA_Init();                                            //DMA��ʼ��	
	usart_enable(USART2);                                  //������2
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
	
	dma_deinit(DMA0,DMA_CH2);                                  //��λDMA0��ͨ��2-����2����ͨ����	
	dma_init_struct.periph_addr = USART2+4;                    //���������ַ������0�����ݼĴ���
	dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;  //�������ݿ�ȣ�8λ1���ֽ�
	dma_init_struct.memory_addr = (uint32_t)U2_RxBuff;         //���ý��ջ�������ַ
	dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;      //���������ݿ�ȣ�8λ1���ֽ�
	dma_init_struct.number = U2_RX_MAX+1;                      //������������
	dma_init_struct.priority = DMA_PRIORITY_HIGH;              //�������ȼ�
	dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;  //�����ַ���ӹر�
	dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;   //�ڴ��ַ���Ӵ�
	dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;      //���䷽������Ĵ������ڴ滺����
	dma_init(DMA0,DMA_CH2,&dma_init_struct);                   //��ʼ��DMA0��ͨ��2-����2����ͨ����
	dma_circulation_disable(DMA0,DMA_CH2);	                   //�ر�ѭ��ģʽ
	dma_channel_enable(DMA0,DMA_CH2);                          //��DMA0��ͨ��2-����2����ͨ����
	
	dma_deinit(DMA0,DMA_CH1);                                  //��λDMA0��ͨ��1-����2����ͨ����	
	dma_init_struct.periph_addr = USART2+4;                    //���������ַ������0�����ݼĴ���
	dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;  //�������ݿ�ȣ�8λ1���ֽ�
	dma_init_struct.memory_addr = (uint32_t)U2_TxBuff;         //���÷��ͻ�������ַ
	dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;      //���������ݿ�ȣ�8λ1���ֽ�
	dma_init_struct.number = 0;                                //������
	dma_init_struct.priority = DMA_PRIORITY_HIGH;              //�������ȼ�
	dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;  //�����ַ���ӹر�
	dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;   //�ڴ��ַ���Ӵ�
	dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;      //���䷽���ڴ滺����������Ĵ���
	dma_init(DMA0,DMA_CH1,&dma_init_struct);                   //��ʼ��DMA0��ͨ��1-����2����ͨ����
	dma_circulation_disable(DMA0,DMA_CH1);	                   //�ر�ѭ��ģʽ
	dma_channel_disable(DMA0,DMA_CH1);                         //�ر�DMA0��ͨ��1-����2����ͨ����
	
	dma_interrupt_enable(DMA0,DMA_CH1,DMA_INT_FTF);            //��������ж�
	nvic_irq_enable(DMA0_Channel1_IRQn,0,0);                   //���жϣ����������ȼ�
}
/*-------------------------------------------------*/
/*������������2���տ��ƽṹ�����ָ���ʼ��        */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void U2Rx_PtrInit(void)
{
	U2CB.URxDataIN = &U2CB.URxDataPtr[0];           //INָ��ָ��SEָ��Խṹ�������0�ų�Ա
	U2CB.URxDataOUT = &U2CB.URxDataPtr[0];	        //OUTָ��ָ��SEָ��Խṹ�������0�ų�Ա
	U2CB.URxDataEND = &U2CB.URxDataPtr[NUM-1];      //ENDָ��ָ��SEָ��Խṹ����������һ����Ա
	U2CB.URxDataIN->start = U2_RxBuff;              //ʹ��INָ��ָ���SEָ����е�Sָ���ǵ�һ�ν��յ���ʼλ��
	U2CB.URxCounter = 0;                            //�ۼƽ�����������
}
/*-------------------------------------------------*/
/*������������2���Ϳ��ƽṹ�����ָ���ʼ��        */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void U2Tx_PtrInit(void)
{
	U2CB.UTxDataIN = &U2CB.UTxDataPtr[0];           //INָ��ָ��SEָ��Խṹ�������0�ų�Ա 
	U2CB.UTxDataOUT = &U2CB.UTxDataPtr[0];	        //OUTָ��ָ��SEָ��Խṹ�������0�ų�Ա
	U2CB.UTxDataEND = &U2CB.UTxDataPtr[NUM-1];      //ENDָ��ָ��SEָ��Խṹ����������һ����Ա
	U2CB.UTxDataIN->start = U2_TxBuff;              //ʹ��INָ��ָ���SEָ����е�Sָ���ǵ�һ�ν��յ���ʼλ��
	U2CB.UTxCounter = 0;                            //�ۼƷ�����������                      
	U2CB.UTxState = 0;                              //����״̬����Ϊ����
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
/*-------------------------------------------------*/
/*������������2 �������� ����                      */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void u2_sdata(uint8_t *data, uint16_t data_len)
{	
	if(U2_TX_SIZE - U2CB.UTxCounter >= data_len){                //���㻺�����ڿ���λ���ڷ��ű��η��͵����������ܹ���Ž���if
		U2CB.UTxDataIN->start = &U2_TxBuff[U2CB.UTxCounter];     //�����ʼλ��
	}else{                                                       //���㻺�����ڿ���λ���ڷ��ű��η��͵���������������Ž���else
		U2CB.UTxCounter = 0;                                     //������Ż������ؾ��ۼƷ�����������
		U2CB.UTxDataIN->start = U2_TxBuff;                       //������Ż������ؾ���ʼλ�ûص����黺������ʼλ��
	}
	memcpy(U2CB.UTxDataIN->start,data,data_len);                 //����ʼλ�ÿ�ʼ��������Ҫ���͵�����
	U2CB.UTxCounter += data_len;                                 //�ۼƷ��������ۼƱ��η��͵�������
	U2CB.UTxDataIN->end = &U2_TxBuff[U2CB.UTxCounter-1];         //��ǽ���λ��
	
	U2CB.UTxDataIN++;                                            //INָ������
	if(U2CB.UTxDataIN == U2CB.UTxDataEND){                       //���INָ�����Ƶ�END��ǵ���β������if
		U2CB.UTxDataIN = &U2CB.UTxDataPtr[0];                    //INָ�뷵����ʼλ��
	}
}
