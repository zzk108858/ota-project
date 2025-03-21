#ifndef __IIC_H
#define __IIC_H
#include "stm32f10x.h"

//IO方向设置
 
#define GPIO_SCL			GPIOB
#define GPIO_SCL_PIN	GPIO_Pin_6

#define GPIO_SDA			GPIOB
#define GPIO_SDA_PIN	GPIO_Pin_7


//IO操作函数	 
#define IIC_SCL_H    GPIO_SetBits(GPIO_SCL, GPIO_SCL_PIN) //SCL
#define IIC_SCL_L    GPIO_ResetBits(GPIO_SCL, GPIO_SCL_PIN) //SCL

#define IIC_SDA_H    GPIO_SetBits(GPIO_SDA, GPIO_SDA_PIN) //SDA	
#define IIC_SDA_L    GPIO_ResetBits(GPIO_SDA, GPIO_SDA_PIN) //SDA

#define READ_SDA  	 GPIO_ReadInputDataBit(GPIO_SDA, GPIO_SDA_PIN)  //输入SDA 

//IIC所有操作函数
void IIC_Init(void);         	//初始化IIC的IO口				 
void IIC_Start(void);					//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(u8 txd);		//IIC发送一个字节
u8 IIC_Read_Byte(u8 ack);			//IIC读取一个字节	ack:是否需要发送应答信号	表示主机是否还要数据（命令从机发不发数据），数据0表示应答(还要数据)，数据1表示非应答(不要数据)
u8 IIC_Wait_Ack(int timeout);	//单位 us
void IIC_Ack(void);
void IIC_NAck(void);
//u8 IIC_Wait_Ack(void);
#endif
















