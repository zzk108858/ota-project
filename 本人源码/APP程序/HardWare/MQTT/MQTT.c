#include "MQTT.h"
#include "usart.h"

MQTT_CB  Aliyun_mqtt;

/*-------------------------------------------------*/
/*������������MQTTЭ��CONNECT����                  */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void MQTT_ConnectPack(void)
{
	Aliyun_mqtt.MessageID = 1;                                                                       //��ʼ�����ı�ʶ����������1��ʼ����
	Aliyun_mqtt.Fixed_len = 1;                                                                       //�̶���ͷ���ȣ��ݶ�1
	Aliyun_mqtt.Variable_len = 10;                                                                   //�ɱ䱨ͷ���ȣ�10
	Aliyun_mqtt.Payload_len = 2 + strlen(CLIENTID) + 2 + strlen(USERNAME) + 2 + strlen(PASSWORD);    //���㸺�س���
	Aliyun_mqtt.Remaining_len = Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len;                  //����ʣ�೤��
	
	Aliyun_mqtt.Pack_buff[0] = 0x10;                                                                 //CONNECT���Ĺ̶���ͷ��1���ֽڣ�0x01
	do{
		if(Aliyun_mqtt.Remaining_len/128 == 0){                                                      //�ж�ʣ�೤���Ƿ�128���������������if
			Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len] = Aliyun_mqtt.Remaining_len;                //��¼��ֵ
		}else{                                                                                       //�ж�ʣ�೤���Ƿ�128�������������else����Ҫ��ǰ��һ���ֽ�
			Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len] = (Aliyun_mqtt.Remaining_len%128)|0x80;     //��¼��ֵ������λBIT7����ʾ��Ҫ��ǰ��һ���ֽ�
		}
		Aliyun_mqtt.Fixed_len++;                                                                     //�̶���ͷ����+1
		Aliyun_mqtt.Remaining_len = Aliyun_mqtt.Remaining_len/128;                                   //ȡ��128
	}while(Aliyun_mqtt.Remaining_len);                                                               //�������0������һ��ѭ���������0���˳�
	
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+0] = 0x00;    //CONNECT���Ŀɱ䱨ͷ��0x00
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+1] = 0x04;    //CONNECT���Ŀɱ䱨ͷ��0x04
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+2] = 0x4D;    //CONNECT���Ŀɱ䱨ͷ��0x4D
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+3] = 0x51;    //CONNECT���Ŀɱ䱨ͷ��0x51
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+4] = 0x54;    //CONNECT���Ŀɱ䱨ͷ��0x54
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+5] = 0x54;    //CONNECT���Ŀɱ䱨ͷ��0x54
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+6] = 0x04;    //CONNECT���Ŀɱ䱨ͷ��0x04
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+7] = 0xC2;    //CONNECT���Ŀɱ䱨ͷ��0xC2
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+8] = 0x00;    //CONNECT���Ŀɱ䱨ͷ��0x00
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+9] = 0x64;    //CONNECT���Ŀɱ䱨ͷ��0x64
	
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+10] = strlen(CLIENTID)/256;                 //���أ�CLIENTID�ַ������ȱ�ʾ���ֽ�
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+11] = strlen(CLIENTID)%256;                 //���أ�CLIENTID�ַ������ȱ�ʾ���ֽ�
	memcpy(&Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+12],CLIENTID,strlen(CLIENTID));     //���أ�����CLIENTID�ַ���
	
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+12+strlen(CLIENTID)] = strlen(USERNAME)/256;              //���أ�USERNAME�ַ������ȱ�ʾ���ֽ�
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+13+strlen(CLIENTID)] = strlen(USERNAME)%256;              //���أ�USERNAME�ַ������ȱ�ʾ���ֽ�
	memcpy(&Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+14+strlen(CLIENTID)],USERNAME,strlen(USERNAME));  //���أ�����USERNAME�ַ���
	
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+14+strlen(CLIENTID)+strlen(USERNAME)] = strlen(PASSWORD)/256;              //���أ�PASSWORD�ַ������ȱ�ʾ���ֽ�
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+15+strlen(CLIENTID)+strlen(USERNAME)] = strlen(PASSWORD)%256;              //���أ�PASSWORD�ַ������ȱ�ʾ���ֽ�
	memcpy(&Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+16+strlen(CLIENTID)+strlen(USERNAME)],PASSWORD,strlen(PASSWORD));  //���أ�����PASSWORD�ַ���
	
//	memcpy(&Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len + Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len],"\r\n",2);
	
	//u3_printf("+++");
	//u3_printf("AT+CIPSEND=175\r\n");
	u3_sdata(Aliyun_mqtt.Pack_buff,Aliyun_mqtt.Fixed_len + Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len);            //CONNECT�������ݼ��뷢�ͻ�����
	
}

