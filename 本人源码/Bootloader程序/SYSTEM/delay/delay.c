#include "delay.h"


void delay_Init(void){
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
}

void delay_us(u16 us){
	SysTick->LOAD = 72 * us;	//滴答时钟的倒计时重载值
	SysTick->VAL	=	0x00;			//对当前计数器的值（SysTick Current Value Register）写值，会使其清零，并清除SysTick控制及状态寄存器（SysTick Control and Status Register）中的COUNTFLAG标志
	SysTick->CTRL	|= SysTick_CTRL_ENABLE_Msk;	//使能滴答定时器
	while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));	//等待滴答计数器倒计时结束
	SysTick->CTRL	&= ~SysTick_CTRL_ENABLE_Msk;	//失能滴答定时器
	
}

void delay_ms(u16 ms){
	while(ms--)
	{
		delay_us(1000);
	}
}

