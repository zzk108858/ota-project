#include "boot.h"

void BootLoader_Branch(void)
{
	if(BootLoader_Enter_Command(20) == 0){
		if(OTA_Info.OTA_flag == OTA_SET_FLAG){
			u1_printf("OTA�и��£�\r\n");
			BootStateFlag |= UPDATA_A_FLAG;
			UpdateA.Updata_A_from_W25Q64_Num = 0;
			
		}else{
			u1_printf("OTA�޸��£���תA��������\r\n");
			LOAD_A(STM32_A_START_ADDR);
		}
	}else{
		
		u1_printf("����Bootloader������\r\n");
		
	}
}

u8 BootLoader_Enter_Command(u8 timeout){ //timeout ��λ �ٺ���
	u1_printf("%d ms ������ w ������������\r\n", timeout * 100);
	while(timeout--){
		delay_ms(100);
		if(USART1_RX_BUF[0] == 'w'){
				return 1;
		}
	}
	return 0;
}

void BootLoader_Info(void){
	u1_printf("\r\n");
	u1_printf("[1]����A��\r\n");
	u1_printf("[2]����IAP����A������\r\n");
	u1_printf("[3]����OTA�汾��\r\n");
	u1_printf("[4]��ѯOTA�汾��\r\n");
	u1_printf("[5]���ⲿFlash���س���\r\n");
	u1_printf("[6]ʹ���ⲿFlash�ڳ���\r\n");
	u1_printf("[7]����\r\n");
	u1_printf("[8]���÷�����������Ϣ\r\n");
}

