#include "FS4G.h"
#include "MQTT.h"
#include "W25Q64.h"
#include "AT24C02.h"
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
		
		FS4G_Connect_State_Flag |= FS4G_CONFIG_CIPMODE;
		
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
		
		//开启PB0 RDY的中断
		EXTI_DeInit();
		
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);
		
		EXTI_InitTypeDef EXTI_InitStruct;
		EXTI_InitStruct.EXTI_Line      =	EXTI_Line0;
		EXTI_InitStruct.EXTI_LineCmd   =	ENABLE;
		EXTI_InitStruct.EXTI_Mode      =	EXTI_Mode_Interrupt;
		EXTI_InitStruct.EXTI_Trigger   =	EXTI_Trigger_Rising_Falling;

		EXTI_Init(&EXTI_InitStruct);
		
		NVIC_InitTypeDef EXTI0_NVIC_Config;
		
		EXTI0_NVIC_Config.NVIC_IRQChannel										= EXTI0_IRQn;
		EXTI0_NVIC_Config.NVIC_IRQChannelCmd								= ENABLE;
		EXTI0_NVIC_Config.NVIC_IRQChannelPreemptionPriority	= 1;
		EXTI0_NVIC_Config.NVIC_IRQChannelSubPriority				= 0;
		NVIC_Init(&EXTI0_NVIC_Config);	
	}
	
	if((datalen == 4)&&(data[0]==0x20)){                                           //如果接收到4个字节 且是 第一个字节是0x20，进入if
		u1_printf("收到CONNACK报文\r\n");	                                       //串口发送数据
		if(data[3]==0x00){                                                         //判断第4个字节，如果是0x00，进入if
			u1_printf("CONNECT报文成功连接服务器\r\n");                            //串口发送数据
			FS4G_Connect_State_Flag |= CONNECT_OK;                                             //设置标志位，表示CONNECT报文成功
			MQTT_SubcribPack("/sys/a1RwXdzy6BZ/D001/thing/service/property/set");  //发送订阅Topic报文
			MQTT_SubcribPack("/sys/a1RwXdzy6BZ/D001/thing/file/download_reply");   //发送订阅Topic报文
			OTA_Version(); 
		}else{                                                                     //判断第4个字节，如果不是0x00，进入else
			u1_printf("CONNECT报文错误，准备重启\r\n");                            //串口发送数据
			NVIC_SystemReset();                                                    //重启
		}                   	
	}
	
	if((datalen == 5)&&(data[0]==0x90)){                          //如果接收到5个字节 且是 第一个字节是0x90，进入if
		u1_printf("收到SUBACK报文\r\n");	                      //串口发送数据
		if((data[datalen-1]==0x00)||(data[datalen-1]==0x01)){     //判断接收的最后一个字节，如果是0x00或则0x01.进入if
			u1_printf("SUBCRIBE订阅报文成功\r\n");                //串口发送数据
		}else{													  //判断接收的最后一个字节，如果不是0x00或则0x01.进入else									
			u1_printf("SUBCRIBE订阅报文错误，准备重启\r\n");      //串口发送数据
			NVIC_SystemReset();                                   //重启
		}                   	
	}
	
	
	
	
	
	
	if((FS4G_Connect_State_Flag&CONNECT_OK)&&(data[0]==0x30)){                      //如果CONNECT报文成功 且是 第一个字节是0x30，进入if
		u1_printf("收到等级0的PUBLISH报文\r\n");                        //串口发送数据	
		MQTT_DealPublishData(data,datalen);                             //提取数据
		u1_printf("%s\r\n",Aliyun_mqtt.CMD_buff);                       //串口输出提取的数据
		if(strstr((char *)Aliyun_mqtt.CMD_buff,"{\"Light_Switch1\":0}")){     //搜索关键词，如果搜索到进入if
			u1_printf("关闭开关\r\n");                                 //串口发送数据

			//发送数据，同步开关状态
			MQTT_PublishDataQs0("/sys/a1RwXdzy6BZ/D001/thing/event/property/post","{\"params\":{\"Switch1\":0}}");
		}
		if(strstr((char *)Aliyun_mqtt.CMD_buff,"{\"Light_Switch1\":1}")){     //搜索关键词，如果搜索到进入if
			u1_printf("打开开关\r\n");                                 //串口发送数据

			//发送数据，同步开关状态
			MQTT_PublishDataQs0("/sys/a1RwXdzy6BZ/D001/thing/event/property/post","{\"params\":{\"Switch1\":1}}");
		}
		

		
		//搜索关键词，如果搜索到进入if
		if(strstr((char *)Aliyun_mqtt.CMD_buff,"/ota/device/upgrade/a1RwXdzy6BZ/D001")){
			//按格式提取关键数据成功，进入if
			if(sscanf((char *)Aliyun_mqtt.CMD_buff,"/ota/device/upgrade/a1RwXdzy6BZ/D001{\"code\":\"1000\",\"data\":{\"size\":%d,\"streamId\":%d,\"sign\":\"%*32s\",\"dProtocol\":\"mqtt\",\"version\":\"%26s\",\"signMethod\":\"Md5\",\"streamFileId\":1,\"md5\":\"%*32s\"},\"id\":%*d,\"message\":\"success\"}",&Aliyun_mqtt.size,&Aliyun_mqtt.streamId,Aliyun_mqtt.OTA_tempver)==3){
				u1_printf("OTA固件大小：%d\r\n",Aliyun_mqtt.size);             //串口发送数据
				u1_printf("OTA固件ID：%d\r\n",Aliyun_mqtt.streamId);           //串口发送数据
				u1_printf("OTA固件版本号：%s\r\n",Aliyun_mqtt.OTA_tempver);    //串口发送数据
				FS4G_Connect_State_Flag |= OTA_EVENT;                                      //置位标志位，OT发生
				W25Q64_Erase64K(0);                                            //清除0号块数据
				if(Aliyun_mqtt.size%256==0){                                   //判断固件大小是否是256的整数倍，是的话进入if
					Aliyun_mqtt.counter = Aliyun_mqtt.size/256;                //每次下载256字节，计算下载次数
				}else{                                                         //判断固件大小是否是256的整数倍，不是的话进入else
					Aliyun_mqtt.counter = Aliyun_mqtt.size/256 + 1;            //每次下载256字节，计算下载次数
				}
				Aliyun_mqtt.num = 1;                                           //当前第1次下载
				Aliyun_mqtt.downlen = 256;                                     //记录本次下载量
				OTA_Download(Aliyun_mqtt.downlen,(Aliyun_mqtt.num - 1)*256);   //发送下载数据
			}else{                                                             //按格式提取关键数据失败，进入else
				u1_printf("提取OTA下载命令错误\r\n");                          //串口发送数据
			}
		}
		//搜索关键词，如果搜索到进入if
		if(strstr((char *)Aliyun_mqtt.CMD_buff,"/sys/a1RwXdzy6BZ/D001/thing/file/download_reply")){
			W25Q64_PageWrite(&data[datalen-Aliyun_mqtt.downlen-2],Aliyun_mqtt.num-1);    //保存本次下载的256字节
			Aliyun_mqtt.num++;                                                           //当前下载次数+1
			OTA_Send_Progress(Aliyun_mqtt.num, Aliyun_mqtt.counter);
			if(Aliyun_mqtt.num<Aliyun_mqtt.counter){                                     //如果小于总下载次数，进入if
				Aliyun_mqtt.downlen = 256;                                               //记录本次下载量
				OTA_Download(Aliyun_mqtt.downlen,(Aliyun_mqtt.num - 1)*256);             //发送下载数据
			}else if(Aliyun_mqtt.num==Aliyun_mqtt.counter){                              //如果等于总下载次数，进入if，说明是最后一次下载
				if(Aliyun_mqtt.size%256==0){                                             //判断固件大小是否是256的整数倍，是的话进入if
					Aliyun_mqtt.downlen = 256;                                           //记录本次下载量
					OTA_Download(Aliyun_mqtt.downlen,(Aliyun_mqtt.num - 1)*256);         //发送下载数据
				}else{                                                                   //判断固件大小是否是256的整数倍，不是的话进入else
					Aliyun_mqtt.downlen = Aliyun_mqtt.size%256;                          //记录本次下载量
					OTA_Download(Aliyun_mqtt.downlen,(Aliyun_mqtt.num - 1)*256);         //发送下载数据	
				}
			}else{                                                                       //如果大于总下载次数，进入else，说明下载完毕
				u1_printf("OTA下载完毕\r\n");                                            //串口发送数据
				memset(OTA_Info.OTA_Version,0,32);                                           //清除版本号缓冲区
				memcpy(OTA_Info.OTA_Version,Aliyun_mqtt.OTA_tempver,26);                     //拷贝版本号数据
				OTA_Info.FirmwareLen[0] = Aliyun_mqtt.size;                                  //记录固件大小
				OTA_Info.OTA_flag = OTA_SET_FLAG;                                        //设置标志位变量
				AT24C02_WriteOTAInfo();                                                   //保存到EEPROM
				NVIC_SystemReset();                                                      //重启回B区更新
			}
		}
	}
	
	
	
	
	
	
}