/*-------------------------------------------------*/
/*������������MQTTЭ��Subcrib����                  */
/*��  ����topic����Ҫ���ĵ�����Topic               */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void MQTT_SubcribPack(char *topic)
{
	Aliyun_mqtt.Fixed_len = 1;                                                             //�̶���ͷ���ȣ��ݶ�1
	Aliyun_mqtt.Variable_len = 2;                                                          //�ɱ䱨ͷ���ȣ�2
	Aliyun_mqtt.Payload_len = 2 + strlen(topic) + 1;                                       //���㸺�س���
	Aliyun_mqtt.Remaining_len = Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len;        //����ʣ�೤��
	
	Aliyun_mqtt.Pack_buff[0] = 0x82;                                                                 //Subcrib���Ĺ̶���ͷ��1���ֽڣ�0x82
	do{                                                                                  
		if(Aliyun_mqtt.Remaining_len/128 == 0){                                                      //�ж�ʣ�೤���Ƿ�128���������������if
			Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len] = Aliyun_mqtt.Remaining_len;                //��¼��ֵ
		}else{                                                                                       //�ж�ʣ�೤���Ƿ�128�������������else����Ҫ��ǰ��һ���ֽ�
			Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len] = (Aliyun_mqtt.Remaining_len%128)|0x80;     //��¼��ֵ������λBIT7����ʾ��Ҫ��ǰ��һ���ֽ�
		}
		Aliyun_mqtt.Fixed_len++;                                                                     //�̶���ͷ����+1
		Aliyun_mqtt.Remaining_len = Aliyun_mqtt.Remaining_len/128;                                   //ȡ��128
	}while(Aliyun_mqtt.Remaining_len);                                                               //�������0������һ��ѭ���������0���˳�
	
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+0] = Aliyun_mqtt.MessageID/256;     //�ɱ䱨ͷ�����ı�ʶ�����ֽ�
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+1] = Aliyun_mqtt.MessageID%256;     //�ɱ䱨ͷ�����ı�ʶ�����ֽ�
	Aliyun_mqtt.MessageID++;                                                        //���ı�ʶ������+1

	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+2] = strlen(topic)/256;	            //���أ����ĵ�Topic�ַ������ȱ�ʾ���ֽ�
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+3] = strlen(topic)%256;	            //���أ����ĵ�Topic�ַ������ȱ�ʾ���ֽ�
	memcpy(&Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+4],topic,strlen(topic));    //���أ��������ĵ�Topic�ַ���
	
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+4+strlen(topic)] = 0;               //���أ����ĵ�Topic���������ȼ�
	//u3_printf("AT+CIPSEND=55\r\n");
	u3_sdata(Aliyun_mqtt.Pack_buff,Aliyun_mqtt.Fixed_len + Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len);//Subcrib�������ݼ��뷢�ͻ�����
}


