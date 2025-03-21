#include "stm32f10x.h"
#include "main.h"
#include "usart.h"
#include "delay.h"
#include "AT24C02.h"
#include "W25Q64.h"
#include "Board_Flash.h"
#include "boot.h"
#include "FS4G.h"


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
u32 BootStateFlag = 0;   //记录全局状态标志位，每位表示1种状态


int main(void)
{	
	u8 i;
	USART1_Init(921600);	//串口初始化，为了更快传输程序，波特率使用比较高的921600
	USART3_Init(115200);	//串口初始化，为了更快传输程序，波特率使用比较高的115200
	delay_Init();                           //延时初始化
	u1_printf("\r\n串口1初始化成功\r\n");   //串口1初始化
	u3_printf("\r\n串口3初始化成功\r\n");   //串口3初始化
	IIC_Init();                             //IIC初始化
	W25Q64_Init();                          //W25Q64初始化
	

	AT24C02_ReadOTAInfo();                  //从24C02读取数据到OTA_Info结构体
	BootLoader_Branch();                    //分支判断
	FS4G_Init();		//复位4G模块
	
  while(1)
	{
		delay_ms(10);
		
		//串口1接收
		if(U1_CB.URxDataOUT != U1_CB.URxDataIN){		//检查串口接收缓冲区是否有新数据
			Bootloader_Event_Process(U1_CB.URxDataOUT->start, (U1_CB.URxDataOUT->end - U1_CB.URxDataOUT->start + 1));	//根据新数据处理对应任务

			U1_CB.URxDataOUT++;		//读指针后移
			if(U1_CB.URxDataOUT == U1_CB.URxDataEND){
				U1_CB.URxDataOUT = &U1_CB.URxDataPtr[0];	//如果已经到缓冲末尾，则重新回到缓冲区头部
			}
		}
		
		//串口3接收
		if(U3_CB.URxDataOUT != U3_CB.URxDataIN){		//检查串口接收缓冲区是否有新数据
			u1_printf("本次接受%d字节数据\r\n",(U3_CB.URxDataOUT->end - U3_CB.URxDataOUT->start + 1));
			for(i = 0; i< (U3_CB.URxDataOUT->end - U3_CB.URxDataOUT->start + 1); i++){
				u1_printf("%c",U3_CB.URxDataOUT->start[i]);
			}	
			U3_Event_Process(U3_CB.URxDataOUT->start, (U3_CB.URxDataOUT->end - U3_CB.URxDataOUT->start + 1));
			U3_CB.URxDataOUT++;		//读指针后移
			if(U3_CB.URxDataOUT == U3_CB.URxDataEND){
				U3_CB.URxDataOUT = &U3_CB.URxDataPtr[0];	//如果已经到缓冲末尾，则重新回到缓冲区头部
			}
		}
		
		
		if(BootStateFlag & IAP_XModem_C_FLAG){		//串口发送XModem协议的起始C
			if(UpdateA.XModem_Timer_Count >= 100){
				u1_printf("C");
				UpdateA.XModem_Timer_Count = 0;
			}
			UpdateA.XModem_Timer_Count++;
		}
		
		
		
		//这之前应该已经把APP代码存放到W25Q64中了
		/*--------------------------------------------------*/
		/*        UPDATA_A_FLAG置位，表明需要更新A区        */
		/*--------------------------------------------------*/
		if( BootStateFlag & UPDATA_A_FLAG ){
			u1_printf("本次需要更新的大小：%d字节\r\n", OTA_Info.FirmwareLen[UpdateA.Updata_A_from_W25Q64_Num]);
			if((OTA_Info.FirmwareLen[UpdateA.Updata_A_from_W25Q64_Num] % 4) == 0){	//判断长度是否是4字节（32位）的整数倍，是的话则允许写入，进入if
				STM32_EraseFlash(STM32_A_START_PAGE, STM32_A_PAGE_NUM);//将A区空间擦除
				u1_printf("A区已擦除\r\n");
				/* 从W25Q64中读取1k并写入片上Flash */
				for(i = 0; i < (OTA_Info.FirmwareLen[UpdateA.Updata_A_from_W25Q64_Num] / STM32_PAGE_SIZE); i++){	//先写完整的1k字节
					W25Q64_Read(UpdateA.Updata_A_Buff, i*STM32_PAGE_SIZE + 64*1024*UpdateA.Updata_A_from_W25Q64_Num, STM32_PAGE_SIZE);//读取到1k内容

					STM32_WriteFlash(STM32_A_START_ADDR + i*STM32_PAGE_SIZE, (u32 *)UpdateA.Updata_A_Buff, STM32_PAGE_SIZE);//将A区写入片上Flash位置
				}
				//现在写完了前面完整的几k内容，下面写不到1k的内容
				if((OTA_Info.FirmwareLen[UpdateA.Updata_A_from_W25Q64_Num] % 1024) != 0){
					memset(UpdateA.Updata_A_Buff,0, STM32_PAGE_SIZE);
					
					W25Q64_Read(UpdateA.Updata_A_Buff,i*1024 + UpdateA.Updata_A_from_W25Q64_Num*64*1024 ,OTA_Info.FirmwareLen[UpdateA.Updata_A_from_W25Q64_Num] % 1024);

					u1_printf("\r\n");
					STM32_WriteFlash(STM32_A_START_ADDR  + i*STM32_PAGE_SIZE, (u32 *)UpdateA.Updata_A_Buff, (OTA_Info.FirmwareLen[UpdateA.Updata_A_from_W25Q64_Num] % 1024));//将A区写入片上Flash位置
				}
				
				if(UpdateA.Updata_A_from_W25Q64_Num == 0){
					//更新完成，清除OTA标志位
					OTA_Info.OTA_flag = 0;
					AT24C02_WriteOTAInfo();
				}
				
				u1_printf("A区更新完成，马上重启系统！\r\n");
				delay_ms(100);
				NVIC_SystemReset();
			}else{
				u1_printf("待更新APP长度错误！\r\n");
				BootStateFlag  &= ~(UPDATA_A_FLAG);
			}
		}

	}
}