void FS4G_Connect_TCP(void){
	
	u3_printf("AT+CIPMODE=1\r\n");
	delay_ms(30);
	u3_printf("AT+CIPMODE=1\r\n");
	delay_ms(30);
	u3_printf("AT+CIPRXF=1\r\n");//设置接收的数据末尾是否自动添加回车换行：1不添加	
	delay_ms(30);
	u3_printf("AT^HEARTCONFIG=1,0,60\r\n");	 	//配置心跳的相关参数
	delay_ms(30);
	u3_printf("AT^HEARTBEATHEX=4,C000\r\n"); 	//设置 HEX 编码格式心跳包内容
	delay_ms(30);
	u3_printf("AT&W\r\n"); 	//设置 HEX 编码格式心跳包内容
	delay_ms(30);
	u3_printf("AT+CIPSTART=\"TCP\",a1RwXdzy6BZ.iot-as-mqtt.cn-shanghai.aliyuncs.com,1883\r\n");		//AT+CIPSTART="TCP",test.usr.cn,2317	建立 TCP 连接或注册 UDP 端口号
	delay_ms(500);
	MQTT_ConnectPack();
	delay_ms(30);
	
}


/*-------------------------------------------------*/
/*函数名：上传OTA版本号                            */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void OTA_Version(void)
{
	char temp[128];
	
	memset(temp,0,128);                                                                  //清空缓冲区
	sprintf(temp,"{\"id\": \"1\",\"params\": {\"version\": \"%s\"}}",OTA_Info.OTA_Version);  //构建数据
	
	MQTT_PublishDataQs1("/ota/device/inform/a1RwXdzy6BZ/D001",temp);                     //发送数据到服务器
}
/*-------------------------------------------------*/
/*函数名：OTA下载数据                              */
/*参  数：size：本次下载量                         */
/*参  数：offset：本次下载偏移量                   */
/*返回值：无                                       */
/*-------------------------------------------------*/
void OTA_Download(int size, int offset)
{
	char temp[256];
	
	memset(temp,0,256);                                                       //清空缓冲区
	//构建数据
	sprintf(temp,"{\"id\": \"1\",\"params\": {\"fileInfo\":{\"streamId\":%d,\"fileId\":1},\"fileBlock\":{\"size\":%d,\"offset\":%d}}}",Aliyun_mqtt.streamId,size,offset);
	u1_printf("当前第%d/%d次\r\n",Aliyun_mqtt.num,Aliyun_mqtt.counter);       //串口输出数据
	MQTT_PublishDataQs0("/sys/a1RwXdzy6BZ/D001/thing/file/download",temp);    //发送数据到服务器
	delay_ms(300);                                                            //延时，阿里云限速，不能发太快
}

/*-------------------------------------------------*/
/*函数名：OTA上报进度                              */
/*参  数：num：本次下载的分片标号                  */
/*参  数：counter：总分片数			                   */
/*返回值：无                                       */
/*-------------------------------------------------*/
void OTA_Send_Progress(int num, int counter)
{
	char temp[256];
	int OTA_Percent;
	OTA_Percent = num*100/counter;
	memset(temp,0,256);                                                       //清空缓冲区
	//构建数据
	sprintf(temp,"{\"id\":\"123\",\"params\":{\"step\":\"%d\",\"desc\":\"更新中\"}}",OTA_Percent);
	MQTT_PublishDataQs0("/ota/device/progress/a1RwXdzy6BZ/D001",temp);    //发送数据到服务器
	delay_ms(300);                                                            //延时，阿里云限速，不能发太快
}

