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

OTA_InfoCB OTA_Info;          //������24C02�ڵ�OTA��Ϣ��صĽṹ��
UpDataA_CB UpDataA;           //A�������õ��Ľṹ��
uint32_t BootStaFlag;         //��¼ȫ��״̬��־λ
uint32_t timeout;             //�����ʱ����

int main(void)
{
	uint16_t i;              //����forѭ��
	
	
	Delay_Init();            //��ʱ��ʼ��
	Usart2_Init(115200);     //����2��ʼ��
	Usart0_Init(921600);     //����0��ʼ��
  IIC_Init();              //IIC��ʼ��
	M24C02_ReadOTAInfo();    //��24C02��ȡ���ݵ�OTA_Info�ṹ��
	W25Q64_Init();           //��ʼ��W25Q64
	GM4G_Init();             //ͬ����λ4Gģ��
	u0_printf("1.0.0\r\n");  //���������Ϣ
	/*--------------------------------------------------*/
	/*--------------------��ѭ��------------------------*/
	/*--------------------------------------------------*/
	while(1){                 		
		/*--------------------------------------------------*/
		/*             ������2���ջ�����������            */
		/*--------------------------------------------------*/
		if(U2CB.URxDataOUT != U2CB.URxDataIN){                                                                 //IN �� OUT����ȵ�ʱ�����if��˵����������������
			u0_printf("���ν���%d�ֽ�����\r\n",U2CB.URxDataOUT->end - U2CB.URxDataOUT->start + 1);             //����ο���Ϣ
			for(i=0;i<U2CB.URxDataOUT->end - U2CB.URxDataOUT->start + 1;i++)
				u0_printf("%02x ",U2CB.URxDataOUT->start[i]);	
			u0_printf("\r\n");
			U2_Event(U2CB.URxDataOUT->start,U2CB.URxDataOUT->end - U2CB.URxDataOUT->start + 1);                //����U2_Event������2������			
			U2CB.URxDataOUT++;                                                                                 //OUT��Ų
		    if(U2CB.URxDataOUT == U2CB.URxDataEND){                                                            //���Ų����END��ǵ����һ����Ա������if
			    U2CB.URxDataOUT = &U2CB.URxDataPtr[0];                                                         //���»ص�����0�ų�Ա
			}
		}	
		/*--------------------------------------------------*/
		/*             ������2���ͻ�����������            */
		/*--------------------------------------------------*/
		if((U2CB.UTxDataOUT != U2CB.UTxDataIN)&&(U2CB.UTxState == 0)){                                         //IN �� OUT����� �� ����DMA���� ��ʱ�����if��˵����������������
			u0_printf("���η���%d�ֽ�����\r\n",U2CB.UTxDataOUT->end - U2CB.UTxDataOUT->start + 1);             //����ο���Ϣ
			for(i=0;i<U2CB.UTxDataOUT->end - U2CB.UTxDataOUT->start + 1;i++)
				u0_printf("%02x ",U2CB.UTxDataOUT->start[i]);	
			u0_printf("\r\n");
			
			U2CB.UTxState = 1;                                                                                 //���÷���æµ
			dma_memory_address_config(DMA0,DMA_CH1,(uint32_t)U2CB.UTxDataOUT->start);                          //�����µķ�����ʼ��ַ
			dma_transfer_number_config(DMA0,DMA_CH1,U2CB.UTxDataOUT->end - U2CB.UTxDataOUT->start + 1);        //�����µķ�����
			dma_channel_enable(DMA0,DMA_CH1);                                                                  //����ͨ��                        
			
			U2CB.UTxDataOUT++;                                                                                 //OUT��Ų
		    if(U2CB.UTxDataOUT == U2CB.UTxDataEND){                                                            //���Ų����END��ǵ����һ����Ա������if
			    U2CB.UTxDataOUT = &U2CB.UTxDataPtr[0];                                                         //���»ص�����0�ų�Ա
			}
		}
		/*--------------------------------------------------*/
		/*                       �����ʱ                   */
		/*--------------------------------------------------*/
		Delay_Ms(10);                                                                     //��ʱ10ms
		timeout++;                                                                        //��������1
		if((timeout>=500)&&(BootStaFlag&CONNECT_OK)&&(!(BootStaFlag&OTA_EVENT))){         //����500*10=5000ms��ʱ ��  CONNECT���ĳɹ�  ��  û��OTA�¼� ����if
			timeout = 0;                                                                  //�����ʱ����
			//�ȼ�0��Publish�����ϴ�����
			MQTT_PublishDataQs0("/sys/a1HvxPcHnkX/D001/thing/event/property/post","{\"params\":{\"temperature\":33.33,\"Humidity\":22.22,\"Light\":22.22}}");
		}

	}
}
