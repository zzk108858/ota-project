#include "stm32f10x.h"
#include "main.h"
#include "usart.h"
#include "delay.h"
#include "AT24C02.h"
#include "W25Q64.h"
#include "Board_Flash.h"

#include "FS4G.h"
#include "MQTT.h"

/**
  ******************************************************************************
  * @project	stm32f103c8t6_usart_dma_idle_interrupt
  * @author  	zd
  * @version 	V1.0
  * @date    	2024.5.26

  ******************************************************************************
  */


OTA_InfoCB OTA_Info;     //������24C02�ڵ�OTA��Ϣ��صĽṹ��
UpdateA_CB UpdateA;      //A�������õ��Ľṹ��
u32 FS4G_Connect_State_Flag = 0;   //��¼ȫ��״̬��־λ��ÿλ��ʾ1��״̬


int main(void)
{	
	u8 i;
	u16 timecount = 0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//�����ж����ȼ�����
	USART1_Init(921600);	//���ڳ�ʼ����921600
	USART3_Init(115200);	//���ڳ�ʼ����115200
	delay_Init();                           //��ʱ��ʼ��
	u1_printf("\r\n����1��ʼ���ɹ�\r\n");   //����1��ʼ��
	u3_printf("\r\n����3��ʼ���ɹ�\r\n");   //����3��ʼ��
	IIC_Init();                             //IIC��ʼ��
	W25Q64_Init();                          //W25Q64��ʼ��
	

	AT24C02_ReadOTAInfo();                  //��24C02��ȡ���ݵ�OTA_Info�ṹ��
	FS4G_Init();		//��λ4Gģ��
	
	
	u1_printf("2.0.0\r\n");  //���������Ϣ   VER-1.0.0-2024-06-08-15.54
  while(1)
	{
		delay_ms(10);
		timecount++;
		
		/*--------------------------------------------------*/
		/*             ������3���ջ�����������            */
		/*--------------------------------------------------*/
		//����3����
		if(U3_CB.URxDataOUT != U3_CB.URxDataIN){		//��鴮�ڽ��ջ������Ƿ���������
//			u1_printf("���ν���%d�ֽ�����\r\n",(U3_CB.URxDataOUT->end - U3_CB.URxDataOUT->start + 1));
//			for(i = 0; i< (U3_CB.URxDataOUT->end - U3_CB.URxDataOUT->start + 1); i++){
//				u1_printf("%c",U3_CB.URxDataOUT->start[i]);
//			}	
			U3_Event_Process(U3_CB.URxDataOUT->start, (U3_CB.URxDataOUT->end - U3_CB.URxDataOUT->start + 1));
			U3_CB.URxDataOUT++;		//��ָ�����
			if(U3_CB.URxDataOUT == U3_CB.URxDataEND){
				U3_CB.URxDataOUT = &U3_CB.URxDataPtr[0];	//����Ѿ�������ĩβ�������»ص�������ͷ��
			}
		}
		
		/*--------------------------------------------------*/
		/*             ������3���ͻ�����������            */
		/*--------------------------------------------------*/
		if((U3_CB.UTxDataOUT != U3_CB.UTxDataIN)&&(U3_CB.UTxState == 0)){                                         //IN �� OUT����� �� ����DMA���� ��ʱ�����if��˵����������������
			u1_printf("���η���%d�ֽ�����\r\n",U3_CB.UTxDataOUT->end - U3_CB.UTxDataOUT->start + 1);             //����ο���Ϣ
			for(i=0;i<U3_CB.UTxDataOUT->end - U3_CB.UTxDataOUT->start + 1;i++)
				u1_printf("%02x ",U3_CB.UTxDataOUT->start[i]);	
			u1_printf("\r\n");
			
			U3_CB.UTxState = 1;                                                                                 //���÷���æµ
			
			DMA_InitTypeDef DMA_InitStructure;
			DMA_InitStructure.DMA_PeripheralBaseAddr = USART3_BASE + 0x04; 	 					//DMA�������ַ
			DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)U3_CB.UTxDataOUT->start; 					//DMA�ڴ����ַ
			DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  											//���ݴ��䷽�򣬴��ڴ淢�͵�����
			DMA_InitStructure.DMA_BufferSize = U3_CB.UTxDataOUT->end - U3_CB.UTxDataOUT->start + 1; //DMAͨ����DMA����Ĵ�С��
			DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 					//�����ַ�Ĵ�������
			DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  									//�ڴ��ַ�Ĵ�������
			DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  	//���ݿ��Ϊ8λ
			DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; 					//���ݿ��Ϊ8λ
			DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  														//��������������ģʽ
			DMA_InitStructure.DMA_Priority = DMA_Priority_High; 											//DMAͨ�� xӵ�������ȼ� 
			DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; 															//DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
			
			DMA_Init(DMA1_Channel2, &DMA_InitStructure);
			
			DMA_Cmd(DMA1_Channel2, ENABLE);         //����ͨ��                        
			
			U3_CB.UTxDataOUT++;                                            //OUT��Ų
		    if(U3_CB.UTxDataOUT == U3_CB.UTxDataEND){                       //���Ų����END��ǵ����һ����Ա������if
			    U3_CB.UTxDataOUT = &U3_CB.UTxDataPtr[0];                    //���»ص�����0�ų�Ա
			}
		}		
	
		
		
		
		if(FS4G_Connect_State_Flag & FS4G_CONFIG_CIPMODE){
			
			if(timecount>800){
				
				FS4G_Connect_State_Flag &= ~FS4G_CONFIG_CIPMODE;
				FS4G_Connect_TCP();
			}
		}
		
		if((timecount>=500) && (FS4G_Connect_State_Flag & CONNECT_OK) && (!(FS4G_Connect_State_Flag&OTA_EVENT))){
				timecount = 0;
				MQTT_PublishDataQs0("/sys/a1RwXdzy6BZ/D001/thing/event/property/post","{\"params\":{\"Light_Switch1\":0}}");
			u1_printf("Version : %s\r\n", OTA_Info.OTA_Version);
		}
	
	}

	
}
