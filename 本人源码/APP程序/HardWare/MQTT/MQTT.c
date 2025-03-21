#include "MQTT.h"
#include "usart.h"

MQTT_CB  Aliyun_mqtt;

/*-------------------------------------------------*/
/*函数名：构建MQTT协议CONNECT报文                  */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void MQTT_ConnectPack(void)
{
	Aliyun_mqtt.MessageID = 1;                                                                       //初始化报文标识符变量，从1开始利用
	Aliyun_mqtt.Fixed_len = 1;                                                                       //固定报头长度，暂定1
	Aliyun_mqtt.Variable_len = 10;                                                                   //可变报头长度，10
	Aliyun_mqtt.Payload_len = 2 + strlen(CLIENTID) + 2 + strlen(USERNAME) + 2 + strlen(PASSWORD);    //计算负载长度
	Aliyun_mqtt.Remaining_len = Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len;                  //计算剩余长度
	
	Aliyun_mqtt.Pack_buff[0] = 0x10;                                                                 //CONNECT报文固定报头第1个字节，0x01
	do{
		if(Aliyun_mqtt.Remaining_len/128 == 0){                                                      //判断剩余长度是否够128，如果不够，进入if
			Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len] = Aliyun_mqtt.Remaining_len;                //记录数值
		}else{                                                                                       //判断剩余长度是否够128，如果够，进入else，需要向前进一个字节
			Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len] = (Aliyun_mqtt.Remaining_len%128)|0x80;     //记录数值，并置位BIT7，表示需要向前进一个字节
		}
		Aliyun_mqtt.Fixed_len++;                                                                     //固定报头长度+1
		Aliyun_mqtt.Remaining_len = Aliyun_mqtt.Remaining_len/128;                                   //取整128
	}while(Aliyun_mqtt.Remaining_len);                                                               //如果不是0，则再一次循环，如果是0就退出
	
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+0] = 0x00;    //CONNECT报文可变报头：0x00
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+1] = 0x04;    //CONNECT报文可变报头：0x04
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+2] = 0x4D;    //CONNECT报文可变报头：0x4D
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+3] = 0x51;    //CONNECT报文可变报头：0x51
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+4] = 0x54;    //CONNECT报文可变报头：0x54
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+5] = 0x54;    //CONNECT报文可变报头：0x54
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+6] = 0x04;    //CONNECT报文可变报头：0x04
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+7] = 0xC2;    //CONNECT报文可变报头：0xC2
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+8] = 0x00;    //CONNECT报文可变报头：0x00
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+9] = 0x64;    //CONNECT报文可变报头：0x64
	
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+10] = strlen(CLIENTID)/256;                 //负载，CLIENTID字符串长度表示高字节
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+11] = strlen(CLIENTID)%256;                 //负载，CLIENTID字符串长度表示低字节
	memcpy(&Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+12],CLIENTID,strlen(CLIENTID));     //负载，拷贝CLIENTID字符串
	
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+12+strlen(CLIENTID)] = strlen(USERNAME)/256;              //负载，USERNAME字符串长度表示高字节
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+13+strlen(CLIENTID)] = strlen(USERNAME)%256;              //负载，USERNAME字符串长度表示低字节
	memcpy(&Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+14+strlen(CLIENTID)],USERNAME,strlen(USERNAME));  //负载，拷贝USERNAME字符串
	
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+14+strlen(CLIENTID)+strlen(USERNAME)] = strlen(PASSWORD)/256;              //负载，PASSWORD字符串长度表示高字节
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+15+strlen(CLIENTID)+strlen(USERNAME)] = strlen(PASSWORD)%256;              //负载，PASSWORD字符串长度表示低字节
	memcpy(&Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+16+strlen(CLIENTID)+strlen(USERNAME)],PASSWORD,strlen(PASSWORD));  //负载，拷贝PASSWORD字符串
	
//	memcpy(&Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len + Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len],"\r\n",2);
	
	//u3_printf("+++");
	//u3_printf("AT+CIPSEND=175\r\n");
	u3_sdata(Aliyun_mqtt.Pack_buff,Aliyun_mqtt.Fixed_len + Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len);            //CONNECT报文数据加入发送缓冲区
	
}

