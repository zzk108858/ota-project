#include "gd32f10x.h"
#include "iic.h"
#include "delay.h"

/*-------------------------------------------------*/
/*��������IIC��ʼ��                                */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void IIC_Init(void)
{
	rcu_periph_clock_enable(RCU_GPIOB);	                                //��GPIOBʱ��
	gpio_init(GPIOB,GPIO_MODE_OUT_OD,GPIO_OSPEED_50MHZ,GPIO_PIN_6);     //����PB6��OD�����ģ��SCL
	gpio_init(GPIOB,GPIO_MODE_OUT_OD,GPIO_OSPEED_50MHZ,GPIO_PIN_7);     //����PB7��OD�����ģ��SDA
	IIC_SCL_H;
	IIC_SDA_H;
}
/*-------------------------------------------------*/
/*��������IIC��ʼ�ź�                              */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void IIC_Start(void)
{
	IIC_SCL_H;        //����SCL
	IIC_SDA_H;        //����SDA
	Delay_Us(2);      //��ʱ
	IIC_SDA_L;        //����SDA
	Delay_Us(2);      //��ʱ
	IIC_SCL_L;        //����SCL
}
/*-------------------------------------------------*/
/*��������IICֹͣ�ź�                              */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void IIC_Stop(void)
{
	IIC_SCL_H;        //����SCL
	IIC_SDA_L;        //����SDA
	Delay_Us(2);      //��ʱ
	IIC_SDA_H;        //����SDA
}
/*-------------------------------------------------*/
/*��������IIC����һ���ֽ�����                      */
/*��  ����txd��Ҫ���͵�����                        */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void IIC_Send_Byte(uint8_t txd)
{
	int8_t i;                  //����forѭ��
	
	for(i=7;i>=0;i--){         //һ���ֽ�ѭ��8�� ��BIT7~BIT0
		IIC_SCL_L;             //����SCL
		if(txd&BIT(i))         //�жϵ�ǰ��Ҫ���͵Ķ����ƣ���1�Ļ�����if��֧
			IIC_SDA_H;         //����SDA
		else                   //�жϵ�ǰ��Ҫ���͵Ķ����ƣ���0�Ļ�����else��֧
			IIC_SDA_L;         //����SDA
		Delay_Us(2);           //��ʱ
		IIC_SCL_H;             //����SCL
		Delay_Us(2);           //��ʱ
	}
	IIC_SCL_L;                 //����SCL
	IIC_SDA_H;                 //����SDA���ȴ��ӻ�Ӧ��
}
/*-------------------------------------------------*/
/*��������IIC�ȴ�Ӧ��                              */
/*��  ����timeout����ʱʱ��                        */
/*����ֵ��0����ȷ  ����������                      */
/*-------------------------------------------------*/
uint8_t IIC_Wait_Ack(int16_t timeout)
{
	do{                                   //ѭ���ȴ�
		timeout--;                        //��ʱʱ���1
		Delay_Us(2);                      //��ʱ
	}while((READ_SDA)&&(timeout>=0));     //2�������˳�whileѭ����SDA��ʹӻ�Ӧ�� / ��ʱʱ��С��0
	if(timeout<0) return 1;               //�����ʱʱ��С��0��˵��û�еȵ��ӻ�Ӧ�𣬷���1����ʾ����
	IIC_SCL_H;                            //�ȵ��ӻ�Ӧ������SCL
	Delay_Us(2);                          //��ʱ
	if(READ_SDA!=0) return 2;             //�ٴ��жϣ���������ȶ��ĵ͵�ƽ������2����ʾ����
	IIC_SCL_L;                            //����SCL
	Delay_Us(2);                          //��ʱ
	return 0;                             //��ȷ������0
} 
/*-------------------------------------------------*/
/*��������IIC����һ���ֽ�����                      */
/*��  ����ack��1 Ӧ��ӻ�  0 ��Ӧ��ӻ�            */
/*����ֵ�����յ�����                               */
/*-------------------------------------------------*/
uint8_t IIC_Read_Byte(uint8_t ack)
{
	int8_t i;              //����forѭ��
	uint8_t rxd;           //���ڱ�����յ�����
	
	rxd = 0;               //����rxd
	for(i=7;i>=0;i--){     //һ���ֽ�ѭ��8�� ��BIT7~BIT0
		IIC_SCL_L;         //����SCL
		Delay_Us(2);       //��ʱ
		IIC_SCL_H;         //����SCL
		if(READ_SDA)       //�жϵ�ǰSDA��״̬������Ǹߵ�ƽ������if
			rxd |= BIT(i); //��Ӧ�Ķ���λ��¼�ߵ�ƽ1
		Delay_Us(2);       //��ʱ
	}
	IIC_SCL_L;             //����SCL
	Delay_Us(2);           //��ʱ
	if(ack){               //���ack��1������if
		IIC_SDA_L;         //����SDA
		IIC_SCL_H;         //����SCL
		Delay_Us(2);       //��ʱ
		IIC_SCL_L;         //����SCL
		IIC_SDA_H;         //����SDA
		Delay_Us(2);       //��ʱ
	}else{                 //���ack��0������else
		IIC_SDA_H;         //����SDA
		IIC_SCL_H;         //����SCL
		Delay_Us(2);       //��ʱ
		IIC_SCL_L;         //����SCL
		Delay_Us(2);       //��ʱ
	}
	return rxd;            //���ؽ��յ�����rxd
}


