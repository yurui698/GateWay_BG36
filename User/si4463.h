#ifndef __SI4463_H__
#define __SI4463_H__

#include "stm32f10x.h"
#include "radio_config_si4463.h"
#include "si446x_defs.h"

#define	 SI4463_NIRQ     GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4)

/*4463ʱ�ӽţ���Ҫ�������ģ��SPIʱʹ��*/
#define  SI_SCK_LOW      GPIO_ResetBits(GPIOA, GPIO_Pin_5)
#define  SI_SCK_HIGH     GPIO_SetBits(GPIOA, GPIO_Pin_5)

/*Stm32Ϊ������SI4463Ϊ�ӻ���SDOΪ����������ӻ�����*/
#define  SI_SDO_LOW      GPIO_ResetBits(GPIOA, GPIO_Pin_7)
#define  SI_SDO_HIGH     GPIO_SetBits(GPIOA, GPIO_Pin_7)

/*SDIΪ�������룬�ӻ����*/
#define  SI4463_SDI      GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6)

/* �������ݽ���ָʾ��PE.1����ת��ʾ�н��յ�����*/
#define WX_LED_ON      	 GPIO_ResetBits(GPIOB, GPIO_Pin_6)
#define WX_LED_OFF          GPIO_SetBits(GPIOB, GPIO_Pin_6)	

/*SPI1Ƭѡ*/
#define SPI1_NSS_LOW       GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define SPI1_NSS_HIGH      GPIO_SetBits(GPIOA, GPIO_Pin_4)

/*SI4463��λ�ܽ�PC.06*/
#define  SI4463_SDN_LOW    GPIO_ResetBits(GPIOB, GPIO_Pin_7)
#define  SI4463_SDN_HIGH   GPIO_SetBits(GPIOB, GPIO_Pin_7)

#define  PACKET_LENGTH   0

/*����SI4463ʱ���������SPI�����ú��ΪӲ��SPI
**���SPIʱ��softspi = 1
**Ӳ��SPIʱ��softspi = 0
*/

static u8 softspi = 1;

u8 SPI_WriteByte(u8 TxData);
void SI4463_GET_PROPERTY_1(u8 *buffer);
void SI4463_SET_PROPERTY_1( SI446X_PROPERTY GROUP_NUM, u8 proirity );
void SI4463_Init(void);
void SI4463_INT_STATUS( u8 *buffer );
void SI4463_RESET(void);
void SI4463_CMD(u8 *cmd, u8 cmdsize);
void SI4463_SPI_Active(void);
void SI4463_Rx_State(void);
void SI4463_CONFIG_INIT(void);
void SI4463_PART_INFO(u8 *buffer);
void SI4463_GET_INFO(u8 *buffer);
void SI4463_RX_FIFO_RESET(void);
void SI4463_TX_FIFO_RESET(void);
void SI4463_SEND_PACKET(u8 *txbuffer, u8 size, u8 channel, u8 condition);
void SI4463_START_RX( u8 channel, u8 condition, u16 rx_len, u8 n_state1, u8 n_state2, u8 n_state3 );
u8 SI4463_READ_PACKET( u8 *buffer );

void Delayms(u16 ms);
void Delayus(u32 us);

#endif