/*-------------------------------------------------*/
/*函数名：构建MQTT协议Subcrib报文                  */
/*参  数：topic：需要订阅的主题Topic               */
/*返回值：无                                       */
/*-------------------------------------------------*/
void MQTT_SubcribPack(char *topic)
{
	Aliyun_mqtt.Fixed_len = 1;                                                             //固定报头长度，暂定1
	Aliyun_mqtt.Variable_len = 2;                                                          //可变报头长度，2
	Aliyun_mqtt.Payload_len = 2 + strlen(topic) + 1;                                       //计算负载长度
	Aliyun_mqtt.Remaining_len = Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len;        //计算剩余长度
	
	Aliyun_mqtt.Pack_buff[0] = 0x82;                                                                 //Subcrib报文固定报头第1个字节，0x82
	do{                                                                                  
		if(Aliyun_mqtt.Remaining_len/128 == 0){                                                      //判断剩余长度是否够128，如果不够，进入if
			Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len] = Aliyun_mqtt.Remaining_len;                //记录数值
		}else{                                                                                       //判断剩余长度是否够128，如果够，进入else，需要向前进一个字节
			Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len] = (Aliyun_mqtt.Remaining_len%128)|0x80;     //记录数值，并置位BIT7，表示需要向前进一个字节
		}
		Aliyun_mqtt.Fixed_len++;                                                                     //固定报头长度+1
		Aliyun_mqtt.Remaining_len = Aliyun_mqtt.Remaining_len/128;                                   //取整128
	}while(Aliyun_mqtt.Remaining_len);                                                               //如果不是0，则再一次循环，如果是0就退出
	
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+0] = Aliyun_mqtt.MessageID/256;     //可变报头，报文标识符高字节
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+1] = Aliyun_mqtt.MessageID%256;     //可变报头，报文标识符低字节
	Aliyun_mqtt.MessageID++;                                                        //报文标识符变量+1

	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+2] = strlen(topic)/256;	            //负载，订阅的Topic字符串长度表示高字节
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+3] = strlen(topic)%256;	            //负载，订阅的Topic字符串长度表示低字节
	memcpy(&Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+4],topic,strlen(topic));    //负载，拷贝订阅的Topic字符串
	
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+4+strlen(topic)] = 0;               //负载，订阅的Topic服务质量等级
	//u3_printf("AT+CIPSEND=55\r\n");
	u3_sdata(Aliyun_mqtt.Pack_buff,Aliyun_mqtt.Fixed_len + Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len);//Subcrib报文数据加入发送缓冲区
}


