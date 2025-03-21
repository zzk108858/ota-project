#include "boot.h"

void BootLoader_Branch(void)
{
	if(BootLoader_Enter_Command(20) == 0){
		if(OTA_Info.OTA_flag == OTA_SET_FLAG){
			u1_printf("OTA有更新！\r\n");
			BootStateFlag |= UPDATA_A_FLAG;
			UpdateA.Updata_A_from_W25Q64_Num = 0;
			
		}else{
			u1_printf("OTA无更新，跳转A分区代码\r\n");
			LOAD_A(STM32_A_START_ADDR);
		}
	}else{
		
		u1_printf("进入Bootloader命令行\r\n");
		
	}
}

u8 BootLoader_Enter_Command(u8 timeout){ //timeout 单位 百毫秒
	u1_printf("%d ms 内输入 w ，进入命令行\r\n", timeout * 100);
	while(timeout--){
		delay_ms(100);
		if(USART1_RX_BUF[0] == 'w'){
				return 1;
		}
	}
	return 0;
}

void BootLoader_Info(void){
	u1_printf("\r\n");
	u1_printf("[1]擦除A区\r\n");
	u1_printf("[2]串口IAP下载A区程序\r\n");
	u1_printf("[3]设置OTA版本号\r\n");
	u1_printf("[4]查询OTA版本号\r\n");
	u1_printf("[5]向外部Flash下载程序\r\n");
	u1_printf("[6]使用外部Flash内程序\r\n");
	u1_printf("[7]重启\r\n");
	u1_printf("[8]设置服务器连接信息\r\n");
}

