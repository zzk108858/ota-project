#include "iic.h"
#include "delay.h"
 
//初始化IIC
void IIC_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	//使能GPIOB时钟
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD ;   //开漏输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	IIC_SCL_H;//PB6,PB7 输出高
	IIC_SDA_H;
}
//产生IIC起始信号
void IIC_Start(void)
{

	IIC_SCL_H;
	IIC_SDA_H;
	delay_us(4);
	IIC_SDA_L;
	delay_us(4);
	IIC_SCL_L;//钳住I2C总线，准备发送或接收数据 
	
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
	
	IIC_SCL_L;
	IIC_SDA_L;//STOP:when CLK is high DATA change form low to high
	delay_us(4);
	IIC_SCL_H;
	IIC_SDA_H;
	delay_us(4);//发送I2C总线结束信号
}

void IIC_Send_Byte(u8 txd)
{
	int8_t i;
	
	
	for(i = 7; i >= 0; i--){
		IIC_SCL_L;//拉低时钟开始数据传输
		if(txd & (0x1<<(i)))
			IIC_SDA_H;
		else
			IIC_SDA_L;
		delay_us(2);
		IIC_SCL_H;
		delay_us(2);

	}
	IIC_SCL_L;
	IIC_SDA_H;	//发送一个字节后，将SDA拉高，等待从机拉低SDA的应答信号

}

u8 IIC_Read_Byte(u8 ack)	//ack:是否需要发送应答信号，	表示主机是否还要数据（命令从机发不发数据），数据0表示应答(还要数据)，数据1表示非应答(不要数据)
{
	int8_t i;
	u8 rxd = 0;	//用于接收
	
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
	//读取一个字节后，将SCL拉低，等待从机释放SDA
//	IIC_SCL_L;
//	delay_us(2);
	
	if(ack){	//如果主机还需要数据		数据0表示应答
		
		IIC_Ack();
		
	}else{		//如果主机不需要数据		数据1表示非应答
		
		IIC_NAck();
	}
	
	return rxd;
}



//产生ACK应答
void IIC_Ack(void)
{
	IIC_SDA_L;
	IIC_SCL_H;
	delay_us(2);
	IIC_SCL_L;
	IIC_SDA_H;
	delay_us(2);
}
//不产生ACK应答		    
void IIC_NAck(void)
{
	IIC_SDA_H;
	IIC_SCL_H;
	delay_us(2);
	IIC_SCL_L;
	delay_us(2);
}		


//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(int timeout)	//单位 us
{
	#if 0
	do{
		delay_us(1);
		timeout--;
	}while((READ_SDA) && (timeout >= 0));
	
	if(timeout < 0)	return 1;	//超时退出
	
	IIC_SCL_H;
	delay_us(2);
	if(READ_SDA != 0)	return 2;	//不是稳定低电平，错误退出
	
	//得到稳定低电平，也就是从机的应答信号，将SCL拉低防止误操作
	IIC_SCL_L;
	delay_us(2);
	return 0;	//正常返回
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



