#include "FS4G.h"



void FS4G_Init(void)
{
	GPIO_InitTypeDef GPIO_Config;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//��IO��ʱ��
	
	//RDY			���4Gģ������״̬   �����ɹ����ɵ͵�ƽתΪ�ߵ�ƽ   GPIOB.0
	GPIO_Config.GPIO_Mode		=	GPIO_Mode_IPD;
	GPIO_Config.GPIO_Pin		=	GPIO_Pin_0;
	GPIO_Config.GPIO_Speed	=	GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_Config);							
	
	//RST			��λ����	  GPIOB.1��ʼ��
	GPIO_Config.GPIO_Mode		=	GPIO_Mode_Out_PP;
	GPIO_Config.GPIO_Pin		=	GPIO_Pin_1;
	GPIO_Config.GPIO_Speed	=	GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_Config);							
	
	u1_printf("��ʼ��λ4Gģ�飬���������\r\n");
	GPIO_SetBits(GPIOB, GPIO_Pin_1);//��λ���ţ� ���� 150ms ����ģ�鸴λ
	delay_ms(500);
	GPIO_ResetBits(GPIOB, GPIO_Pin_1);
	u1_printf("��λʱ�������\r\n");
}

void U3_Event_Process(u8 *data, u16 datalen){
	if((datalen == 7) && (memcmp(data,"\r\nRDY\r\n",7) == 0)){
		u1_printf("4Gģ�鸴λ�ɹ���\r\n");
		BootLoader_Info();
	}
	
	
}


