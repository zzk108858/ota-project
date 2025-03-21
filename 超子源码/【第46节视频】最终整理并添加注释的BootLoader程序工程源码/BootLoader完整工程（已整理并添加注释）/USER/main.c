#include "gd32f10x.h"
#include "main.h"
#include "usart.h"
#include "delay.h"
#include "fmc.h"
#include "iic.h"
#include "m24c02.h"
#include "boot.h"
#include "w25q64.h"

OTA_InfoCB OTA_Info;          //������24C02�ڵ�OTA��Ϣ��صĽṹ��
UpDataA_CB UpDataA;           //A�������õ��Ľṹ��
uint32_t BootStaFlag;         //��¼ȫ��״̬��־λ

int main(void)
{
	uint8_t i;               //����forѭ��
	
	Delay_Init();            //��ʱ��ʼ��
	Usart0_Init(921600);     //����0��ʼ��
  IIC_Init();              //IIC��ʼ��
	M24C02_ReadOTAInfo();    //��24C02��ȡ���ݵ�OTA_Info�ṹ��
	W25Q64_Init();           //��ʼ��W25Q64
	BootLoader_Brance();     //��֧�ж�
	
	/*--------------------------------------------------*/
	/*--------------------��ѭ��------------------------*/
	/*--------------------------------------------------*/
	while(1){                 
		
		/*--------------------------------------------------*/
		/*       ��ʱ������XmodemЭ�鷢��C���ͳ��         */
		/*--------------------------------------------------*/
		Delay_Ms(10);
		/*--------------------------------------------------*/
		/*              �����ڽ��ջ�����������            */
		/*--------------------------------------------------*/
		if(U0CB.URxDataOUT != U0CB.URxDataIN){                                                                 //IN �� OUT����ȵ�ʱ�����if��˵����������������
			BootLoader_Event(U0CB.URxDataOUT->start,U0CB.URxDataOUT->end - U0CB.URxDataOUT->start + 1);        //����BootLoader_Event��������
			U0CB.URxDataOUT++;                                                                                 //OUT��Ų
		    if(U0CB.URxDataOUT == U0CB.URxDataEND){                                                            //���Ų����END��ǵ����һ����Ա������if
			    U0CB.URxDataOUT = &U0CB.URxDataPtr[0];                                                         //���»ص�����0�ų�Ա
			}
		}
		/*--------------------------------------------------*/
		/*                 XmodemЭ�鷢��C                  */
		/*--------------------------------------------------*/
		if(BootStaFlag&IAP_XMODEMC_FLAG){     //���IAP_XMODEMC_FLAG��־λ��λ��������Ҫ����C
			if(UpDataA.XmodemTimer>=100){     //������ʱ�䣬��ʱ����if
				u0_printf("C");               //����C
				UpDataA.XmodemTimer = 0;      //���������ʱ��ı���
			}
			UpDataA.XmodemTimer++;            //������ʱ��ı���++
		}		
		/*--------------------------------------------------*/
		/*        UPDATA_A_FLAG��λ��������Ҫ����A��        */
		/*--------------------------------------------------*/
		if(BootStaFlag&UPDATA_A_FLAG){
			u0_printf("����%d�ֽ�\r\n",OTA_Info.Firelen[UpDataA.W25Q64_BlockNB]);          //����0�����Ϣ
			if(OTA_Info.Firelen[UpDataA.W25Q64_BlockNB] % 4 == 0){                         //�жϳ����Ƿ���4�����������ǵĻ�����if
				GD32_EraseFlash(GD32_A_START_PAGE,GD32_A_PAGE_NUM);                        //����A��FLASH
				for(i=0;i<OTA_Info.Firelen[UpDataA.W25Q64_BlockNB]/GD32_PAGE_SIZE;i++){    //ÿ�ζ�дһ���������ݣ�ʹ��forѭ����д������������
					W25Q64_Read(UpDataA.Updatabuff,i*1024 + UpDataA.W25Q64_BlockNB*64*1024 ,GD32_PAGE_SIZE);               //�ȴ�W25Q64��ȡһ����Ƭ������������
					GD32_WriteFlash(GD32_A_SADDR + i*GD32_PAGE_SIZE,(uint32_t *)UpDataA.Updatabuff,GD32_PAGE_SIZE);    //д�뵽��Ƭ��A����Ӧ������
				}
				if(OTA_Info.Firelen[UpDataA.W25Q64_BlockNB] % 1024 != 0){                  //�ж��Ƿ��в���һ���������������ݣ��еĻ�����if					
					W25Q64_Read(UpDataA.Updatabuff,i*1024 + UpDataA.W25Q64_BlockNB*64*1024 ,OTA_Info.Firelen[UpDataA.W25Q64_BlockNB] % 1024);             //��W25Q64��ȡ����һ����������������
					GD32_WriteFlash(GD32_A_SADDR + i*GD32_PAGE_SIZE,(uint32_t *)UpDataA.Updatabuff,OTA_Info.Firelen[UpDataA.W25Q64_BlockNB] % 1024);  //Ȼ��д�뵥Ƭ��A����Ӧ������
				}
				if(UpDataA.W25Q64_BlockNB == 0){   //���W25Q64_BlockNB��0����ʾ��OTA����A��������if
					OTA_Info.OTA_flag = 0;         //����OTA_flag��ֻҪ����OTA_SET_FLAG�����ֵ����
					M24C02_WriteOTAInfo();         //д��24C02�ڱ���
				}
				u0_printf("A���������\r\n");      //����0�����Ϣ
				NVIC_SystemReset();                //����
			}else{                                 //�жϳ����Ƿ���4�������������ǵĻ�����else
				u0_printf("���ȴ���\r\n");         //����0�����Ϣ
				BootStaFlag &=~ UPDATA_A_FLAG;     //���UPDATA_A_FLAG��־λ
			}
		}
	}
}