void Bootloader_Event_Process(u8 *data, u16 datalen)
{
	int Version_sscanf_temp;
	u8 i;
	if(BootStateFlag == 0){	//BootStateFlagd����0�����ϵ��״̬����ʱֻ��Ҫ�����յ���ָ���� 
		if((datalen == 1) && (data[0] == '1')){
			u1_printf("����A��\r\n");
			STM32_EraseFlash(STM32_A_START_PAGE, STM32_A_PAGE_NUM);//��A���ռ����
		}
		else if((datalen == 1) && (data[0] == '2')){
			u1_printf("ͨ��XModemЭ�飬�Ӵ���IAP����A��������ʹ��bin�ļ���ʽ\r\n");
			STM32_EraseFlash(STM32_A_START_PAGE, STM32_A_PAGE_NUM);//��A���ռ�������������³�����¼
			BootStateFlag |= (IAP_XModem_C_FLAG | IAP_XModem_DATA_FLAG);
			UpdateA.XModem_Timer_Count = 0;
			UpdateA.XModem_Count = 0;
		}
		else if((datalen == 1) && (data[0] == '3')){
			u1_printf("����OTA�汾��\r\n");
			BootStateFlag |= IAP_WRITE_VERSION_FLAG;
		}
		else if((datalen == 1) && (data[0] == '4')){
			u1_printf("��ѯOTA�汾��\r\n");
			AT24C02_ReadOTAInfo();
			u1_printf("\r\n�汾�ţ�%s\r\n",OTA_Info.OTA_Version);
			BootLoader_Info();
		}
		else if((datalen == 1) && (data[0] == '5')){
			u1_printf("���ⲿFlash���س���������Ҫʹ�õĿ��ţ�1~9��\r\n");
			BootStateFlag |= CMD_5_FLAG;
		}
		else if((datalen == 1) && (data[0] == '6')){
			u1_printf("ʹ���ⲿFlash�ڳ���������Ҫʹ�õĿ��ţ�1~9��\r\n");
			BootStateFlag |= CMD_6_FLAG;
		}
		else if((datalen == 1) && (data[0] == '7')){
			u1_printf("����\r\n");
			delay_ms(50);
			NVIC_SystemReset();
		}else if((datalen == 1) && (data[0] == '8')){
			u1_printf("�����÷�����������Ϣ\r\n");
			BootStateFlag |= CMD_8_FLAG;
		}
	}
	else if(BootStateFlag & IAP_XModem_DATA_FLAG){	//����XModem����״̬
		if((datalen == 133) && data[0] == 0x01){	//���յ�133�ֽ����ݲ����԰�ͷ��ʼ���ж�ΪXModem���ݣ����봦��
			BootStateFlag &= ~IAP_XModem_C_FLAG;	//ֹͣ����C
			UpdateA.XModemCRC = XModem_CRC16(&data[3], 128);
			if(UpdateA.XModemCRC == ((data[131]<<8) | data[132])){//У�����ݣ�û�������ٽ��д���
				memcpy(&UpdateA.Updata_A_Buff[(UpdateA.XModem_Count % (STM32_PAGE_SIZE/128)) * 128], &data[3], 128);
				UpdateA.XModem_Count++;
				if((UpdateA.XModem_Count % (STM32_PAGE_SIZE/128)) == 0){	//˵��������������1ҳ���������ڲ�Flashд����
					
					if(BootStateFlag & CMD_5_XMODEM_FLAG){
						/*	W25Q64���һ��д��256�ֽ�	*/
						
						for(i = 0; i < (STM32_PAGE_SIZE/256); i++){
							W25Q64_PageWrite(&UpdateA.Updata_A_Buff[i*256], UpdateA.Updata_A_from_W25Q64_Num * 64 * 1024 / 256 + (UpdateA.XModem_Count-8)*128/256+i);
						}
					}else{
						STM32_WriteFlash(STM32_A_START_ADDR + (UpdateA.XModem_Count/(STM32_PAGE_SIZE/128) - 1 ) * STM32_PAGE_SIZE, (u32 *)UpdateA.Updata_A_Buff, STM32_PAGE_SIZE);	
					}
					
				}
				u1_printf("\x06");//һ����������ɣ��ظ�Ӧ���ź�
			}else{	//У�����ݴ���
				u1_printf("\x15");//������󣬻ظ���������Ӧ
			}
		}
		if((datalen == 1) && (data[0] == 0x04)){	//���յ����������־
			u1_printf("\x06");//һ����������ɣ��ظ�Ӧ���ź�
			
			//�ж���û�в���1ҳ��С�����ݣ��еĻ����ⲿ��д��
			if((UpdateA.XModem_Count % (STM32_PAGE_SIZE/128)) != 0){
				
				if(BootStateFlag & CMD_5_XMODEM_FLAG){
						/*	W25Q64���һ��д��256�ֽ�	*/
						
						for(i = 0; i < (STM32_PAGE_SIZE/256); i++){
							W25Q64_PageWrite(&UpdateA.Updata_A_Buff[i*256], UpdateA.Updata_A_from_W25Q64_Num * 64 * 4 + (UpdateA.XModem_Count)/8*4+i);
						}
				}else{
			
					STM32_WriteFlash(STM32_A_START_ADDR + (UpdateA.XModem_Count/(STM32_PAGE_SIZE/128)) * STM32_PAGE_SIZE, (u32 *)UpdateA.Updata_A_Buff, UpdateA.XModem_Count%(STM32_PAGE_SIZE/128)*128);	
				}
			}
			BootStateFlag &= (~IAP_XModem_DATA_FLAG);
			
			if(BootStateFlag & CMD_5_XMODEM_FLAG){
				BootStateFlag &= ~CMD_5_XMODEM_FLAG;
				OTA_Info.FirmwareLen[UpdateA.Updata_A_from_W25Q64_Num] = UpdateA.XModem_Count * 128;
				AT24C02_WriteOTAInfo();
				delay_ms(50);
				BootLoader_Info();
			}else{
				u1_printf("����\r\n");
				delay_ms(100);
				NVIC_SystemReset(); 
			}				
		}
	}else if(BootStateFlag & IAP_WRITE_VERSION_FLAG){	//���ð汾��	VER-1.0.0-2024-06-08-15.54
		u1_printf("datalen = %d\r\n", datalen);
		if(datalen==26){
			if(sscanf((char *)data,"VER-%d.%d.%d-%d-%d-%d-%d.%d", &Version_sscanf_temp, &Version_sscanf_temp, &Version_sscanf_temp, &Version_sscanf_temp, &Version_sscanf_temp, &Version_sscanf_temp, &Version_sscanf_temp, &Version_sscanf_temp) == 8){
				memset(OTA_Info.OTA_Version,0,32);
				memcpy(OTA_Info.OTA_Version,data,26);
				AT24C02_WriteOTAInfo();
				u1_printf("�汾�����óɹ�\r\n",OTA_Info.OTA_Version);
				BootStateFlag &= (~IAP_WRITE_VERSION_FLAG);
				BootLoader_Info();
			}else{
				u1_printf("�汾�Ÿ�ʽ�����˳���ʽ����\r\n");
				BootStateFlag &= (~IAP_WRITE_VERSION_FLAG);
				BootLoader_Info();
			}
		}else{
			u1_printf("�汾�ų��ȴ����˳���ʽ����\r\n");
			BootStateFlag &= (~IAP_WRITE_VERSION_FLAG);
			BootLoader_Info();
		}
	}else if(BootStateFlag & CMD_5_FLAG){	//���ⲿFlash���س���
		if(datalen == 1){
			if((data[0] >= '1') && (data[0] <= '9')){
				UpdateA.Updata_A_from_W25Q64_Num = data[0] - '0';	//������������ı�ţ�ʹ��XModemЭ�鴫��
				BootStateFlag |= (IAP_XModem_C_FLAG | IAP_XModem_DATA_FLAG | CMD_5_XMODEM_FLAG);
				UpdateA.XModem_Timer_Count = 0;
				UpdateA.XModem_Count = 0;
				OTA_Info.FirmwareLen[UpdateA.Updata_A_from_W25Q64_Num] = 0;
				W25Q64_Erase64K(UpdateA.Updata_A_from_W25Q64_Num);
				u1_printf("ͨ��XModemЭ�飬��W25Q64��%d������A��������ʹ��bin�ļ���ʽ\r\n",UpdateA.Updata_A_from_W25Q64_Num);
				BootStateFlag &= ~CMD_5_FLAG;
				
			}else{
				u1_printf("�봫��1~9�ı��\r\n");
			}
		}else{
			u1_printf("����������ȴ���\r\n");
		}
		
	}else if(BootStateFlag & CMD_6_FLAG){	//ʹ���ⲿFlash�ڳ���
		if(datalen == 1){
			if((data[0] >= '1') && (data[0] <= '9')){
				UpdateA.Updata_A_from_W25Q64_Num = data[0] - '0';
				u1_printf("ͨ����%d������A������\r\n",UpdateA.Updata_A_from_W25Q64_Num);
				BootStateFlag |= UPDATA_A_FLAG;
				BootStateFlag &= ~CMD_6_FLAG;
			}else{
				u1_printf("�봫��1~9�ı��\r\n");
			}
		}else{
			u1_printf("����������ȴ���\r\n");
		}
		
	}else if(BootStateFlag & CMD_8_FLAG){	    	//����TCPIPӦ��Ϊ͸������ģʽ	0��͸ 1͸
//		u3_printf("AT+CIPMODE=0\r\n");
//		delay_ms(30);
//		u3_printf("AT+CIPMODE=0\r\n");
//		delay_ms(30);
//		u3_printf("AT+CIPSTART=%s\r\n", data);		//AT+CIPSTART="TCP",test.usr.cn,2317	���� TCP ���ӻ�ע�� UDP �˿ں�
//		delay_ms(30);
//		u3_printf("AT+CIPSTART=%s\r\n", data);		//AT+CIPSTART="TCP",test.usr.cn,2317	���� TCP ���ӻ�ע�� UDP �˿ں�
//		delay_ms(30);
//		u3_printf("AT^HEARTCONFIG=1,0,60\r\n");	 	//������������ز���
//		delay_ms(30);
//		u3_printf("AT^HEARTBEATHEX=4,C000\r\n"); 	//���� HEX �����ʽ����������
//		delay_ms(30);
//		u3_printf("AT^HEARTCONFIG=1,0,60\r\n");	 	//������������ز���
//		delay_ms(30);
//		u3_printf("AT^HEARTBEATHEX=4,C000\r\n"); 	//���� HEX �����ʽ����������
//		delay_ms(30);
//		u3_printf("AT&W\r\n"); 										//�����û���ǰ������
//		delay_ms(30);
		
		BootStateFlag &= ~CMD_8_FLAG;
	}
	
}


