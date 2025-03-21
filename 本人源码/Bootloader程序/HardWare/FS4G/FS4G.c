#include "FS4G.h"



void FS4G_Init(void)
{
	GPIO_InitTypeDef GPIO_Config;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//打开IO口时钟
	
	//RDY			监控4G模块连接状态   联网成功后由低电平转为高电平   GPIOB.0
	GPIO_Config.GPIO_Mode		=	GPIO_Mode_IPD;
	GPIO_Config.GPIO_Pin		=	GPIO_Pin_0;
	GPIO_Config.GPIO_Speed	=	GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_Config);							
	
	//RST			复位引脚	  GPIOB.1初始化
	GPIO_Config.GPIO_Mode		=	GPIO_Mode_Out_PP;
	GPIO_Config.GPIO_Pin		=	GPIO_Pin_1;
	GPIO_Config.GPIO_Speed	=	GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_Config);							
	
	u1_printf("开始复位4G模块，请勿操作！\r\n");
	GPIO_SetBits(GPIOB, GPIO_Pin_1);//复位引脚， 拉高 150ms 以上模块复位
	delay_ms(500);
	GPIO_ResetBits(GPIOB, GPIO_Pin_1);
	u1_printf("复位时间结束！\r\n");
}

void U3_Event_Process(u8 *data, u16 datalen){
	if((datalen == 7) && (memcmp(data,"\r\nRDY\r\n",7) == 0)){
		u1_printf("4G模块复位成功！\r\n");
		BootLoader_Info();
	}
	
	
}


