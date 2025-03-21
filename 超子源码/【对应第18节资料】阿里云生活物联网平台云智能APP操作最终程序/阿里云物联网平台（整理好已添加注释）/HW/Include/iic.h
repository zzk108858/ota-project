#ifndef IIC_H
#define IIC_H

#define IIC_SCL_H   gpio_bit_set(GPIOB,GPIO_PIN_6)           //����SCL��PB6�ߵ�ƽ
#define IIC_SCL_L   gpio_bit_reset(GPIOB,GPIO_PIN_6)         //����SCL��PB6�͵�ƽ      

#define IIC_SDA_H   gpio_bit_set(GPIOB,GPIO_PIN_7)           //����SDA��PB7�ߵ�ƽ
#define IIC_SDA_L   gpio_bit_reset(GPIOB,GPIO_PIN_7)         //����SDA��PB7�͵�ƽ

#define READ_SDA    gpio_input_bit_get(GPIOB,GPIO_PIN_7)     //��ȡSDA״̬����ȡPB7��ƽ

void IIC_Init(void);                      //��������
void IIC_Start(void);                     //��������
void IIC_Stop(void);                      //��������
void IIC_Send_Byte(uint8_t txd);          //��������
uint8_t IIC_Wait_Ack(int16_t timeout);    //��������
uint8_t IIC_Read_Byte(uint8_t ack);       //��������


#endif