void Bootloader_Event_Process(u8 *data, u16 datalen)
{
	int Version_sscanf_temp;
	u8 i;
	if(BootStateFlag == 0){	//BootStateFlagd等于0，刚上电的状态，这时只需要检测接收到的指令编号 
		if((datalen == 1) && (data[0] == '1')){
			u1_printf("擦除A区\r\n");
			STM32_EraseFlash(STM32_A_START_PAGE, STM32_A_PAGE_NUM);//将A区空间擦除
		}
		else if((datalen == 1) && (data[0] == '2')){
			u1_printf("通过XModem协议，从串口IAP下载A区程序，请使用bin文件格式\r\n");
			STM32_EraseFlash(STM32_A_START_PAGE, STM32_A_PAGE_NUM);//将A区空间擦除，才能让新程序烧录
			BootStateFlag |= (IAP_XModem_C_FLAG | IAP_XModem_DATA_FLAG);
			UpdateA.XModem_Timer_Count = 0;
			UpdateA.XModem_Count = 0;
		}
		else if((datalen == 1) && (data[0] == '3')){
			u1_printf("设置OTA版本号\r\n");
			BootStateFlag |= IAP_WRITE_VERSION_FLAG;
		}
		else if((datalen == 1) && (data[0] == '4')){
			u1_printf("查询OTA版本号\r\n");
			AT24C02_ReadOTAInfo();
			u1_printf("\r\n版本号：%s\r\n",OTA_Info.OTA_Version);
			BootLoader_Info();
		}
		else if((datalen == 1) && (data[0] == '5')){
			u1_printf("向外部Flash下载程序，输入需要使用的块编号（1~9）\r\n");
			BootStateFlag |= CMD_5_FLAG;
		}
		else if((datalen == 1) && (data[0] == '6')){
			u1_printf("使用外部Flash内程序，输入需要使用的块编号（1~9）\r\n");
			BootStateFlag |= CMD_6_FLAG;
		}
		else if((datalen == 1) && (data[0] == '7')){
			u1_printf("重启\r\n");
			delay_ms(50);
			NVIC_SystemReset();
		}else if((datalen == 1) && (data[0] == '8')){
			u1_printf("请设置服务器连接信息\r\n");
			BootStateFlag |= CMD_8_FLAG;
		}
	}
	else if(BootStateFlag & IAP_XModem_DATA_FLAG){	//接受XModem数据状态
		if((datalen == 133) && data[0] == 0x01){	//接收到133字节数据并且以包头起始，判定为XModem数据，进入处理
			BootStateFlag &= ~IAP_XModem_C_FLAG;	//停止发送C
			UpdateA.XModemCRC = XModem_CRC16(&data[3], 128);
			if(UpdateA.XModemCRC == ((data[131]<<8) | data[132])){//校验数据，没有问题再进行处理
				memcpy(&UpdateA.Updata_A_Buff[(UpdateA.XModem_Count % (STM32_PAGE_SIZE/128)) * 128], &data[3], 128);
				UpdateA.XModem_Count++;
				if((UpdateA.XModem_Count % (STM32_PAGE_SIZE/128)) == 0){	//说明缓冲区存满了1页，可以向内部Flash写入了
					
					if(BootStateFlag & CMD_5_XMODEM_FLAG){
						/*	W25Q64最大一次写入256字节	*/
						
						for(i = 0; i < (STM32_PAGE_SIZE/256); i++){
							W25Q64_PageWrite(&UpdateA.Updata_A_Buff[i*256], UpdateA.Updata_A_from_W25Q64_Num * 64 * 1024 / 256 + (UpdateA.XModem_Count-8)*128/256+i);
						}
					}else{
						STM32_WriteFlash(STM32_A_START_ADDR + (UpdateA.XModem_Count/(STM32_PAGE_SIZE/128) - 1 ) * STM32_PAGE_SIZE, (u32 *)UpdateA.Updata_A_Buff, STM32_PAGE_SIZE);	
					}
					
				}
				u1_printf("\x06");//一个包传输完成，回复应答信号
			}else{	//校验数据错误
				u1_printf("\x15");//传输错误，回复非正常响应
			}
		}
		if((datalen == 1) && (data[0] == 0x04)){	//接收到结束传输标志
			u1_printf("\x06");//一个包传输完成，回复应答信号
			
			//判断有没有不到1页大小的数据，有的话将这部分写入
			if((UpdateA.XModem_Count % (STM32_PAGE_SIZE/128)) != 0){
				
				if(BootStateFlag & CMD_5_XMODEM_FLAG){
						/*	W25Q64最大一次写入256字节	*/
						
						for(i = 0; i < (STM32_PAGE_SIZE/256); i++){
							W25Q64_PageWrite(&UpdateA.Updata_A_Buff[i*256], UpdateA.Updata_A_from_W25Q64_Num * 64 * 4 + (UpdateA.XModem_Count)/8*4+i);
						}
				}else{
			
					STM32_WriteFlash(STM32_A_START_ADDR + (UpdateA.XModem_Count/(STM32_PAGE_SIZE/128)) * STM32_PAGE_SIZE, (u32 *)UpdateA.Updata_A_Buff, UpdateA.XModem_Count%(STM32_PAGE_SIZE/128)*128);	
				}
			}
			BootStateFlag &= (~IAP_XModem_DATA_FLAG);
			
			if(BootStateFlag & CMD_5_XMODEM_FLAG){
				BootStateFlag &= ~CMD_5_XMODEM_FLAG;
				OTA_Info.FirmwareLen[UpdateA.Updata_A_from_W25Q64_Num] = UpdateA.XModem_Count * 128;
				AT24C02_WriteOTAInfo();
				delay_ms(50);
				BootLoader_Info();
			}else{
				u1_printf("重启\r\n");
				delay_ms(100);
				NVIC_SystemReset(); 
			}				
		}
	}else if(BootStateFlag & IAP_WRITE_VERSION_FLAG){	//设置版本号	VER-1.0.0-2024-06-08-15.54
		u1_printf("datalen = %d\r\n", datalen);
		if(datalen==26){
			if(sscanf((char *)data,"VER-%d.%d.%d-%d-%d-%d-%d.%d", &Version_sscanf_temp, &Version_sscanf_temp, &Version_sscanf_temp, &Version_sscanf_temp, &Version_sscanf_temp, &Version_sscanf_temp, &Version_sscanf_temp, &Version_sscanf_temp) == 8){
				memset(OTA_Info.OTA_Version,0,32);
				memcpy(OTA_Info.OTA_Version,data,26);
				AT24C02_WriteOTAInfo();
				u1_printf("版本号设置成功\r\n",OTA_Info.OTA_Version);
				BootStateFlag &= (~IAP_WRITE_VERSION_FLAG);
				BootLoader_Info();
			}else{
				u1_printf("版本号格式错误，退出格式设置\r\n");
				BootStateFlag &= (~IAP_WRITE_VERSION_FLAG);
				BootLoader_Info();
			}
		}else{
			u1_printf("版本号长度错误，退出格式设置\r\n");
			BootStateFlag &= (~IAP_WRITE_VERSION_FLAG);
			BootLoader_Info();
		}
	}else if(BootStateFlag & CMD_5_FLAG){	//向外部Flash下载程序
		if(datalen == 1){
			if((data[0] >= '1') && (data[0] <= '9')){
				UpdateA.Updata_A_from_W25Q64_Num = data[0] - '0';	//存入待下载区的编号，使用XModem协议传输
				BootStateFlag |= (IAP_XModem_C_FLAG | IAP_XModem_DATA_FLAG | CMD_5_XMODEM_FLAG);
				UpdateA.XModem_Timer_Count = 0;
				UpdateA.XModem_Count = 0;
				OTA_Info.FirmwareLen[UpdateA.Updata_A_from_W25Q64_Num] = 0;
				W25Q64_Erase64K(UpdateA.Updata_A_from_W25Q64_Num);
				u1_printf("通过XModem协议，向W25Q64第%d块下载A区程序，请使用bin文件格式\r\n",UpdateA.Updata_A_from_W25Q64_Num);
				BootStateFlag &= ~CMD_5_FLAG;
				
			}else{
				u1_printf("请传入1~9的编号\r\n");
			}
		}else{
			u1_printf("输入参数长度错误\r\n");
		}
		
	}else if(BootStateFlag & CMD_6_FLAG){	//使用外部Flash内程序
		if(datalen == 1){
			if((data[0] >= '1') && (data[0] <= '9')){
				UpdateA.Updata_A_from_W25Q64_Num = data[0] - '0';
				u1_printf("通过第%d块下载A区程序\r\n",UpdateA.Updata_A_from_W25Q64_Num);
				BootStateFlag |= UPDATA_A_FLAG;
				BootStateFlag &= ~CMD_6_FLAG;
			}else{
				u1_printf("请传入1~9的编号\r\n");
			}
		}else{
			u1_printf("输入参数长度错误\r\n");
		}
		
	}else if(BootStateFlag & CMD_8_FLAG){	    	//设置TCPIP应用为透明传输模式	0非透 1透
//		u3_printf("AT+CIPMODE=0\r\n");
//		delay_ms(30);
//		u3_printf("AT+CIPMODE=0\r\n");
//		delay_ms(30);
//		u3_printf("AT+CIPSTART=%s\r\n", data);		//AT+CIPSTART="TCP",test.usr.cn,2317	建立 TCP 连接或注册 UDP 端口号
//		delay_ms(30);
//		u3_printf("AT+CIPSTART=%s\r\n", data);		//AT+CIPSTART="TCP",test.usr.cn,2317	建立 TCP 连接或注册 UDP 端口号
//		delay_ms(30);
//		u3_printf("AT^HEARTCONFIG=1,0,60\r\n");	 	//配置心跳的相关参数
//		delay_ms(30);
//		u3_printf("AT^HEARTBEATHEX=4,C000\r\n"); 	//设置 HEX 编码格式心跳包内容
//		delay_ms(30);
//		u3_printf("AT^HEARTCONFIG=1,0,60\r\n");	 	//配置心跳的相关参数
//		delay_ms(30);
//		u3_printf("AT^HEARTBEATHEX=4,C000\r\n"); 	//设置 HEX 编码格式心跳包内容
//		delay_ms(30);
//		u3_printf("AT&W\r\n"); 										//保存用户当前的配置
//		delay_ms(30);
		
		BootStateFlag &= ~CMD_8_FLAG;
	}
	
}


