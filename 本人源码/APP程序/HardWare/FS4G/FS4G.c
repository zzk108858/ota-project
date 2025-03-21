#include "FS4G.h"
#include "MQTT.h"
#include "W25Q64.h"
#include "AT24C02.h"
void FS4G_Init(void)
{
	GPIO_InitTypeDef GPIO_Config;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//��IO��ʱ��
	
	//RDY			���4Gģ������״̬   �����ɹ����ɵ͵�ƽתΪ�ߵ�ƽ   GPIOB.0
	GPIO_Config.GPIO_Mode		=	GPIO_Mode_IPD;
	GPIO_Config.GPIO_Pin		=	GPIO_Pin_0;
	GPIO_Config.GPIO_Speed	=	GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_Config);							
	
	//RST			��λ����	  GPIOB.1��ʼ��
	GPIO_Config.GPIO_Mode		=	GPIO_Mode_Out_PP;
	GPIO_Config.GPIO_Pin		=	GPIO_Pin_1;
	GPIO_Config.GPIO_Speed	=	GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_Config);							
	
	u1_printf("��ʼ��λ4Gģ�飬���������\r\n");
	GPIO_SetBits(GPIOB, GPIO_Pin_1);//��λ���ţ� ���� 150ms ����ģ�鸴λ
	delay_ms(500);
	GPIO_ResetBits(GPIOB, GPIO_Pin_1);
	u1_printf("��λʱ�������\r\n");
}

