#include "gd32f10x.h"
#include "boot.h"
#include "main.h"
#include "usart.h"
#include "delay.h"
#include "fmc.h"
#include "iic.h"
#include "m24c02.h"
#include "w25q64.h"

load_a load_A;        //����ָ��load_A

/*-------------------------------------------------*/
/*��������BootLoader��֧�ж�                       */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void BootLoader_Brance(void)
{
	if(BootLoader_Enter(20)==0){	
		if(OTA_Info.OTA_flag == OTA_SET_FLAG){       //�ж�OTA_flag�ǲ���OTA_SET_FLAG�����ֵ���ǵĻ�����if
			u0_printf("OTA����\r\n");                //����0�����Ϣ
			BootStaFlag |= UPDATA_A_FLAG;            //��λ��־λ��������Ҫ����A��
			UpDataA.W25Q64_BlockNB = 0;              //W25Q64_BlockNB����0��������OTAҪ����A��		
		}else{                                       //�ж�OTA_flag�ǲ���OTA_SET_FLAG�����ֵ�����ǵĻ�����else
			u0_printf("��תA����\r\n");              //����0�����Ϣ
			LOAD_A(GD32_A_SADDR);                    //��ת��A��
		}
	}
	u0_printf("����BootLoader������\r\n");           //����0�����Ϣ
	
}
/*-------------------------------------------------*/
/*���������ж��Ƿ����BootLoader������             */
/*��  ����timeout ����ʱʱ��                       */
/*����ֵ��1������  0��������                       */
/*-------------------------------------------------*/
uint8_t BootLoader_Enter(uint8_t timeout)
{
	u0_printf("%dms�ڣ�����Сд��ĸ w ,����BootLoader������\r\n",timeout*100);   //����0�����Ϣ
	while(timeout--){                                                            //�ȴ���ʱ��0
		Delay_Ms(100);                                                           //��ʱ200ms
		if(U0_RxBuff[0] == 'w'){                                                 //�����յ��ĵ�һ���ַ�������Сдw���Ž���if
			return 1;                                                            //����1 ����BootLoader������ 
		}
	}
	return 0;                                                                    //����0 ������BootLoader������ 
}
/*-------------------------------------------------*/
/*�����������������������Ϣ                       */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void BootLoader_Info(void)
{ 
	u0_printf("\r\n");                           //����0�����Ϣ
	u0_printf("[1]����A��\r\n");                 //����0�����Ϣ
	u0_printf("[2]����IAP����A������\r\n");      //����0�����Ϣ
	u0_printf("[3]����OTA�汾��\r\n");           //����0�����Ϣ
	u0_printf("[4]��ѯOTA�汾��\r\n");           //����0�����Ϣ
	u0_printf("[5]���ⲿFlash���س���\r\n");     //����0�����Ϣ
	u0_printf("[6]ʹ���ⲿFlash�ڳ���\r\n");     //����0�����Ϣ
	u0_printf("[7]����\r\n");
	u0_printf("[8]���÷�����������Ϣ\r\n");
}
/*-------------------------------------------------*/
/*��������BootLoader����������                   */
/*��  ����data������ָ��      datalen�����ݳ���    */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void BootLoader_Event(uint8_t *data, uint16_t datalen)
{
	int temp,i;                                                                            //temp���ڰ汾��sscanf�жϸ�ʽ    i����forѭ��
	
	/*--------------------------------------------------*/
	/*          û���κ��¼����ж϶�������              */
	/*--------------------------------------------------*/
	if(BootStaFlag == 0){                                                                  //���BootStaFlag����0��û���κ��¼�������if���ж����ĸ�����
		if((datalen==1)&&(data[0]=='1')){                                                  //������ݳ���1�ֽ� �� ���ַ� 1
			u0_printf("����A��\r\n");                                                      //����0�����Ϣ                                                   
			GD32_EraseFlash(GD32_A_START_PAGE,GD32_A_PAGE_NUM);                            //����A����ռ�õ�����
		}
		else if((datalen==1)&&(data[0]=='2')){                                             //������ݳ���1�ֽ� �� ���ַ� 2
			u0_printf("ͨ��XmodemЭ�飬����IAP����A��������ʹ��bin��ʽ�ļ�\r\n");        //����0�����Ϣ 
			GD32_EraseFlash(GD32_A_START_PAGE,GD32_A_PAGE_NUM);	                           //����A����ռ�õ�����  
			BootStaFlag |= (IAP_XMODEMC_FLAG|IAP_XMODEMD_FLAG);                            //��λ IAP_XMODEMC_FLAG �� IAP_XMODEMD_FLAG ��־λ
			UpDataA.XmodemTimer = 0;                                                       //Xmodem���ʹ�дC ���ʱ���������
			UpDataA.XmodemNB= 0;                                                           //���ֽ���XmodemЭ�����ݰ������ı�������
		}
		else if((datalen==1)&&(data[0]=='3')){                                             //������ݳ���1�ֽ� �� ���ַ� 3
			u0_printf("���ð汾��\r\n");                                                   //����0�����Ϣ
			BootStaFlag |= SET_VERSION_FLAG;                                               //��λ SET_VERSION_FLAG 
		}
		else if((datalen==1)&&(data[0]=='4')){                                             //������ݳ���1�ֽ� �� ���ַ� 4
			u0_printf("��ѯ�汾��\r\n");                                                   //����0�����Ϣ
			M24C02_ReadOTAInfo();                                                          //��24C02��ȡ���ֵ�����
			u0_printf("�汾��:%s\r\n",OTA_Info.OTA_ver);                                   //����0�����Ϣ
			BootLoader_Info();                                                             //���������������Ϣ      
		}
		else if((datalen==1)&&(data[0]=='5')){                                             //������ݳ���1�ֽ� �� ���ַ� 5
			u0_printf("���ⲿFlash���س���������Ҫʹ�õĿ��ţ�1~9��\r\n");             //����0�����Ϣ
			BootStaFlag |= CMD_5_FLAG;                                                     //��λ CMD_5_FLAG      
		}
		else if((datalen==1)&&(data[0]=='6')){                                             //������ݳ���1�ֽ� �� ���ַ� 6
			u0_printf("ʹ���ⲿFlash�ڵĳ���������Ҫʹ�õĿ��ţ�1~9��\r\n");           //����0�����Ϣ
			BootStaFlag |= CMD_6_FLAG;                                                     //��λ CMD_6_FLAG 
		}
		else if((datalen==1)&&(data[0]=='7')){                                             //������ݳ���1�ֽ� �� ���ַ� 7
			u0_printf("����\r\n");                                                         //����0�����Ϣ
			Delay_Ms(100);                                                                 //��ʱ
			NVIC_SystemReset(); 	                                                       //����		
		}
		else if((datalen==1)&&(data[0]=='8')){                                             //������ݳ���1�ֽ� �� ���ַ� 8
			u0_printf("�����÷�����������Ϣ\r\n");                                         //����0�����Ϣ
			BootStaFlag |= CMD_8_FLAG;                                                     //��λ CMD_8_FLAG 		
		}
	}
	/*--------------------------------------------------*/
	/*          ����Xmodem�¼���������¼�              */
	/*--------------------------------------------------*/
	else if(BootStaFlag&IAP_XMODEMD_FLAG){                                                 //��� IAP_XMODEMD_FLAG ��λ��ʾ��ʼXmodemЭ���������
		if((datalen==133)&&(data[0]==0x01)){                                               //�ж� XmodemЭ��һ���ܳ�133�ֽ� �� ��һ���ֽ�֡ͷ��0x01
			BootStaFlag &=~ IAP_XMODEMC_FLAG;                                              //�Ѿ��յ����ݰ��ˣ�������� IAP_XMODEMC_FLAG�����ٷ��ʹ�дC
			UpDataA.XmodemCRC = Xmodem_CRC16(&data[3],128);                                //���㱾�ν��յ����ݰ����ݵ�CRC
			if(UpDataA.XmodemCRC == data[131]*256 + data[132]){                            //�����CRC �� ���յ�CRC �Ƚϣ�һ��˵����ȷ������if
				UpDataA.XmodemNB++;                                                        //�ѽ��յ����ݰ�����+1
				memcpy(&UpDataA.Updatabuff[((UpDataA.XmodemNB-1)%(GD32_PAGE_SIZE/128))*128],&data[3],128);   //�����ν��յ����ݣ��ݴ浽UpDataA.Updatabuff������
				if((UpDataA.XmodemNB%(GD32_PAGE_SIZE/128))==0){                            //��C8T6���ԣ�����ѽ������ݰ�������8����������˵������1����1024�ֽڣ�����if
					if(BootStaFlag&CMD5_XMODEM_FLAG){                                      //�ж����������5����Xmodem�Ļ�������if
						for(i=0;i<4;i++){                                                  //W25Q64ÿ��д��256�ֽڣ���C8T6���ԣ�1����1024�ֽڣ���Ҫѭ��4��д
							W25Q64_PageWrite(&UpDataA.Updatabuff[i*256], (UpDataA.XmodemNB/8 - 1) * 4 + i + UpDataA.W25Q64_BlockNB * 64 *4);    //�����ܵ�����д��W25Q64
						}					
					}else{                                                                 //�ж������������5����Xmodem�Ļ����Ǿ��Ǵ���IAP�����ģ�����else
						GD32_WriteFlash(GD32_A_SADDR + ((UpDataA.XmodemNB/(GD32_PAGE_SIZE/128))-1)*GD32_PAGE_SIZE,(uint32_t *)UpDataA.Updatabuff,GD32_PAGE_SIZE);    //д�뵽��Ƭ��A����Ӧ������
					}
				}
				u0_printf("\x06");    //��ȷ������ACK��CRT���
			}else{                    //���CRCУ����󣬽���else
				u0_printf("\x15");    //����NCK��CRT���
			}
		}
		if((datalen==1)&&(data[0]==0x04)){                              //����յ�1���ֽ����� �� ��0x04������if��˵���յ�EOT�����������Ѿ��������
			u0_printf("\x06");                                          //����ACK��CRT���
			if((UpDataA.XmodemNB%(GD32_PAGE_SIZE/128))!=0){             //��C8T6���ԣ��ж��Ƿ��в���1����1024�ֽڵ����ݣ�����н���if����ʣ���Сβ��д��
				if(BootStaFlag&CMD5_XMODEM_FLAG){                       //�ж����������5����Xmodem�Ļ�������if
					for(i=0;i<4;i++){                                   //W25Q64ÿ��д��256�ֽڣ���C8T6���ԣ�1����1024�ֽڣ���Ҫѭ��4��д
						W25Q64_PageWrite(&UpDataA.Updatabuff[i*256], (UpDataA.XmodemNB/8) * 4 + i + UpDataA.W25Q64_BlockNB * 64 *4); //�����ܵ�����д��W25Q64
					}
				}else{                                                  //�ж������������5����Xmodem�Ļ����Ǿ��Ǵ���IAP�����ģ�����else
					GD32_WriteFlash(GD32_A_SADDR + ((UpDataA.XmodemNB/(GD32_PAGE_SIZE/128)))*GD32_PAGE_SIZE,(uint32_t *)UpDataA.Updatabuff,(UpDataA.XmodemNB%(GD32_PAGE_SIZE/128))*128);    //д�뵽��Ƭ��A����Ӧ������
				}
			}
			BootStaFlag &=~ IAP_XMODEMD_FLAG;                           //Xmodem������ϣ������־λ
			if(BootStaFlag&CMD5_XMODEM_FLAG){                           //�ж����������5����Xmodem�Ļ�������if
				BootStaFlag &=~ CMD5_XMODEM_FLAG;                       //���CMD5_XMODEM_FLAG
				OTA_Info.Firelen[UpDataA.W25Q64_BlockNB] = UpDataA.XmodemNB * 128;   //���㲢���汾�δ���ĳ����С
				M24C02_WriteOTAInfo();                                  //���浽24C02
				Delay_Ms(100);                                          //��ʱ
				BootLoader_Info();                                      //�����������Ϣ
			}else{                                                      //�ж������������5����Xmodem�Ļ����Ǿ��Ǵ���IAP�����ģ�����else
				Delay_Ms(100);                                          //��ʱ
				NVIC_SystemReset();                                     //����
			}
		}
	}
	/*--------------------------------------------------*/
	/*         �������ð汾���¼���������¼�           */
	/*--------------------------------------------------*/
	else if(BootStaFlag&SET_VERSION_FLAG){                              //�����֧���������ð汾���¼�
		if(datalen==26){                                                //�жϰ汾�ų����ǲ���26����ȷ����if
			if(sscanf((char *)data,"VER-%d.%d.%d-%d/%d/%d-%d:%d",&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp)==8){  //�жϰ汾�Ÿ�ʽ����ȷ����if
				memset(OTA_Info.OTA_ver,0,32);                          //���OTA_Info.OTA_ver������
				memcpy(OTA_Info.OTA_ver,data,26);                       //�����ڷ��͹����İ汾�ſ�����OTA_Info.OTA_ver������       
				M24C02_WriteOTAInfo();                                  //д��24C02
				u0_printf("�汾��ȷ\r\n");                              //����0�����Ϣ
				BootStaFlag &=~ SET_VERSION_FLAG;                       //�����־λ
				BootLoader_Info();                                      //�����������Ϣ
			}else u0_printf("�汾�Ÿ�ʽ����\r\n");                      //�жϰ汾�Ÿ�ʽ���󣬴���0��ʾ
		}else u0_printf("�汾�ų��ȴ���\r\n");                          //�жϰ汾�ų��ȴ��󣬴���0��ʾ
	}
	/*--------------------------------------------------*/
	/*            ��������5�¼���������¼�             */
	/*--------------------------------------------------*/
	else if(BootStaFlag&CMD_5_FLAG){                                    //�����֧����������5���������ļ�д��W25Q64
		if(datalen==1){                                                 //���ݳ�����1��ȷ������if����ʾW25Q64�Ŀ���
			if((data[0]>=0x31)&&(data[0]<=0x39)){                       //�ж�W25Q64�Ŀ��ţ���Χ1~9����ȷ����if
				UpDataA.W25Q64_BlockNB = data[0] - 0x30;                //���������ַ�1~9��ţ�ת��������1~9
				BootStaFlag |= (IAP_XMODEMC_FLAG|IAP_XMODEMD_FLAG|CMD5_XMODEM_FLAG); //��λ3����־λ
				UpDataA.XmodemTimer = 0;                                             //Xmodem���ʹ�дC ���ʱ���������
				UpDataA.XmodemNB= 0;                                                 //���ֽ���XmodemЭ�����ݰ������ı�������
				OTA_Info.Firelen[UpDataA.W25Q64_BlockNB] = 0;                        //W25Q64�Ŀ��Ŷ�Ӧ�ĳ����С��������
				W25Q64_Erase64K(UpDataA.W25Q64_BlockNB);                             //������Ӧ��W25Q64��
				u0_printf("ͨ��XmodemЭ�飬���ⲿFlash��%d�������س�����ʹ��bin��ʽ�ļ�\r\n",UpDataA.W25Q64_BlockNB); //����0�����Ϣ
				BootStaFlag &=~ CMD_5_FLAG;                                          //�����־λ
			}else u0_printf("��Ŵ���\r\n");                            //�ж�W25Q64�Ŀ��ţ���Χ1~9���������else������0�����Ϣ
		}else u0_printf("���ݳ��ȴ���\r\n");                            //�ж����ݳ��ȣ�����1�Ļ��������else������0�����Ϣ
	}
	/*--------------------------------------------------*/
	/*            ��������6�¼���������¼�             */
	/*--------------------------------------------------*/
	else if(BootStaFlag&CMD_6_FLAG){                     //�����֧����������6��ʹ��W25Q64�ڵĳ���
		if(datalen==1){                                  //���ݳ�����1��ȷ������if����ʾW25Q64�Ŀ���    
			if((data[0]>=0x31)&&(data[0]<=0x39)){        //�ж�W25Q64�Ŀ��ţ���Χ1~9����ȷ����if
				UpDataA.W25Q64_BlockNB = data[0] - 0x30; //���������ַ�1~9��ţ�ת��������1~9
				BootStaFlag |= UPDATA_A_FLAG;            //��λ��־λ��������Ҫ����A��
				BootStaFlag &=~ CMD_6_FLAG;              //�����־λ
			}else u0_printf("��Ŵ���\r\n");             //�ж�W25Q64�Ŀ��ţ���Χ1~9���������else������0�����Ϣ
		}else u0_printf("���ݳ��ȴ���\r\n");             //�ж����ݳ��ȣ�����1�Ļ��������else������0�����Ϣ
	}
	/*--------------------------------------------------*/
	/*            ��������8�¼���������¼�             */
	/*--------------------------------------------------*/
	else if(BootStaFlag&CMD_8_FLAG){                     //�����֧����������6��ʹ��W25Q64�ڵĳ���
		u2_printf("AT+SOCKA=%s\r\n",data);
		Delay_Ms(30);
		u2_printf("AT+SOCKAEN=ON\r\n");
		Delay_Ms(30);
		u2_printf("AT+SOCKBEN=OFF\r\n");
		Delay_Ms(30);
		u2_printf("AT+SOCKCEN=OFF\r\n");
		Delay_Ms(30);
		u2_printf("AT+SOCKDEN=OFF\r\n");
		Delay_Ms(30);
		u2_printf("AT+HEART=ON,NET,USER,60,C000\r\n");
		Delay_Ms(30);
		u2_printf("AT+S\r\n");
		Delay_Ms(30);
		BootStaFlag &=~ CMD_8_FLAG;                      //�����־λ
	}
}
/*-------------------------------------------------*/
/*������������SP                                   */
/*��  ����addr��ջ��ָ���ʼֵ                     */
/*����ֵ����                                       */
/*-------------------------------------------------*/
__asm void MSR_SP(uint32_t addr)
{
	MSR MSP, r0        //addr��ֵ���ص���r0ͨ�üĴ�����Ȼ��ͨ��MSRָ���ͨ�üĴ���r0��ֵд�뵽MSP����ջָ��
	BX r14             //���ص���MSR_SP������������
}
/*-------------------------------------------------*/
/*����������ת��A��                                */
/*��  ����addr��A������ʼ��ַ                      */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void LOAD_A(uint32_t addr)
{
	if((*(uint32_t *)addr>=0x20000000)&&(*(uint32_t *)addr<=0x20004FFF)){     //�ж�spջ��ָ��ķ�Χ�Ƿ�Ϸ����ڶ�Ӧ�ͺŵ�RAM�ؼ���Χ��
		MSR_SP(*(uint32_t *)addr);                                            //����SP
		load_A = (load_a)*(uint32_t *)(addr+4);                               //������ָ��load_Aָ��A���ĸ�λ����
		BootLoader_Clear();                                                   //���B��ʹ�õ�����
		load_A();                                                             //���ú���ָ��load_A���ı�PCָ�룬�Ӷ�ת��A���ĸ�λ����λ�ã������ת
	}else u0_printf("��תA����ʧ��\r\n"); 
}
/*-------------------------------------------------*/
/*�����������B��ʹ�õ�����                        */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void BootLoader_Clear(void)
{
	usart_deinit(USART0);   //��λ����0
	gpio_deinit(GPIOA);     //��λGPIOA
	gpio_deinit(GPIOB);     //��λGPIOB
}
/*-------------------------------------------------*/
/*��������Xmodem_CRC16У��                         */
/*��  ����data������ָ��  datalen�����ݳ���        */
/*����ֵ��У��������                             */
/*-------------------------------------------------*/
uint16_t Xmodem_CRC16(uint8_t *data, uint16_t datalen){
	
	uint8_t i;                                               //����forѭ��
	uint16_t Crcinit = 0x0000;                               //Xmdoem CRCУ��ĳ�ʼֵ��������0x0000
	uint16_t Crcipoly = 0x1021;                              //Xmdoem CRCУ��Ķ���ʽ��������0x1021
	
	while(datalen--){                                        //����datalen��С���ж����ֽ�ѭ�����ٴ�
		Crcinit = (*data << 8) ^ Crcinit;                    //�Ƚ���У����ֽڣ�Ų����8λ
		for(i=0;i<8;i++){                                    //ÿ���ֽ�8��������λ��ѭ��8��
			if(Crcinit&0x8000)                               //�ж�BIT15��1����0,��1�Ļ�������if
				Crcinit = (Crcinit << 1) ^ Crcipoly;         //��1�Ļ��������ƣ���������ʽ
			else                                             //�ж�BIT15��1����0,��0�Ļ�������else
				Crcinit = (Crcinit << 1);                    //��0�Ļ���ֻ����
		}
		data++;                                              //���ƣ�����һ���ֽ�����
	}
	return Crcinit;                                          //����У��������
}