__ASM void MSR_SP(u32 addr)
{
	MSR MSP, R0
	BX R14
}

void LOAD_A(u32 addr)
{
	pFunction Jump_To_Application;
	
	if((*(u32 *)addr >= 0x20000000) && (*(u32 *)addr <= 0x20004fff)){
		u1_printf("A��������MSP��Ч���ж�ͨ��\r\n");
		MSR_SP(*(u32 *)addr);
		Jump_To_Application = (pFunction)*(u32 *)(addr + 4);	
		BootLoader_Clear();
		Jump_To_Application();
	}else{
		u1_printf("A��������MSP��Ч���жϲ�ͨ������תA����ʧ�ܣ�\r\n");
	}
}

void BootLoader_Clear(void)
{
	USART_DeInit(USART1);
	
}

u16 XModem_CRC16(u8 *data, u16 datalen)
{
	u16 CRC16_INIT = 0x0000;
	u16 CRC16_POLY = 0x1021;
	
	while(datalen){
		CRC16_INIT = ((*data) << 8) ^ CRC16_INIT;
		for(u8 i=0; i < 8; i++){
			if((CRC16_INIT & 0x8000) == 0){
				CRC16_INIT = CRC16_INIT << 1;
			}else{
				CRC16_INIT = (CRC16_INIT << 1) ^ CRC16_POLY;
			}
		}
		data++;
		datalen--;
	}
	return CRC16_INIT;
}


