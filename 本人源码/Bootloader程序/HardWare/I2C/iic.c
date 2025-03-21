#include "iic.h"
#include "delay.h"
 
//��ʼ��IIC
void IIC_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	//ʹ��GPIOBʱ��
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD ;   //��©���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	IIC_SCL_H;//PB6,PB7 �����
	IIC_SDA_H;
}
//����IIC��ʼ�ź�
void IIC_Start(void)
{

	IIC_SCL_H;
	IIC_SDA_H;
	delay_us(4);
	IIC_SDA_L;
	delay_us(4);
	IIC_SCL_L;//ǯסI2C���ߣ�׼�����ͻ�������� 
	
}	  
//����IICֹͣ�ź�
void IIC_Stop(void)
{
	
	IIC_SCL_L;
	IIC_SDA_L;//STOP:when CLK is high DATA change form low to high
	delay_us(4);
	IIC_SCL_H;
	IIC_SDA_H;
	delay_us(4);//����I2C���߽����ź�
}

void IIC_Send_Byte(u8 txd)
{
	int8_t i;
	
	
	for(i = 7; i >= 0; i--){
		IIC_SCL_L;//����ʱ�ӿ�ʼ���ݴ���
		if(txd & (0x1<<(i)))
			IIC_SDA_H;
		else
			IIC_SDA_L;
		delay_us(2);
		IIC_SCL_H;
		delay_us(2);

	}
	IIC_SCL_L;
	IIC_SDA_H;	//����һ���ֽں󣬽�SDA���ߣ��ȴ��ӻ�����SDA��Ӧ���ź�

}

u8 IIC_Read_Byte(u8 ack)	//ack:�Ƿ���Ҫ����Ӧ���źţ�	��ʾ�����Ƿ�Ҫ���ݣ�����ӻ����������ݣ�������0��ʾӦ��(��Ҫ����)������1��ʾ��Ӧ��(��Ҫ����)
{
	int8_t i;
	u8 rxd = 0;	//���ڽ���
	
	IIC_SCL_L;
	delay_us(2);
	IIC_SDA_H;
	
	for(i = 7; i >= 0; i--){
		IIC_SCL_L;
		delay_us(2);
		IIC_SCL_H;
				
		if(READ_SDA)
			rxd |= (1<<(i));
		
		delay_us(2);
		
	}
	//��ȡһ���ֽں󣬽�SCL���ͣ��ȴ��ӻ��ͷ�SDA
//	IIC_SCL_L;
//	delay_us(2);
	
	if(ack){	//�����������Ҫ����		����0��ʾӦ��
		
		IIC_Ack();
		
	}else{		//�����������Ҫ����		����1��ʾ��Ӧ��
		
		IIC_NAck();
	}
	
	return rxd;
}



//����ACKӦ��
void IIC_Ack(void)
{
	IIC_SDA_L;
	IIC_SCL_H;
	delay_us(2);
	IIC_SCL_L;
	IIC_SDA_H;
	delay_us(2);
}
//������ACKӦ��		    
void IIC_NAck(void)
{
	IIC_SDA_H;
	IIC_SCL_H;
	delay_us(2);
	IIC_SCL_L;
	delay_us(2);
}		


//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC_Wait_Ack(int timeout)	//��λ us
{
	#if 0
	do{
		delay_us(1);
		timeout--;
	}while((READ_SDA) && (timeout >= 0));
	
	if(timeout < 0)	return 1;	//��ʱ�˳�
	
	IIC_SCL_H;
	delay_us(2);
	if(READ_SDA != 0)	return 2;	//�����ȶ��͵�ƽ�������˳�
	
	//�õ��ȶ��͵�ƽ��Ҳ���Ǵӻ���Ӧ���źţ���SCL���ͷ�ֹ�����
	IIC_SCL_L;
	delay_us(2);
	return 0;	//��������
	#endif
	
	
		
	do{
		timeout--;
		delay_us(2);
	}while((READ_SDA)&&(timeout>=0));
	if(timeout<0) return 1;
	IIC_SCL_H;
	delay_us(2);
	if(READ_SDA!=0) return 2;
	IIC_SCL_L;
	delay_us(2);
	return 0;
}



