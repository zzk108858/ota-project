#include "gd32f10x.h"
#include "main.h"
#include "4g.h"
#include "delay.h"
#include "usart.h"
#include "w25q64.h"
#include "m24c02.h"
#include "mqtt.h"

/*-------------------------------------------------*/
/*����������λ4Gģ��                               */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void GM4G_Init(void)
{
	rcu_periph_clock_enable(RCU_GPIOB);	                                 //��GPIOBʱ��
	gpio_init(GPIOB,GPIO_MODE_IPD,GPIO_OSPEED_50MHZ,GPIO_PIN_0);         //����PB0���������A��״̬
	gpio_init(GPIOB,GPIO_MODE_OUT_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_2);      //����PB2������4Gģ��ĸ�λ
	
	gpio_init(GPIOB,GPIO_MODE_OUT_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_12);     //����PB12������LED
	gpio_init(GPIOB,GPIO_MODE_OUT_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_13);     //����PB13������LED
	gpio_init(GPIOB,GPIO_MODE_OUT_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_14);     //����PB14������LED
	gpio_init(GPIOB,GPIO_MODE_OUT_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_15);     //����PB15������LED
	gpio_bit_set(GPIOB,GPIO_PIN_12);                                     //���øߵ�ƽ��Ĭ��Ϩ��LED
	gpio_bit_set(GPIOB,GPIO_PIN_13);                                     //���øߵ�ƽ��Ĭ��Ϩ��LED
	gpio_bit_set(GPIOB,GPIO_PIN_14);                                     //���øߵ�ƽ��Ĭ��Ϩ��LED
	gpio_bit_set(GPIOB,GPIO_PIN_15);                                     //���øߵ�ƽ��Ĭ��Ϩ��LED
	
	//ͬ����λ4Gģ��
	u0_printf("��λ4Gģ�飬��ȴ�... ...");
	gpio_bit_set(GPIOB,GPIO_PIN_2);
	Delay_Ms(500);
	gpio_bit_reset(GPIOB,GPIO_PIN_2);
}
/*-------------------------------------------------*/
/*��������������2������                          */
/*��  ����data������ָ��      datalen�����ݳ���    */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void U2_Event(uint8_t *data, uint16_t datalen)
{	
	if((datalen == 6)&&(memcmp(data,"chaozi",6) == 0)){                     //������յ�6���ֽ� ���� chaozi������if
		u0_printf("\r\n4Gģ�鸴λ�ɹ�,�ȴ����ӷ������ɹ�... ...\r\n");	    //���ڷ�������
		rcu_periph_clock_enable(RCU_AF);                                    //����RCU_AF                              
		exti_deinit();                                                      //��λ�ⲿ�ж�
		exti_init(EXTI_0,EXTI_INTERRUPT,EXTI_TRIG_BOTH);                    //��ʼ����0�ⲿ�жϣ������ش���
		gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOB,GPIO_PIN_SOURCE_0);  //ѡ��PB0
		exti_interrupt_enable(EXTI_0);                                      //ʹ����0�ⲿ�ж�
		nvic_irq_enable(EXTI0_IRQn,0,0);                                    //�����ж��������ȼ�               	
	}
	
	if((datalen == 4)&&(data[0]==0x20)){                                           //������յ�4���ֽ� ���� ��һ���ֽ���0x20������if
		u0_printf("�յ�CONNACK����\r\n");	                                       //���ڷ�������
		if(data[3]==0x00){                                                         //�жϵ�4���ֽڣ������0x00������if
			u0_printf("CONNECT���ĳɹ����ӷ�����\r\n");                            //���ڷ�������
			BootStaFlag |= CONNECT_OK;                                             //���ñ�־λ����ʾCONNECT���ĳɹ�
			MQTT_SubcribPack("/sys/a1HvxPcHnkX/D001/thing/service/property/set");  //���Ͷ���Topic����
			MQTT_SubcribPack("/sys/a1HvxPcHnkX/D001/thing/file/download_reply");   //���Ͷ���Topic����
			OTA_Version();                                                         //�ϴ���ǰ�汾��
		}else{                                                                     //�жϵ�4���ֽڣ��������0x00������else
			u0_printf("CONNECT���Ĵ���׼������\r\n");                            //���ڷ�������
			NVIC_SystemReset();                                                    //����
		}                   	
	}
	
	if((datalen == 5)&&(data[0]==0x90)){                          //������յ�5���ֽ� ���� ��һ���ֽ���0x90������if
		u0_printf("�յ�SUBACK����\r\n");	                      //���ڷ�������
		if((data[datalen-1]==0x00)||(data[datalen-1]==0x01)){     //�жϽ��յ����һ���ֽڣ������0x00����0x01.����if
			u0_printf("SUBCRIBE���ı��ĳɹ�\r\n");                //���ڷ�������
		}else{													  //�жϽ��յ����һ���ֽڣ��������0x00����0x01.����else									
			u0_printf("SUBCRIBE���ı��Ĵ���׼������\r\n");      //���ڷ�������
			NVIC_SystemReset();                                   //����
		}                   	
	}
	
	if((BootStaFlag&CONNECT_OK)&&(data[0]==0x30)){                      //���CONNECT���ĳɹ� ���� ��һ���ֽ���0x30������if
		u0_printf("�յ��ȼ�0��PUBLISH����\r\n");                        //���ڷ�������	
		MQTT_DealPublishData(data,datalen);                             //��ȡ����
		u0_printf("%s\r\n",Aliyun_mqtt.CMD_buff);                       //���������ȡ������
		if(strstr((char *)Aliyun_mqtt.CMD_buff,"{\"Switch1\":0}")){     //�����ؼ��ʣ��������������if
			u0_printf("�رտ���1\r\n");                                 //���ڷ�������
			gpio_bit_set(GPIOB,GPIO_PIN_12);                            //�ߵ�ƽ��Ϩ��LED
			//�������ݣ�ͬ������״̬
			MQTT_PublishDataQs0("/sys/a1HvxPcHnkX/D001/thing/event/property/post","{\"params\":{\"Switch1\":0}}");
		}
		if(strstr((char *)Aliyun_mqtt.CMD_buff,"{\"Switch1\":1}")){     //�����ؼ��ʣ��������������if
			u0_printf("�򿪿���1\r\n");                                 //���ڷ�������
			gpio_bit_reset(GPIOB,GPIO_PIN_12);                          //�͵�ƽ������LED
			//�������ݣ�ͬ������״̬
			MQTT_PublishDataQs0("/sys/a1HvxPcHnkX/D001/thing/event/property/post","{\"params\":{\"Switch1\":1}}");
		}
		
		if(strstr((char *)Aliyun_mqtt.CMD_buff,"{\"Switch2\":0}")){     //�����ؼ��ʣ��������������if
			u0_printf("�رտ���2\r\n");                                 //���ڷ�������
			gpio_bit_set(GPIOB,GPIO_PIN_13);                            //�ߵ�ƽ��Ϩ��LED
			//�������ݣ�ͬ������״̬
			MQTT_PublishDataQs0("/sys/a1HvxPcHnkX/D001/thing/event/property/post","{\"params\":{\"Switch2\":0}}");
		}
		if(strstr((char *)Aliyun_mqtt.CMD_buff,"{\"Switch2\":1}")){     //�����ؼ��ʣ��������������if
			u0_printf("�򿪿���2\r\n");                                 //���ڷ�������
			gpio_bit_reset(GPIOB,GPIO_PIN_13);                          //�͵�ƽ������LED
			//�������ݣ�ͬ������״̬
			MQTT_PublishDataQs0("/sys/a1HvxPcHnkX/D001/thing/event/property/post","{\"params\":{\"Switch2\":1}}");
		}
		
		if(strstr((char *)Aliyun_mqtt.CMD_buff,"{\"Switch3\":0}")){     //�����ؼ��ʣ��������������if
			u0_printf("�رտ���3\r\n");                                 //���ڷ�������
			gpio_bit_set(GPIOB,GPIO_PIN_14);                            //�ߵ�ƽ��Ϩ��LED
			//�������ݣ�ͬ������״̬
			MQTT_PublishDataQs0("/sys/a1HvxPcHnkX/D001/thing/event/property/post","{\"params\":{\"Switch3\":0}}");
		}
		if(strstr((char *)Aliyun_mqtt.CMD_buff,"{\"Switch3\":1}")){     //�����ؼ��ʣ��������������if
			u0_printf("�򿪿���3\r\n");                                 //���ڷ�������
			gpio_bit_reset(GPIOB,GPIO_PIN_14);                          //�͵�ƽ������LED
			//�������ݣ�ͬ������״̬
			MQTT_PublishDataQs0("/sys/a1HvxPcHnkX/D001/thing/event/property/post","{\"params\":{\"Switch3\":1}}");
		}
		
		if(strstr((char *)Aliyun_mqtt.CMD_buff,"{\"Switch4\":0}")){     //�����ؼ��ʣ��������������if
			u0_printf("�رտ���4\r\n");                                 //���ڷ�������
			gpio_bit_set(GPIOB,GPIO_PIN_15);                            //�ߵ�ƽ��Ϩ��LED
			//�������ݣ�ͬ������״̬
			MQTT_PublishDataQs0("/sys/a1HvxPcHnkX/D001/thing/event/property/post","{\"params\":{\"Switch4\":0}}");
		}
		if(strstr((char *)Aliyun_mqtt.CMD_buff,"{\"Switch4\":1}")){     //�����ؼ��ʣ��������������if
			u0_printf("�򿪿���4\r\n");                                 //���ڷ�������
			gpio_bit_reset(GPIOB,GPIO_PIN_15);                          //�͵�ƽ������LED
			//�������ݣ�ͬ������״̬
			MQTT_PublishDataQs0("/sys/a1HvxPcHnkX/D001/thing/event/property/post","{\"params\":{\"Switch4\":1}}");
		}
		
		//�����ؼ��ʣ��������������if
		if(strstr((char *)Aliyun_mqtt.CMD_buff,"/ota/device/upgrade/a1HvxPcHnkX/D001")){
			//����ʽ��ȡ�ؼ����ݳɹ�������if
			if(sscanf((char *)Aliyun_mqtt.CMD_buff,"/ota/device/upgrade/a1HvxPcHnkX/D001{\"code\":\"1000\",\"data\":{\"size\":%d,\"streamId\":%d,\"sign\":\"%*32s\",\"dProtocol\":\"mqtt\",\"version\":\"%26s\",\"signMethod\":\"Md5\",\"streamFileId\":1,\"md5\":\"%*32s\"},\"id\":%*d,\"message\":\"success\"}",&Aliyun_mqtt.size,&Aliyun_mqtt.streamId,Aliyun_mqtt.OTA_tempver)==3){
				u0_printf("OTA�̼���С��%d\r\n",Aliyun_mqtt.size);             //���ڷ�������
				u0_printf("OTA�̼�ID��%d\r\n",Aliyun_mqtt.streamId);           //���ڷ�������
				u0_printf("OTA�̼��汾�ţ�%s\r\n",Aliyun_mqtt.OTA_tempver);    //���ڷ�������
				BootStaFlag |= OTA_EVENT;                                      //��λ��־λ��OT����
				W25Q64_Erase64K(0);                                            //���0�ſ�����
				if(Aliyun_mqtt.size%256==0){                                   //�жϹ̼���С�Ƿ���256�����������ǵĻ�����if
					Aliyun_mqtt.counter = Aliyun_mqtt.size/256;                  //ÿ������256�ֽڣ��������ش���
				}else{                                                         //�жϹ̼���С�Ƿ���256�������������ǵĻ�����else
					Aliyun_mqtt.counter = Aliyun_mqtt.size/256 + 1;            //ÿ������256�ֽڣ��������ش���
				}
				Aliyun_mqtt.num = 1;                                           //��ǰ��1������
				Aliyun_mqtt.downlen = 256;                                     //��¼����������
				OTA_Download(Aliyun_mqtt.downlen,(Aliyun_mqtt.num - 1)*256);   //������������
			}else{                                                             //����ʽ��ȡ�ؼ�����ʧ�ܣ�����else
				u0_printf("��ȡOTA�����������\r\n");                          //���ڷ�������
			}
		}
		//�����ؼ��ʣ��������������if
		if(strstr((char *)Aliyun_mqtt.CMD_buff,"/sys/a1HvxPcHnkX/D001/thing/file/download_reply")){
			W25Q64_PageWrite(&data[datalen-Aliyun_mqtt.downlen-2],Aliyun_mqtt.num-1);    //���汾�����ص�256�ֽ�
			Aliyun_mqtt.num++;                                                           //��ǰ���ش���+1
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
				u0_printf("OTA�������\r\n");                                            //���ڷ�������
				memset(OTA_Info.OTA_ver,0,32);                                           //����汾�Ż�����
				memcpy(OTA_Info.OTA_ver,Aliyun_mqtt.OTA_tempver,26);                     //�����汾������
				OTA_Info.Firelen[0] = Aliyun_mqtt.size;                                  //��¼�̼���С
				OTA_Info.OTA_flag = OTA_SET_FLAG;                                        //���ñ�־λ����
				M24C02_WriteOTAInfo();                                                   //���浽EEPROM
				NVIC_SystemReset();                                                      //������B������
			}
		}
	}
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
	sprintf(temp,"{\"id\": \"1\",\"params\": {\"version\": \"%s\"}}",OTA_Info.OTA_ver);  //��������
	
	MQTT_PublishDataQs1("/ota/device/inform/a1HvxPcHnkX/D001",temp);                     //�������ݵ�������
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
	u0_printf("��ǰ��%d/%d��\r\n",Aliyun_mqtt.num,Aliyun_mqtt.counter);       //�����������
	MQTT_PublishDataQs0("/sys/a1HvxPcHnkX/D001/thing/file/download",temp);    //�������ݵ�������
	Delay_Ms(300);                                                            //��ʱ�����������٣����ܷ�̫��
}
