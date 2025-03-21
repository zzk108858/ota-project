#include "gd32f10x.h"
#include "main.h"
#include "4g.h"
#include "delay.h"
#include "usart.h"
#include "w25q64.h"
#include "m24c02.h"
#include "mqtt.h"

/*-------------------------------------------------*/
/*函数名：复位4G模块                               */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void GM4G_Init(void)
{
	rcu_periph_clock_enable(RCU_GPIOB);	                                 //打开GPIOB时钟
	gpio_init(GPIOB,GPIO_MODE_IPD,GPIO_OSPEED_50MHZ,GPIO_PIN_0);         //设置PB0，监控连接A的状态
	gpio_init(GPIOB,GPIO_MODE_OUT_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_2);      //设置PB2，控制4G模块的复位
	
	gpio_init(GPIOB,GPIO_MODE_OUT_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_12);     //设置PB12，控制LED
	gpio_init(GPIOB,GPIO_MODE_OUT_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_13);     //设置PB13，控制LED
	gpio_init(GPIOB,GPIO_MODE_OUT_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_14);     //设置PB14，控制LED
	gpio_init(GPIOB,GPIO_MODE_OUT_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_15);     //设置PB15，控制LED
	gpio_bit_set(GPIOB,GPIO_PIN_12);                                     //设置高电平，默认熄灭LED
	gpio_bit_set(GPIOB,GPIO_PIN_13);                                     //设置高电平，默认熄灭LED
	gpio_bit_set(GPIOB,GPIO_PIN_14);                                     //设置高电平，默认熄灭LED
	gpio_bit_set(GPIOB,GPIO_PIN_15);                                     //设置高电平，默认熄灭LED
	
	//同步复位4G模块
	u0_printf("复位4G模块，请等待... ...");
	gpio_bit_set(GPIOB,GPIO_PIN_2);
	Delay_Ms(500);
	gpio_bit_reset(GPIOB,GPIO_PIN_2);
}
/*-------------------------------------------------*/
/*函数名：处理串口2的数据                          */
/*参  数：data：数据指针      datalen：数据长度    */
/*返回值：无                                       */
/*-------------------------------------------------*/
void U2_Event(uint8_t *data, uint16_t datalen)
{	
	if((datalen == 6)&&(memcmp(data,"chaozi",6) == 0)){                     //如果接收到6个字节 且是 chaozi，进入if
		u0_printf("\r\n4G模块复位成功,等待连接服务器成功... ...\r\n");	    //串口发送数据
		rcu_periph_clock_enable(RCU_AF);                                    //开启RCU_AF                              
		exti_deinit();                                                      //复位外部中断
		exti_init(EXTI_0,EXTI_INTERRUPT,EXTI_TRIG_BOTH);                    //初始化线0外部中断，上下沿触发
		gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOB,GPIO_PIN_SOURCE_0);  //选择PB0
		exti_interrupt_enable(EXTI_0);                                      //使能线0外部中断
		nvic_irq_enable(EXTI0_IRQn,0,0);                                    //是能中断设置优先级               	
	}
	
	if((datalen == 4)&&(data[0]==0x20)){                                           //如果接收到4个字节 且是 第一个字节是0x20，进入if
		u0_printf("收到CONNACK报文\r\n");	                                       //串口发送数据
		if(data[3]==0x00){                                                         //判断第4个字节，如果是0x00，进入if
			u0_printf("CONNECT报文成功连接服务器\r\n");                            //串口发送数据
			BootStaFlag |= CONNECT_OK;                                             //设置标志位，表示CONNECT报文成功
			MQTT_SubcribPack("/sys/a1HvxPcHnkX/D001/thing/service/property/set");  //发送订阅Topic报文
			MQTT_SubcribPack("/sys/a1HvxPcHnkX/D001/thing/file/download_reply");   //发送订阅Topic报文
			OTA_Version();                                                         //上传当前版本号
		}else{                                                                     //判断第4个字节，如果不是0x00，进入else
			u0_printf("CONNECT报文错误，准备重启\r\n");                            //串口发送数据
			NVIC_SystemReset();                                                    //重启
		}                   	
	}
	
	if((datalen == 5)&&(data[0]==0x90)){                          //如果接收到5个字节 且是 第一个字节是0x90，进入if
		u0_printf("收到SUBACK报文\r\n");	                      //串口发送数据
		if((data[datalen-1]==0x00)||(data[datalen-1]==0x01)){     //判断接收的最后一个字节，如果是0x00或则0x01.进入if
			u0_printf("SUBCRIBE订阅报文成功\r\n");                //串口发送数据
		}else{													  //判断接收的最后一个字节，如果不是0x00或则0x01.进入else									
			u0_printf("SUBCRIBE订阅报文错误，准备重启\r\n");      //串口发送数据
			NVIC_SystemReset();                                   //重启
		}                   	
	}
	
	if((BootStaFlag&CONNECT_OK)&&(data[0]==0x30)){                      //如果CONNECT报文成功 且是 第一个字节是0x30，进入if
		u0_printf("收到等级0的PUBLISH报文\r\n");                        //串口发送数据	
		MQTT_DealPublishData(data,datalen);                             //提取数据
		u0_printf("%s\r\n",Aliyun_mqtt.CMD_buff);                       //串口输出提取的数据
		if(strstr((char *)Aliyun_mqtt.CMD_buff,"{\"Switch1\":0}")){     //搜索关键词，如果搜索到进入if
			u0_printf("关闭开关1\r\n");                                 //串口发送数据
			gpio_bit_set(GPIOB,GPIO_PIN_12);                            //高电平，熄灭LED
			//发送数据，同步开关状态
			MQTT_PublishDataQs0("/sys/a1HvxPcHnkX/D001/thing/event/property/post","{\"params\":{\"Switch1\":0}}");
		}
		if(strstr((char *)Aliyun_mqtt.CMD_buff,"{\"Switch1\":1}")){     //搜索关键词，如果搜索到进入if
			u0_printf("打开开关1\r\n");                                 //串口发送数据
			gpio_bit_reset(GPIOB,GPIO_PIN_12);                          //低电平，点亮LED
			//发送数据，同步开关状态
			MQTT_PublishDataQs0("/sys/a1HvxPcHnkX/D001/thing/event/property/post","{\"params\":{\"Switch1\":1}}");
		}
		
		if(strstr((char *)Aliyun_mqtt.CMD_buff,"{\"Switch2\":0}")){     //搜索关键词，如果搜索到进入if
			u0_printf("关闭开关2\r\n");                                 //串口发送数据
			gpio_bit_set(GPIOB,GPIO_PIN_13);                            //高电平，熄灭LED
			//发送数据，同步开关状态
			MQTT_PublishDataQs0("/sys/a1HvxPcHnkX/D001/thing/event/property/post","{\"params\":{\"Switch2\":0}}");
		}
		if(strstr((char *)Aliyun_mqtt.CMD_buff,"{\"Switch2\":1}")){     //搜索关键词，如果搜索到进入if
			u0_printf("打开开关2\r\n");                                 //串口发送数据
			gpio_bit_reset(GPIOB,GPIO_PIN_13);                          //低电平，点亮LED
			//发送数据，同步开关状态
			MQTT_PublishDataQs0("/sys/a1HvxPcHnkX/D001/thing/event/property/post","{\"params\":{\"Switch2\":1}}");
		}
		
		if(strstr((char *)Aliyun_mqtt.CMD_buff,"{\"Switch3\":0}")){     //搜索关键词，如果搜索到进入if
			u0_printf("关闭开关3\r\n");                                 //串口发送数据
			gpio_bit_set(GPIOB,GPIO_PIN_14);                            //高电平，熄灭LED
			//发送数据，同步开关状态
			MQTT_PublishDataQs0("/sys/a1HvxPcHnkX/D001/thing/event/property/post","{\"params\":{\"Switch3\":0}}");
		}
		if(strstr((char *)Aliyun_mqtt.CMD_buff,"{\"Switch3\":1}")){     //搜索关键词，如果搜索到进入if
			u0_printf("打开开关3\r\n");                                 //串口发送数据
			gpio_bit_reset(GPIOB,GPIO_PIN_14);                          //低电平，点亮LED
			//发送数据，同步开关状态
			MQTT_PublishDataQs0("/sys/a1HvxPcHnkX/D001/thing/event/property/post","{\"params\":{\"Switch3\":1}}");
		}
		
		if(strstr((char *)Aliyun_mqtt.CMD_buff,"{\"Switch4\":0}")){     //搜索关键词，如果搜索到进入if
			u0_printf("关闭开关4\r\n");                                 //串口发送数据
			gpio_bit_set(GPIOB,GPIO_PIN_15);                            //高电平，熄灭LED
			//发送数据，同步开关状态
			MQTT_PublishDataQs0("/sys/a1HvxPcHnkX/D001/thing/event/property/post","{\"params\":{\"Switch4\":0}}");
		}
		if(strstr((char *)Aliyun_mqtt.CMD_buff,"{\"Switch4\":1}")){     //搜索关键词，如果搜索到进入if
			u0_printf("打开开关4\r\n");                                 //串口发送数据
			gpio_bit_reset(GPIOB,GPIO_PIN_15);                          //低电平，点亮LED
			//发送数据，同步开关状态
			MQTT_PublishDataQs0("/sys/a1HvxPcHnkX/D001/thing/event/property/post","{\"params\":{\"Switch4\":1}}");
		}
		
		//搜索关键词，如果搜索到进入if
		if(strstr((char *)Aliyun_mqtt.CMD_buff,"/ota/device/upgrade/a1HvxPcHnkX/D001")){
			//按格式提取关键数据成功，进入if
			if(sscanf((char *)Aliyun_mqtt.CMD_buff,"/ota/device/upgrade/a1HvxPcHnkX/D001{\"code\":\"1000\",\"data\":{\"size\":%d,\"streamId\":%d,\"sign\":\"%*32s\",\"dProtocol\":\"mqtt\",\"version\":\"%26s\",\"signMethod\":\"Md5\",\"streamFileId\":1,\"md5\":\"%*32s\"},\"id\":%*d,\"message\":\"success\"}",&Aliyun_mqtt.size,&Aliyun_mqtt.streamId,Aliyun_mqtt.OTA_tempver)==3){
				u0_printf("OTA固件大小：%d\r\n",Aliyun_mqtt.size);             //串口发送数据
				u0_printf("OTA固件ID：%d\r\n",Aliyun_mqtt.streamId);           //串口发送数据
				u0_printf("OTA固件版本号：%s\r\n",Aliyun_mqtt.OTA_tempver);    //串口发送数据
				BootStaFlag |= OTA_EVENT;                                      //置位标志位，OT发生
				W25Q64_Erase64K(0);                                            //清除0号块数据
				if(Aliyun_mqtt.size%256==0){                                   //判断固件大小是否是256的整数倍，是的话进入if
					Aliyun_mqtt.counter = Aliyun_mqtt.size/256;                  //每次下载256字节，计算下载次数
				}else{                                                         //判断固件大小是否是256的整数倍，不是的话进入else
					Aliyun_mqtt.counter = Aliyun_mqtt.size/256 + 1;            //每次下载256字节，计算下载次数
				}
				Aliyun_mqtt.num = 1;                                           //当前第1次下载
				Aliyun_mqtt.downlen = 256;                                     //记录本次下载量
				OTA_Download(Aliyun_mqtt.downlen,(Aliyun_mqtt.num - 1)*256);   //发送下载数据
			}else{                                                             //按格式提取关键数据失败，进入else
				u0_printf("提取OTA下载命令错误\r\n");                          //串口发送数据
			}
		}
		//搜索关键词，如果搜索到进入if
		if(strstr((char *)Aliyun_mqtt.CMD_buff,"/sys/a1HvxPcHnkX/D001/thing/file/download_reply")){
			W25Q64_PageWrite(&data[datalen-Aliyun_mqtt.downlen-2],Aliyun_mqtt.num-1);    //保存本次下载的256字节
			Aliyun_mqtt.num++;                                                           //当前下载次数+1
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
				u0_printf("OTA下载完毕\r\n");                                            //串口发送数据
				memset(OTA_Info.OTA_ver,0,32);                                           //清除版本号缓冲区
				memcpy(OTA_Info.OTA_ver,Aliyun_mqtt.OTA_tempver,26);                     //拷贝版本号数据
				OTA_Info.Firelen[0] = Aliyun_mqtt.size;                                  //记录固件大小
				OTA_Info.OTA_flag = OTA_SET_FLAG;                                        //设置标志位变量
				M24C02_WriteOTAInfo();                                                   //保存到EEPROM
				NVIC_SystemReset();                                                      //重启回B区更新
			}
		}
	}
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
	sprintf(temp,"{\"id\": \"1\",\"params\": {\"version\": \"%s\"}}",OTA_Info.OTA_ver);  //构建数据
	
	MQTT_PublishDataQs1("/ota/device/inform/a1HvxPcHnkX/D001",temp);                     //发送数据到服务器
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
	u0_printf("当前第%d/%d次\r\n",Aliyun_mqtt.num,Aliyun_mqtt.counter);       //串口输出数据
	MQTT_PublishDataQs0("/sys/a1HvxPcHnkX/D001/thing/file/download",temp);    //发送数据到服务器
	Delay_Ms(300);                                                            //延时，阿里云限速，不能发太快
}