/*-------------------------------------------------*/
/*��������������������͵�Publish����              */
/*��  ����data������                               */
/*��  ����data_len�����ݳ���                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void MQTT_DealPublishData(uint8_t *data, uint16_t data_len)
{
	uint8_t i;
	
	for(i=1;i<5;i++){            //���ѭ��4�Σ��ж�ʣ�೤��ռ�ü����ֽ�
		if((data[i]&0x80)==0)    //���BIT7����1������if
			break;               //�˳�forѭ������ʱi��ֵ����ʣ�೤��ռ�õ��ֽ���
	}
	
	memset(Aliyun_mqtt.CMD_buff,0,512);                          //��ջ�����
	memcpy(Aliyun_mqtt.CMD_buff,&data[1+i+2],data_len-1-i-2);    //�������ݵ�������
}
/*-------------------------------------------------*/
/*������������������͵ȼ�0��Publish����           */
/*��  ����topic����Ҫ�������ݵ�Topic               */
/*��  ����data������                               */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void MQTT_PublishDataQs0(char *topic, char *data)
{
	Aliyun_mqtt.Fixed_len = 1;                                                           //�̶���ͷ���ȣ��ݶ�1
	Aliyun_mqtt.Variable_len = 2 + strlen(topic);                                        //����ɱ䱨ͷ����
	Aliyun_mqtt.Payload_len = strlen(data);                                              //���㸺�س���
	Aliyun_mqtt.Remaining_len = Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len;      //����ʣ�೤�ȳ���
	
	Aliyun_mqtt.Pack_buff[0] = 0x30;                                                                 //�ȼ�0��Publish���Ĺ̶���ͷ��1���ֽڣ�0x30
	do{                                                                                  
		if(Aliyun_mqtt.Remaining_len/128 == 0){                                                      //�ж�ʣ�೤���Ƿ�128���������������if
			Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len] = Aliyun_mqtt.Remaining_len;                //��¼��ֵ
		}else{                                                                                       //�ж�ʣ�೤���Ƿ�128�������������else����Ҫ��ǰ��һ���ֽ�
			Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len] = (Aliyun_mqtt.Remaining_len%128)|0x80;     //��¼��ֵ������λBIT7����ʾ��Ҫ��ǰ��һ���ֽ�
		}
		Aliyun_mqtt.Fixed_len++;                                                                     //�̶���ͷ����+1
		Aliyun_mqtt.Remaining_len = Aliyun_mqtt.Remaining_len/128;                                   //ȡ��128
	}while(Aliyun_mqtt.Remaining_len);                                                               //�������0������һ��ѭ���������0���˳�	
	
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+0] = strlen(topic)/256;                              //�ɱ䱨ͷ���������ݵ�Topic�ַ������ȱ�ʾ���ֽ�    
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+1] = strlen(topic)%256;                              //�ɱ䱨ͷ���������ݵ�Topic�ַ������ȱ�ʾ���ֽ�
	memcpy(&Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+2],topic,strlen(topic));                     //�ɱ䱨ͷ�������������ݵ�Topic�ַ���
	memcpy(&Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+2+strlen(topic)],data,strlen(data));         //���أ��������͵�����
	
	u3_sdata(Aliyun_mqtt.Pack_buff,Aliyun_mqtt.Fixed_len + Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len);	 //Publish�������ݼ��뷢�ͻ�����
}
/*-------------------------------------------------*/
/*������������������͵ȼ�1��Publish����           */
/*��  ����topic����Ҫ�������ݵ�Topic               */
/*��  ����data������                               */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void MQTT_PublishDataQs1(char *topic, char *data)
{
	Aliyun_mqtt.Fixed_len = 1;                                                           //�̶���ͷ���ȣ��ݶ�1
	Aliyun_mqtt.Variable_len = 2 + 2 + strlen(topic);                                    //����ɱ䱨ͷ����
	Aliyun_mqtt.Payload_len = strlen(data);                                              //���㸺�س���
	Aliyun_mqtt.Remaining_len = Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len;      //����ʣ�೤�ȳ���
	
	Aliyun_mqtt.Pack_buff[0] = 0x32;                                                                 //�ȼ�1��Publish���Ĺ̶���ͷ��1���ֽڣ�0x32
	do{                                                                                  
		if(Aliyun_mqtt.Remaining_len/128 == 0){                                                      //�ж�ʣ�೤���Ƿ�128���������������if
			Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len] = Aliyun_mqtt.Remaining_len;                //��¼��ֵ
		}else{                                                                                       //�ж�ʣ�೤���Ƿ�128�������������else����Ҫ��ǰ��һ���ֽ�
			Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len] = (Aliyun_mqtt.Remaining_len%128)|0x80;     //��¼��ֵ������λBIT7����ʾ��Ҫ��ǰ��һ���ֽ�
		}
		Aliyun_mqtt.Fixed_len++;                                                                     //�̶���ͷ����+1
		Aliyun_mqtt.Remaining_len = Aliyun_mqtt.Remaining_len/128;                                   //ȡ��128
	}while(Aliyun_mqtt.Remaining_len);                                                               //�������0������һ��ѭ���������0���˳�	
	
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+0] = strlen(topic)/256;                              //�ɱ䱨ͷ���������ݵ�Topic�ַ������ȱ�ʾ���ֽ�
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+1] = strlen(topic)%256;                              //�ɱ䱨ͷ���������ݵ�Topic�ַ������ȱ�ʾ���ֽ�
	memcpy(&Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+2],topic,strlen(topic));                     //�ɱ䱨ͷ�������������ݵ�Topic�ַ���
	
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+2+strlen(topic)] = Aliyun_mqtt.MessageID/256;        //�ɱ䱨ͷ�����ı�ʶ�����ֽ�
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+3+strlen(topic)] = Aliyun_mqtt.MessageID%256;        //�ɱ䱨ͷ�����ı�ʶ�����ֽ�
	Aliyun_mqtt.MessageID++;                                                                         //���ı�ʶ������+1
	
	memcpy(&Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+4+strlen(topic)],data,strlen(data));         //���أ��������͵�����
	
	u3_sdata(Aliyun_mqtt.Pack_buff,Aliyun_mqtt.Fixed_len + Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len);	//Publish�������ݼ��뷢�ͻ�����
}


