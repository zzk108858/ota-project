#include "gd32f10x.h"
#include "delay.h"

/*-------------------------------------------------*/
/*����������ʱ��ʼ��                               */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void Delay_Init(void)
{
	systick_clksource_set(SYSTICK_CLKSOURCE_HCLK);    //����Systickʱ��
}
/*-------------------------------------------------*/
/*����������ʱ΢��                                 */
/*��  ����us����ʱ����΢��                         */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void Delay_Us(uint16_t us)
{
	SysTick->LOAD = us*108;                                //��������ֵ
	SysTick->VAL = 0x00;                                   //�����ǰ������ֵ
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;              //��Systick
	while(!(SysTick->CTRL&SysTick_CTRL_COUNTFLAG_Msk));    //�ȵ�Systick��LOAD����ֵ������0����־λ��λ����ʾ��ʱʱ�䵽
	SysTick->CTRL &=~ SysTick_CTRL_ENABLE_Msk;             //��Systick
} 
/*-------------------------------------------------*/
/*����������ʱ����                                 */
/*��  ����us����ʱ���ٺ���                         */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void Delay_Ms(uint16_t ms)
{
	while(ms--){             //ѭ��
		Delay_Us(1000);      //ÿ����ʱ1000us
	}
}


