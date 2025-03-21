#ifndef __IIC_H
#define __IIC_H
#include "stm32f10x.h"

//IO��������
 
#define GPIO_SCL			GPIOB
#define GPIO_SCL_PIN	GPIO_Pin_6

#define GPIO_SDA			GPIOB
#define GPIO_SDA_PIN	GPIO_Pin_7


//IO��������	 
#define IIC_SCL_H    GPIO_SetBits(GPIO_SCL, GPIO_SCL_PIN) //SCL
#define IIC_SCL_L    GPIO_ResetBits(GPIO_SCL, GPIO_SCL_PIN) //SCL

#define IIC_SDA_H    GPIO_SetBits(GPIO_SDA, GPIO_SDA_PIN) //SDA	
#define IIC_SDA_L    GPIO_ResetBits(GPIO_SDA, GPIO_SDA_PIN) //SDA

#define READ_SDA  	 GPIO_ReadInputDataBit(GPIO_SDA, GPIO_SDA_PIN)  //����SDA 

//IIC���в�������
void IIC_Init(void);         	//��ʼ��IIC��IO��				 
void IIC_Start(void);					//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);		//IIC����һ���ֽ�
u8 IIC_Read_Byte(u8 ack);			//IIC��ȡһ���ֽ�	ack:�Ƿ���Ҫ����Ӧ���ź�	��ʾ�����Ƿ�Ҫ���ݣ�����ӻ����������ݣ�������0��ʾӦ��(��Ҫ����)������1��ʾ��Ӧ��(��Ҫ����)
u8 IIC_Wait_Ack(int timeout);	//��λ us
void IIC_Ack(void);
void IIC_NAck(void);
//u8 IIC_Wait_Ack(void);
#endif
















