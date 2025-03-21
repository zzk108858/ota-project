#include "AT24C02.h"
#include "usart.h"
#include "delay.h"
#include <string.h>
#include "main.h"

/**
	*	@param	addr:		��д��ĵ�ַ��ȡֵ����0~255	at24c02�ռ��С2k�ֽ�	
	*	@param	wdata:	��дֵ��8λ
	*	@retval	0������
						�������쳣
	*/

u8 AT24C02_WriteByte(u8 addr, u8 wdata)
{
	IIC_Start();
	IIC_Send_Byte(AT24C02_WADDR);					//д�豸��ַ
	if(IIC_Wait_Ack(100) != 0)	return 1;//�ȴ�Ӧ���ź���������򷵻�
	IIC_Send_Byte(addr);									//д�洢��ַ
	if(IIC_Wait_Ack(100) != 0)	return 2;	//�ȴ�Ӧ���ź���������򷵻�
	IIC_Send_Byte(wdata);									//д����
	if(IIC_Wait_Ack(100) != 0)	return 3;	//�ȴ�Ӧ���ź���������򷵻�
	IIC_Stop();	
	return 0;
}


/**
	*	@param	addr:		��д�����ʼ��ַ��ȡֵ����0~255��Ӧ�ɴ�д���ҳ��*8�õ���at24c02֧�����8�ֽ�һҳд��	
	*	@param	wdata:	��д���飬Ӧ������8�ֽڣ�����������Ч
	*	@retval	0������
						�������쳣
	*/
u8 AT24C02_WritePage(u8 addr, u8 *wdata)	//��ҳд�� ��The 1K/2K EEPROM is capable of an 8-byte page write��
{
	u8 i;
	
	IIC_Start();
	IIC_Send_Byte(AT24C02_WADDR);					//д�豸��ַ
	if(IIC_Wait_Ack(100) != 0)	return 1;	//�ȴ�Ӧ���ź���������򷵻�
	IIC_Send_Byte(addr);									//д�洢��ַ
	if(IIC_Wait_Ack(100) != 0)	return 2;	//�ȴ�Ӧ���ź���������򷵻�
	
	for(i = 0;i < 8; i++){
		IIC_Send_Byte(wdata[i]);									//д����
		if(IIC_Wait_Ack(100) != 0)	return 3+i;	//�ȴ�Ӧ���ź���������򷵻�
	}
	
	IIC_Stop();	
	return 0;
}

/**
	*	@param	addr:		����ȡ�ĵ�ַ��ȡֵ����0~255
	*	@param	rdata:	��ȡֵ�洢��ַ
	*	@retval	0������
						�������쳣
	*/
u8 AT24C02_ReadByte(u8 addr, u8 *rdata)	//
{
	IIC_Start();
	IIC_Send_Byte(AT24C02_WADDR);					//д�豸��ַ
	if(IIC_Wait_Ack(100) != 0)	return 1;	//�ȴ�Ӧ���ź���������򷵻�
	IIC_Send_Byte(addr);									//д�洢��ַ
	if(IIC_Wait_Ack(100) != 0)	return 2;	//�ȴ�Ӧ���ź���������򷵻�
	IIC_Start();
	IIC_Send_Byte(AT24C02_RADDR);	
	if(IIC_Wait_Ack(100) != 0)	return 3;
	

	*rdata = IIC_Read_Byte(0);
	
	IIC_Stop();	
	return 0;
}


/*
	*	2024.6.5 zd Add
	*	ָ����ַָ�����ȶ�ȡ����һֱ�����⣬���ܶ�ȡ��һ���ֽڣ���棬���պ���bug����ʱ��ReadByteѭ����ʵ�ֱ�����

u8 AT24C02_ReadData(u8 addr, u8 *rdata, u16 datalen)	//
{
	u8 i;
	IIC_Start();
	IIC_Send_Byte(AT24C02_WADDR);					//д�豸��ַ
	if(IIC_Wait_Ack(100) != 0)	return 1;	//�ȴ�Ӧ���ź���������򷵻�
	IIC_Send_Byte(addr);									//д�洢��ַ
	if(IIC_Wait_Ack(100) != 0)	return 2;	//�ȴ�Ӧ���ź���������򷵻�
	IIC_Start();
	IIC_Send_Byte(AT24C02_RADDR);	
	if(IIC_Wait_Ack(100) != 0)	return 3;
	
	for(i = 0;i < datalen - 1; i++){
		rdata[i] = IIC_Read_Byte(1);
	}
	rdata[datalen-1] = IIC_Read_Byte(0);
	
	IIC_Stop();	
	return 0;

}
*/

/**
	*	@note		2024.6.5 zd Add
	*					ָ����ַָ�����ȶ�ȡ����һֱ�����⣬���ܶ�ȡ��һ���ֽڣ���棬���պ���bug����ʱ��ReadByteѭ����ʵ�ֱ�����
	*	
	*	@param	addr:		����ȡ����ʼ��ַ��ȡֵ����0~255
	*	@param	rdata:	��ȡ�洢���飬������256�ֽڣ�at24c02���256�ֽ�
	*	@param	datalen:����ȡ����
	*/
u8 AT24C02_ReadData(u8 addr, u8 *rdata, u16 datalen)
{ 
	while(datalen)
	{
		AT24C02_ReadByte(addr++, rdata);
		rdata++;
		datalen--;
	}
	return 0;
}

/**
	*	@brief	���AT24C02�Ƿ����������д
	*	
	*	@retval	0������
						1���쳣
	*/
u8 AT24C02_Check(void)
{
	u8 buf;
	AT24C02_ReadByte(255, &buf);//����ÿ�ο�����дAT24CXX			   
	if((buf)==0X55){
		//u1_printf("1��rdata�����ݣ�0x%x\r\n",buf);
		return 0;
	}else{				//�ų���һ�γ�ʼ�������
		AT24C02_WriteByte(255,0X55);
		delay_ms(5);
		AT24C02_ReadByte(255, &buf);
		//u1_printf("2��rdata�����ݣ�0x%x\r\n",buf);		
		if((buf)==0X55)
			return 0;
	}
	return 1;											  
}


void AT24C02_ReadOTAInfo(void)
{
	memset(&OTA_Info ,0 , OTA_INFOCB_SIZE);
	AT24C02_ReadData(0, (u8 *)&OTA_Info, OTA_INFOCB_SIZE);
}

void AT24C02_WriteOTAInfo(void)
{
	for(u8 i = 0;i < OTA_INFOCB_SIZE/8; i++){
		AT24C02_WritePage(i*8, ((u8 *)&OTA_Info) + i*8);
		delay_ms(5);
	}
}


