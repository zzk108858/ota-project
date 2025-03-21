#include "gd32f10x.h"
#include "spi.h"

/*-------------------------------------------------*/
/*��������PSI0��ʼ��                               */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void SPI0_Init(void){
	
	spi_parameter_struct spi_parameter;                                             //��ʼ��SPI�Ľṹ��
	
	rcu_periph_clock_enable(RCU_SPI0);                                              //��SPI0ʱ��
	rcu_periph_clock_enable(RCU_GPIOA);                                             //��GPIOAʱ��
	
	gpio_init(GPIOA,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_5|GPIO_PIN_7);       //����PA5��PA7
	gpio_init(GPIOA,GPIO_MODE_IN_FLOATING,GPIO_OSPEED_50MHZ,GPIO_PIN_6);            //����PA6
	
	spi_i2s_deinit(SPI0);                                           //��λSPI0
	spi_parameter.device_mode = SPI_MASTER;                         //����ģʽ
	spi_parameter.trans_mode = SPI_TRANSMODE_FULLDUPLEX;            //˫��˫��ȫ˫��
	spi_parameter.frame_size = SPI_FRAMESIZE_8BIT;                  //���ݿ��8λһ���ֽ�
	spi_parameter.nss = SPI_NSS_SOFT;                               //�������NSS
	spi_parameter.endian = SPI_ENDIAN_MSB;                          //�����λ��ʼ����
	spi_parameter.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;    //ģʽ0
	spi_parameter.prescale = SPI_PSC_2;                             //2��Ƶ
	spi_init(SPI0,&spi_parameter);                                  //��ʼ��SPI0
	spi_enable(SPI0);	                                            //��SPI0
}  
/*-------------------------------------------------*/
/*��������SPI�շ�һ���ֽ�                          */
/*��  ����txd��Ҫ���͵�����                        */
/*����ֵ�����յ�������                             */
/*-------------------------------------------------*/
uint8_t SPI0_ReadWriteByte(uint8_t txd)
{
	while(spi_i2s_flag_get(SPI0,SPI_FLAG_TBE)!=1);  //�ȵ����ͻ�������
	spi_i2s_data_transmit(SPI0,txd);                //��������
	while(spi_i2s_flag_get(SPI0,SPI_FLAG_RBNE)!=1); //�ȴ����ջ�����������
	return spi_i2s_data_receive(SPI0);              //���ؽ��յ�������
}
/*-------------------------------------------------*/
/*��������SPI������                                */
/*��  ����wdata����������ָ��  datalen�����ͳ���   */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void SPI0_Write(uint8_t *wdata, uint16_t datalen)
{
	uint16_t i;                         //����forѭ��
	
	for(i=0;i<datalen;i++){             //ѭ��datalen�Σ�һ���ֽ�һ���ֽڷ���
		SPI0_ReadWriteByte(wdata[i]);   //����һ���ֽ�����
	}
}
/*-------------------------------------------------*/
/*��������SPI��������                              */
/*��  ����rdata�����ջ�����  datalen�����ճ���     */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void SPI0_Read(uint8_t *rdata, uint16_t datalen)
{
	uint16_t i;                                   //����forѭ��
	
	for(i=0;i<datalen;i++){                       //ѭ��datalen�Σ�һ���ֽ�һ���ֽڽ���
		rdata[i] = SPI0_ReadWriteByte(0xff);      //���ձ���һ���ֽ�����
	}
}


