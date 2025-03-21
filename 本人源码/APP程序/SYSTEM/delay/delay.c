#include "delay.h"


void delay_Init(void){
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
}

void delay_us(u16 us){
	SysTick->LOAD = 72 * us;	//�δ�ʱ�ӵĵ���ʱ����ֵ
	SysTick->VAL	=	0x00;			//�Ե�ǰ��������ֵ��SysTick Current Value Register��дֵ����ʹ�����㣬�����SysTick���Ƽ�״̬�Ĵ�����SysTick Control and Status Register���е�COUNTFLAG��־
	SysTick->CTRL	|= SysTick_CTRL_ENABLE_Msk;	//ʹ�ܵδ�ʱ��
	while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));	//�ȴ��δ����������ʱ����
	SysTick->CTRL	&= ~SysTick_CTRL_ENABLE_Msk;	//ʧ�ܵδ�ʱ��
	
}

void delay_ms(u16 ms){
	while(ms--)
	{
		delay_us(1000);
	}
}

