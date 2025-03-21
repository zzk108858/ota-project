#include "gd32f10x.h"
#include "main.h"
#include "usart.h"
#include "delay.h"
#include "fmc.h"
#include "iic.h"
#include "m24c02.h"
#include "w25q64.h"
#include "4g.h"
#include "mqtt.h"

OTA_InfoCB OTA_Info;          //保存在24C02内的OTA信息相关的结构体
UpDataA_CB UpDataA;           //A区更新用到的结构体
uint32_t BootStaFlag;         //记录全局状态标志位
uint32_t timeout;             //软件定时变量

int main(void)
{
	uint16_t i;              //用于for循环
	
	
	Delay_Init();            //延时初始化
	Usart2_Init(115200);     //串口2初始化
	Usart0_Init(921600);     //串口0初始化
  IIC_Init();              //IIC初始化
	M24C02_ReadOTAInfo();    //从24C02读取数据到OTA_Info结构体
	W25Q64_Init();           //初始化W25Q64
	GM4G_Init();             //同步复位4G模块
	u0_printf("1.0.0\r\n");  //串口输出信息
	/*--------------------------------------------------*/
	/*--------------------主循环------------------------*/
	/*--------------------------------------------------*/
	while(1){                 		
		/*--------------------------------------------------*/
		/*             处理串口2接收缓冲区的数据            */
		/*--------------------------------------------------*/
		if(U2CB.URxDataOUT != U2CB.URxDataIN){                                                                 //IN 和 OUT不相等的时候进入if，说明缓冲区有数据了
			u0_printf("本次接收%d字节数据\r\n",U2CB.URxDataOUT->end - U2CB.URxDataOUT->start + 1);             //输出参考消息
			for(i=0;i<U2CB.URxDataOUT->end - U2CB.URxDataOUT->start + 1;i++)
				u0_printf("%02x ",U2CB.URxDataOUT->start[i]);	
			u0_printf("\r\n");
			U2_Event(U2CB.URxDataOUT->start,U2CB.URxDataOUT->end - U2CB.URxDataOUT->start + 1);                //调用U2_Event处理串口2的数据			
			U2CB.URxDataOUT++;                                                                                 //OUT后挪
		    if(U2CB.URxDataOUT == U2CB.URxDataEND){                                                            //如果挪到了END标记的最后一个成员，进入if
			    U2CB.URxDataOUT = &U2CB.URxDataPtr[0];                                                         //重新回到数组0号成员
			}
		}	
		/*--------------------------------------------------*/
		/*             处理串口2发送缓冲区的数据            */
		/*--------------------------------------------------*/
		if((U2CB.UTxDataOUT != U2CB.UTxDataIN)&&(U2CB.UTxState == 0)){                                         //IN 和 OUT不相等 且 发送DMA空闲 的时候进入if，说明缓冲区有数据了
			u0_printf("本次发送%d字节数据\r\n",U2CB.UTxDataOUT->end - U2CB.UTxDataOUT->start + 1);             //输出参考消息
			for(i=0;i<U2CB.UTxDataOUT->end - U2CB.UTxDataOUT->start + 1;i++)
				u0_printf("%02x ",U2CB.UTxDataOUT->start[i]);	
			u0_printf("\r\n");
			
			U2CB.UTxState = 1;                                                                                 //设置发送忙碌
			dma_memory_address_config(DMA0,DMA_CH1,(uint32_t)U2CB.UTxDataOUT->start);                          //配置新的发送起始地址
			dma_transfer_number_config(DMA0,DMA_CH1,U2CB.UTxDataOUT->end - U2CB.UTxDataOUT->start + 1);        //配置新的发送量
			dma_channel_enable(DMA0,DMA_CH1);                                                                  //开启通道                        
			
			U2CB.UTxDataOUT++;                                                                                 //OUT后挪
		    if(U2CB.UTxDataOUT == U2CB.UTxDataEND){                                                            //如果挪到了END标记的最后一个成员，进入if
			    U2CB.UTxDataOUT = &U2CB.UTxDataPtr[0];                                                         //重新回到数组0号成员
			}
		}
		/*--------------------------------------------------*/
		/*                       软件定时                   */
		/*--------------------------------------------------*/
		Delay_Ms(10);                                                                     //延时10ms
		timeout++;                                                                        //变量自增1
		if((timeout>=500)&&(BootStaFlag&CONNECT_OK)&&(!(BootStaFlag&OTA_EVENT))){         //超过500*10=5000ms计时 且  CONNECT报文成功  且  没有OTA事件 进入if
			timeout = 0;                                                                  //清除计时变量
			//等级0的Publish报文上传数据
			MQTT_PublishDataQs0("/sys/a1HvxPcHnkX/D001/thing/event/property/post","{\"params\":{\"temperature\":33.33,\"Humidity\":22.22,\"Light\":22.22}}");
		}

	}
}
