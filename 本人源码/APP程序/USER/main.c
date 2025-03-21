#include "stm32f10x.h"
#include "main.h"
#include "usart.h"
#include "delay.h"
#include "AT24C02.h"
#include "W25Q64.h"
#include "Board_Flash.h"

#include "FS4G.h"
#include "MQTT.h"

/**
  ******************************************************************************
  * @project	stm32f103c8t6_usart_dma_idle_interrupt
  * @author  	zd
  * @version 	V1.0
  * @date    	2024.5.26

  ******************************************************************************
  */


OTA_InfoCB OTA_Info;     //保存在24C02内的OTA信息相关的结构体
UpdateA_CB UpdateA;      //A区更新用到的结构体
u32 FS4G_Connect_State_Flag = 0;   //记录全局状态标志位，每位表示1种状态


int main(void)
{	
	u8 i;
	u16 timecount = 0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//配置中断优先级分组
	USART1_Init(921600);	//串口初始化，921600
	USART3_Init(115200);	//串口初始化，115200
	delay_Init();                           //延时初始化
	u1_printf("\r\n串口1初始化成功\r\n");   //串口1初始化
	u3_printf("\r\n串口3初始化成功\r\n");   //串口3初始化
	IIC_Init();                             //IIC初始化
	W25Q64_Init();                          //W25Q64初始化
	

	AT24C02_ReadOTAInfo();                  //从24C02读取数据到OTA_Info结构体
	FS4G_Init();		//复位4G模块
	
	
	u1_printf("2.0.0\r\n");  //串口输出信息   VER-1.0.0-2024-06-08-15.54
  while(1)
	{
		delay_ms(10);
		timecount++;
		
		/*--------------------------------------------------*/
		/*             处理串口3接收缓冲区的数据            */
		/*--------------------------------------------------*/
		//串口3接收
		if(U3_CB.URxDataOUT != U3_CB.URxDataIN){		//检查串口接收缓冲区是否有新数据
//			u1_printf("本次接受%d字节数据\r\n",(U3_CB.URxDataOUT->end - U3_CB.URxDataOUT->start + 1));
//			for(i = 0; i< (U3_CB.URxDataOUT->end - U3_CB.URxDataOUT->start + 1); i++){
//				u1_printf("%c",U3_CB.URxDataOUT->start[i]);
//			}	
			U3_Event_Process(U3_CB.URxDataOUT->start, (U3_CB.URxDataOUT->end - U3_CB.URxDataOUT->start + 1));
			U3_CB.URxDataOUT++;		//读指针后移
			if(U3_CB.URxDataOUT == U3_CB.URxDataEND){
				U3_CB.URxDataOUT = &U3_CB.URxDataPtr[0];	//如果已经到缓冲末尾，则重新回到缓冲区头部
			}
		}
		
		/*--------------------------------------------------*/
		/*             处理串口3发送缓冲区的数据            */
		/*--------------------------------------------------*/
		if((U3_CB.UTxDataOUT != U3_CB.UTxDataIN)&&(U3_CB.UTxState == 0)){                                         //IN 和 OUT不相等 且 发送DMA空闲 的时候进入if，说明缓冲区有数据了
			u1_printf("本次发送%d字节数据\r\n",U3_CB.UTxDataOUT->end - U3_CB.UTxDataOUT->start + 1);             //输出参考消息
			for(i=0;i<U3_CB.UTxDataOUT->end - U3_CB.UTxDataOUT->start + 1;i++)
				u1_printf("%02x ",U3_CB.UTxDataOUT->start[i]);	
			u1_printf("\r\n");
			
			U3_CB.UTxState = 1;                                                                                 //设置发送忙碌
			
			DMA_InitTypeDef DMA_InitStructure;
			DMA_InitStructure.DMA_PeripheralBaseAddr = USART3_BASE + 0x04; 	 					//DMA外设基地址
			DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)U3_CB.UTxDataOUT->start; 					//DMA内存基地址
			DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  											//数据传输方向，从内存发送到外设
			DMA_InitStructure.DMA_BufferSize = U3_CB.UTxDataOUT->end - U3_CB.UTxDataOUT->start + 1; //DMA通道的DMA缓存的大小，
			DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 					//外设地址寄存器不变
			DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  									//内存地址寄存器递增
			DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  	//数据宽度为8位
			DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; 					//数据宽度为8位
			DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  														//工作在正常缓存模式
			DMA_InitStructure.DMA_Priority = DMA_Priority_High; 											//DMA通道 x拥有中优先级 
			DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; 															//DMA通道x没有设置为内存到内存传输
			
			DMA_Init(DMA1_Channel2, &DMA_InitStructure);
			
			DMA_Cmd(DMA1_Channel2, ENABLE);         //开启通道                        
			
			U3_CB.UTxDataOUT++;                                            //OUT后挪
		    if(U3_CB.UTxDataOUT == U3_CB.UTxDataEND){                       //如果挪到了END标记的最后一个成员，进入if
			    U3_CB.UTxDataOUT = &U3_CB.UTxDataPtr[0];                    //重新回到数组0号成员
			}
		}		
	
		
		
		
		if(FS4G_Connect_State_Flag & FS4G_CONFIG_CIPMODE){
			
			if(timecount>800){
				
				FS4G_Connect_State_Flag &= ~FS4G_CONFIG_CIPMODE;
				FS4G_Connect_TCP();
			}
		}
		
		if((timecount>=500) && (FS4G_Connect_State_Flag & CONNECT_OK) && (!(FS4G_Connect_State_Flag&OTA_EVENT))){
				timecount = 0;
				MQTT_PublishDataQs0("/sys/a1RwXdzy6BZ/D001/thing/event/property/post","{\"params\":{\"Light_Switch1\":0}}");
			u1_printf("Version : %s\r\n", OTA_Info.OTA_Version);
		}
	
	}

	
}