__ASM void MSR_SP(u32 addr)
{
	MSR MSP, R0
	BX R14
}

void LOAD_A(u32 addr)
{
	pFunction Jump_To_Application;
	
	if((*(u32 *)addr >= 0x20000000) && (*(u32 *)addr <= 0x20004fff)){
		u1_printf("A分区代码MSP有效性判断通过\r\n");
		MSR_SP(*(u32 *)addr);
		Jump_To_Application = (pFunction)*(u32 *)(addr + 4);	
		BootLoader_Clear();
		Jump_To_Application();
	}else{
		u1_printf("A分区代码MSP有效性判断不通过，跳转A分区失败！\r\n");
	}
}

void BootLoader_Clear(void)
{
	USART_DeInit(USART1);
	
}

u16 XModem_CRC16(u8 *data, u16 datalen)
{
	u16 CRC16_INIT = 0x0000;
	u16 CRC16_POLY = 0x1021;
	
	while(datalen){
		CRC16_INIT = ((*data) << 8) ^ CRC16_INIT;
		for(u8 i=0; i < 8; i++){
			if((CRC16_INIT & 0x8000) == 0){
				CRC16_INIT = CRC16_INIT << 1;
			}else{
				CRC16_INIT = (CRC16_INIT << 1) ^ CRC16_POLY;
			}
		}
		data++;
		datalen--;
	}
	return CRC16_INIT;
}