void U3_Event_Process(u8 *data, u16 datalen){
	if((datalen == 7) && (memcmp(data,"\r\nRDY\r\n",7) == 0)){
		u1_printf("4Gģ�鸴λ�ɹ���\r\n");
		
		FS4G_Connect_State_Flag |= FS4G_CONFIG_CIPMODE;
		
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
		
		//����PB0 RDY���ж�
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
	
	if((datalen == 4)&&(data[0]==0x20)){                                           //������յ�4���ֽ� ���� ��һ���ֽ���0x20������if
		u1_printf("�յ�CONNACK����\r\n");	                                       //���ڷ�������
		if(data[3]==0x00){                                                         //�жϵ�4���ֽڣ������0x00������if
			u1_printf("CONNECT���ĳɹ����ӷ�����\r\n");                            //���ڷ�������
			FS4G_Connect_State_Flag |= CONNECT_OK;                                             //���ñ�־λ����ʾCONNECT���ĳɹ�
			MQTT_SubcribPack("/sys/a1RwXdzy6BZ/D001/thing/service/property/set");  //���Ͷ���Topic����
			MQTT_SubcribPack("/sys/a1RwXdzy6BZ/D001/thing/file/download_reply");   //���Ͷ���Topic����
			OTA_Version(); 
		}else{                                                                     //�жϵ�4���ֽڣ��������0x00������else
			u1_printf("CONNECT���Ĵ���׼������\r\n");                            //���ڷ�������
			NVIC_SystemReset();                                                    //����
		}                   	
	}
	
	if((datalen == 5)&&(data[0]==0x90)){                          //������յ�5���ֽ� ���� ��һ���ֽ���0x90������if
		u1_printf("�յ�SUBACK����\r\n");	                      //���ڷ�������
		if((data[datalen-1]==0x00)||(data[datalen-1]==0x01)){     //�жϽ��յ����һ���ֽڣ������0x00����0x01.����if
			u1_printf("SUBCRIBE���ı��ĳɹ�\r\n");                //���ڷ�������
		}else{													  //�жϽ��յ����һ���ֽڣ��������0x00����0x01.����else									
			u1_printf("SUBCRIBE���ı��Ĵ���׼������\r\n");      //���ڷ�������
			NVIC_SystemReset();                                   //����
		}                   	
	}
	
	
	
	
	
	
	if((FS4G_Connect_State_Flag&CONNECT_OK)&&(data[0]==0x30)){                      //���CONNECT���ĳɹ� ���� ��һ���ֽ���0x30������if
		u1_printf("�յ��ȼ�0��PUBLISH����\r\n");                        //���ڷ�������	
		MQTT_DealPublishData(data,datalen);                             //��ȡ����
		u1_printf("%s\r\n",Aliyun_mqtt.CMD_buff);                       //���������ȡ������
		if(strstr((char *)Aliyun_mqtt.CMD_buff,"{\"Light_Switch1\":0}")){     //�����ؼ��ʣ��������������if
			u1_printf("�رտ���\r\n");                                 //���ڷ�������

			//�������ݣ�ͬ������״̬
			MQTT_PublishDataQs0("/sys/a1RwXdzy6BZ/D001/thing/event/property/post","{\"params\":{\"Switch1\":0}}");
		}
		if(strstr((char *)Aliyun_mqtt.CMD_buff,"{\"Light_Switch1\":1}")){     //�����ؼ��ʣ��������������if
			u1_printf("�򿪿���\r\n");                                 //���ڷ�������

			//�������ݣ�ͬ������״̬
			MQTT_PublishDataQs0("/sys/a1RwXdzy6BZ/D001/thing/event/property/post","{\"params\":{\"Switch1\":1}}");
		}
		

		
		//�����ؼ��ʣ��������������if
		if(strstr((char *)Aliyun_mqtt.CMD_buff,"/ota/device/upgrade/a1RwXdzy6BZ/D001")){
			//����ʽ��ȡ�ؼ����ݳɹ�������if
			if(sscanf((char *)Aliyun_mqtt.CMD_buff,"/ota/device/upgrade/a1RwXdzy6BZ/D001{\"code\":\"1000\",\"data\":{\"size\":%d,\"streamId\":%d,\"sign\":\"%*32s\",\"dProtocol\":\"mqtt\",\"version\":\"%26s\",\"signMethod\":\"Md5\",\"streamFileId\":1,\"md5\":\"%*32s\"},\"id\":%*d,\"message\":\"success\"}",&Aliyun_mqtt.size,&Aliyun_mqtt.streamId,Aliyun_mqtt.OTA_tempver)==3){
				u1_printf("OTA�̼���С��%d\r\n",Aliyun_mqtt.size);             //���ڷ�������
				u1_printf("OTA�̼�ID��%d\r\n",Aliyun_mqtt.streamId);           //���ڷ�������
				u1_printf("OTA�̼��汾�ţ�%s\r\n",Aliyun_mqtt.OTA_tempver);    //���ڷ�������
				FS4G_Connect_State_Flag |= OTA_EVENT;                                      //��λ��־λ��OT����
				W25Q64_Erase64K(0);                                            //���0�ſ�����
				if(Aliyun_mqtt.size%256==0){                                   //�жϹ̼���С�Ƿ���256�����������ǵĻ�����if
					Aliyun_mqtt.counter = Aliyun_mqtt.size/256;                //ÿ������256�ֽڣ��������ش���
				}else{                                                         //�жϹ̼���С�Ƿ���256�������������ǵĻ�����else
					Aliyun_mqtt.counter = Aliyun_mqtt.size/256 + 1;            //ÿ������256�ֽڣ��������ش���
				}
				Aliyun_mqtt.num = 1;                                           //��ǰ��1������
				Aliyun_mqtt.downlen = 256;                                     //��¼����������
				OTA_Download(Aliyun_mqtt.downlen,(Aliyun_mqtt.num - 1)*256);   //������������
			}else{                                                             //����ʽ��ȡ�ؼ�����ʧ�ܣ�����else
				u1_printf("��ȡOTA�����������\r\n");                          //���ڷ�������
			}
		}
		//�����ؼ��ʣ��������������if
		if(strstr((char *)Aliyun_mqtt.CMD_buff,"/sys/a1RwXdzy6BZ/D001/thing/file/download_reply")){
			W25Q64_PageWrite(&data[datalen-Aliyun_mqtt.downlen-2],Aliyun_mqtt.num-1);    //���汾�����ص�256�ֽ�
			Aliyun_mqtt.num++;                                                           //��ǰ���ش���+1
			OTA_Send_Progress(Aliyun_mqtt.num, Aliyun_mqtt.counter);
			if(Aliyun_mqtt.num<Aliyun_mqtt.counter){                                     //���С�������ش���������if
				Aliyun_mqtt.downlen = 256;                                               //��¼����������
				OTA_Download(Aliyun_mqtt.downlen,(Aliyun_mqtt.num - 1)*256);             //������������
			}else if(Aliyun_mqtt.num==Aliyun_mqtt.counter){                              //������������ش���������if��˵�������һ������
				if(Aliyun_mqtt.size%256==0){                                             //�жϹ̼���С�Ƿ���256�����������ǵĻ�����if
					Aliyun_mqtt.downlen = 256;                                           //��¼����������
					OTA_Download(Aliyun_mqtt.downlen,(Aliyun_mqtt.num - 1)*256);         //������������
				}else{                                                                   //�жϹ̼���С�Ƿ���256�������������ǵĻ�����else
					Aliyun_mqtt.downlen = Aliyun_mqtt.size%256;                          //��¼����������
					OTA_Download(Aliyun_mqtt.downlen,(Aliyun_mqtt.num - 1)*256);         //������������	
				}
			}else{                                                                       //������������ش���������else��˵���������
				u1_printf("OTA�������\r\n");                                            //���ڷ�������
				memset(OTA_Info.OTA_Version,0,32);                                           //����汾�Ż�����
				memcpy(OTA_Info.OTA_Version,Aliyun_mqtt.OTA_tempver,26);                     //�����汾������
				OTA_Info.FirmwareLen[0] = Aliyun_mqtt.size;                                  //��¼�̼���С
				OTA_Info.OTA_flag = OTA_SET_FLAG;                                        //���ñ�־λ����
				AT24C02_WriteOTAInfo();                                                   //���浽EEPROM
				NVIC_SystemReset();                                                      //������B������
			}
		}
	}
	
	
	
	
	
	
}


void FS4G_Connect_TCP(void){
	
	u3_printf("AT+CIPMODE=1\r\n");
	delay_ms(30);
	u3_printf("AT+CIPMODE=1\r\n");
	delay_ms(30);
	u3_printf("AT+CIPRXF=1\r\n");//���ý��յ�����ĩβ�Ƿ��Զ���ӻس����У�1�����	
	delay_ms(30);
	u3_printf("AT^HEARTCONFIG=1,0,60\r\n");	 	//������������ز���
	delay_ms(30);
	u3_printf("AT^HEARTBEATHEX=4,C000\r\n"); 	//���� HEX �����ʽ����������
	delay_ms(30);
	u3_printf("AT&W\r\n"); 	//���� HEX �����ʽ����������
	delay_ms(30);
	u3_printf("AT+CIPSTART=\"TCP\",a1RwXdzy6BZ.iot-as-mqtt.cn-shanghai.aliyuncs.com,1883\r\n");		//AT+CIPSTART="TCP",test.usr.cn,2317	���� TCP ���ӻ�ע�� UDP �˿ں�
	delay_ms(500);
	MQTT_ConnectPack();
	delay_ms(30);
	
}


/*-------------------------------------------------*/
/*���������ϴ�OTA�汾��                            */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void OTA_Version(void)
{
	char temp[128];
	
	memset(temp,0,128);                                                                  //��ջ�����
	sprintf(temp,"{\"id\": \"1\",\"params\": {\"version\": \"%s\"}}",OTA_Info.OTA_Version);  //��������
	
	MQTT_PublishDataQs1("/ota/device/inform/a1RwXdzy6BZ/D001",temp);                     //�������ݵ�������
}
/*-------------------------------------------------*/
/*��������OTA��������                              */
/*��  ����size������������                         */
/*��  ����offset����������ƫ����                   */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void OTA_Download(int size, int offset)
{
	char temp[256];
	
	memset(temp,0,256);                                                       //��ջ�����
	//��������
	sprintf(temp,"{\"id\": \"1\",\"params\": {\"fileInfo\":{\"streamId\":%d,\"fileId\":1},\"fileBlock\":{\"size\":%d,\"offset\":%d}}}",Aliyun_mqtt.streamId,size,offset);
	u1_printf("��ǰ��%d/%d��\r\n",Aliyun_mqtt.num,Aliyun_mqtt.counter);       //�����������
	MQTT_PublishDataQs0("/sys/a1RwXdzy6BZ/D001/thing/file/download",temp);    //�������ݵ�������
	delay_ms(300);                                                            //��ʱ�����������٣����ܷ�̫��
}

/*-------------------------------------------------*/
/*��������OTA�ϱ�����                              */
/*��  ����num���������صķ�Ƭ���                  */
/*��  ����counter���ܷ�Ƭ��			                   */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void OTA_Send_Progress(int num, int counter)
{
	char temp[256];
	int OTA_Percent;
	OTA_Percent = num*100/counter;
	memset(temp,0,256);                                                       //��ջ�����
	//��������
	sprintf(temp,"{\"id\":\"123\",\"params\":{\"step\":\"%d\",\"desc\":\"������\"}}",OTA_Percent);
	MQTT_PublishDataQs0("/ota/device/progress/a1RwXdzy6BZ/D001",temp);    //�������ݵ�������
	delay_ms(300);                                                            //��ʱ�����������٣����ܷ�̫��
}