/*-------------------------------------------------*/
/*函数名：处理服务器推送的Publish报文              */
/*参  数：data：数据                               */
/*参  数：data_len：数据长度                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void MQTT_DealPublishData(uint8_t *data, uint16_t data_len)
{
	uint8_t i;
	
	for(i=1;i<5;i++){            //最多循环4次，判断剩余长度占用几个字节
		if((data[i]&0x80)==0)    //如果BIT7不是1，进入if
			break;               //退出for循环，此时i的值就是剩余长度占用的字节数
	}
	
	memset(Aliyun_mqtt.CMD_buff,0,512);                          //清空缓冲区
	memcpy(Aliyun_mqtt.CMD_buff,&data[1+i+2],data_len-1-i-2);    //拷贝数据到缓冲区
}
/*-------------------------------------------------*/
/*函数名：向服务器发送等级0的Publish报文           */
/*参  数：topic：需要发送数据的Topic               */
/*参  数：data：数据                               */
/*返回值：无                                       */
/*-------------------------------------------------*/
void MQTT_PublishDataQs0(char *topic, char *data)
{
	Aliyun_mqtt.Fixed_len = 1;                                                           //固定报头长度，暂定1
	Aliyun_mqtt.Variable_len = 2 + strlen(topic);                                        //计算可变报头长度
	Aliyun_mqtt.Payload_len = strlen(data);                                              //计算负载长度
	Aliyun_mqtt.Remaining_len = Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len;      //计算剩余长度长度
	
	Aliyun_mqtt.Pack_buff[0] = 0x30;                                                                 //等级0的Publish报文固定报头第1个字节，0x30
	do{                                                                                  
		if(Aliyun_mqtt.Remaining_len/128 == 0){                                                      //判断剩余长度是否够128，如果不够，进入if
			Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len] = Aliyun_mqtt.Remaining_len;                //记录数值
		}else{                                                                                       //判断剩余长度是否够128，如果够，进入else，需要向前进一个字节
			Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len] = (Aliyun_mqtt.Remaining_len%128)|0x80;     //记录数值，并置位BIT7，表示需要向前进一个字节
		}
		Aliyun_mqtt.Fixed_len++;                                                                     //固定报头长度+1
		Aliyun_mqtt.Remaining_len = Aliyun_mqtt.Remaining_len/128;                                   //取整128
	}while(Aliyun_mqtt.Remaining_len);                                                               //如果不是0，则再一次循环，如果是0就退出	
	
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+0] = strlen(topic)/256;                              //可变报头，发送数据的Topic字符串长度表示高字节    
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+1] = strlen(topic)%256;                              //可变报头，发送数据的Topic字符串长度表示低字节
	memcpy(&Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+2],topic,strlen(topic));                     //可变报头，拷贝发送数据的Topic字符串
	memcpy(&Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+2+strlen(topic)],data,strlen(data));         //负载，拷贝发送的数据
	
	u3_sdata(Aliyun_mqtt.Pack_buff,Aliyun_mqtt.Fixed_len + Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len);	 //Publish报文数据加入发送缓冲区
}
/*-------------------------------------------------*/
/*函数名：向服务器发送等级1的Publish报文           */
/*参  数：topic：需要发送数据的Topic               */
/*参  数：data：数据                               */
/*返回值：无                                       */
/*-------------------------------------------------*/
void MQTT_PublishDataQs1(char *topic, char *data)
{
	Aliyun_mqtt.Fixed_len = 1;                                                           //固定报头长度，暂定1
	Aliyun_mqtt.Variable_len = 2 + 2 + strlen(topic);                                    //计算可变报头长度
	Aliyun_mqtt.Payload_len = strlen(data);                                              //计算负载长度
	Aliyun_mqtt.Remaining_len = Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len;      //计算剩余长度长度
	
	Aliyun_mqtt.Pack_buff[0] = 0x32;                                                                 //等级1的Publish报文固定报头第1个字节，0x32
	do{                                                                                  
		if(Aliyun_mqtt.Remaining_len/128 == 0){                                                      //判断剩余长度是否够128，如果不够，进入if
			Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len] = Aliyun_mqtt.Remaining_len;                //记录数值
		}else{                                                                                       //判断剩余长度是否够128，如果够，进入else，需要向前进一个字节
			Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len] = (Aliyun_mqtt.Remaining_len%128)|0x80;     //记录数值，并置位BIT7，表示需要向前进一个字节
		}
		Aliyun_mqtt.Fixed_len++;                                                                     //固定报头长度+1
		Aliyun_mqtt.Remaining_len = Aliyun_mqtt.Remaining_len/128;                                   //取整128
	}while(Aliyun_mqtt.Remaining_len);                                                               //如果不是0，则再一次循环，如果是0就退出	
	
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+0] = strlen(topic)/256;                              //可变报头，发送数据的Topic字符串长度表示高字节
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+1] = strlen(topic)%256;                              //可变报头，发送数据的Topic字符串长度表示低字节
	memcpy(&Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+2],topic,strlen(topic));                     //可变报头，拷贝发送数据的Topic字符串
	
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+2+strlen(topic)] = Aliyun_mqtt.MessageID/256;        //可变报头，报文标识符高字节
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+3+strlen(topic)] = Aliyun_mqtt.MessageID%256;        //可变报头，报文标识符低字节
	Aliyun_mqtt.MessageID++;                                                                         //报文标识符变量+1
	
	memcpy(&Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+4+strlen(topic)],data,strlen(data));         //负载，拷贝发送的数据
	
	u3_sdata(Aliyun_mqtt.Pack_buff,Aliyun_mqtt.Fixed_len + Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len);	//Publish报文数据加入发送缓冲区
}


