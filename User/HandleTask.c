#include "HandleTask.h"
#include "systemclock.h"
#include "string.h"
#include "DataFlash.h"
#include "Stm32_Configuration.h"
#include <stdlib.h>
#include "stm32f10x_it.h"
#include "stm32f10x.h"
#include "SI7021.h"

#include "MQTTPacket.h"
#include "StackTrace.h"

#include "SI7021.h"
#include "max44009.h"
#include "password_MD5.h"
#include "PIDCollector.h"

/* 2017��4��ȫ����Ӳ��˵����
USART2��PD3~PD7(Remap);CDMA��GSM
����ָʾ�ƣ�PB6;
SPI1��Դ��ʼ�� PB7;
SPI2:����P13: P13_1 VCC, P13_2 PB14-ģ��SPI2_MISO, P13_3 PB15-ģ��SPI2_MISI, P13_4 PB12-ģ��SPI2_NSS, P13_5 PB13-ģ��SPI2_SCK, P13_7 GND, P13_8 NRST��ϵͳrest��, P13_9 PB11-ģ��INT,��������

BOOT0:����P12: P12_1 VCC, P12_2 BOOT0,���ض̽�   
   
USART1:����P11: USART1,P11_1 VCC, P11_2  PA10-RX1��P11_3 PA9-TX1��P11_4 GND;�������س�

SWD:����P10��SWD�ӿڣ�P10_1 VCC,P10_2 SWCLK,P10_3 NRST,P10_6 GND,P10_7 SWDIO,1-6����Բ��ţ�2-7����Բ��ţ������Ų���

WIFI:����P9��WIFI�ӿڣ���USART2��ѡһ��P9_1 PD0-ģ��REST,P9_3 ģ��ADC ����,P9_5 PD7-ģ��CH_PD,P9_7 ģ��GPIO16 ����,P9_9 ģ��GPIO14 ����,P9_11 ģ��GPIO12 ����,P9_13 ģ��GPIO13 ����,P9_15 VCC
	   P9_2 PD6-RX2-��ģ��TX ����470��VCC��,P9_4 PD5-TX2-��ģ��RX ����470��VCC?,P9_6 ģ��GPIO4 ����,P9_8 ģ��GPIO5 ����,P9_10 ģ��GPIO0 ����,P9_10 ģ��GPIO2 ����,P9_14-PD_16 GND 
   
USB:����P8��USB�ӿڣ�P8_1 +5V���� P8_2 PA11-USB_DM-USB_N, P8_3 PA12-USB_DP-USB_P, P8_4 GND ��Ҫ������ƣ�
   
UART5:����P7��UART5��PD2-R-UART5_RX,PB8-�շ��л���(�ظ���U6_7 U6-7Ϊָʾ�� )PC12-D-UART5_TX,AIO3��BIO3,RS485��
	 
URAT4:����P6��UART4, PC11-R-UART4_RX,PB9-�շ��л���PC10-D-UART4_TX,AIO2����ע���󣿣���BIO2��RS485 ������
   
   ����P5���ɼ��ӿڣ�P5_1 24VOUT-PA11, P5_2 GND, P5_3 DATA5-PA0(������̼)��P5_4 SCK-PC3 
   
   ����P4: �ɼ��ӿڣ�P4_1 VCC, P4_2 GND, P4_3 DATA4-PA1(������ʪ��)��P4_4 SCK-PC3  

   ����P3: �ɼ��ӿڣ�P3_1 VCC, P3_2 GND, P3_3 DATA3-PC0(���ն�)��P3_4 SCK-PC3  
   
   ����P2: �ɼ��ӿڣ�P2_1 24VOUT-PA11, P2_2 GND, P2_3 DATA1-PC2(����ˮ��)��P2_4 DATA2-PC1(�����¶�)  
   
USART3:����P1����Դ USART3�ӿڣ�P1_1 InVCC, P1_2 GND,PD9-R-USART3_RX(Remap),PD10-�շ��л���PD8-D-USART3_TX(Remap),P1_3 AIO1��P1_4 BIO1��RS485  
  
   U6 ESP8266WIFI�ӿ�
	 ��·����ܽ�����:P2:PC1�����¶�(�������)DATA2��PC2����ˮ��(Һλ)DATA1;P3:PC0���ն�DATA3;P5:PA0������̼DATA5;P4:PA1������ʪ��DATA4;PC3 ΪSCK;
���������PE7-24V��Դ�������
���������ӣ�P6(AI02��BI02)/P7(24V0��GND)
����1���ӣ�P11-1-VCC,P11-2-RXD,P11-3-TXD,P11-4-GND.˵��������CPU1�Ĺܽ�ΪP11-1
�ɼ���ʽ�������������˲��������ɼ���ʽ��  00 �޲ɼ���01 ���� k=100 b=0��02 ������ʪ�� ʪ��k=524.29 b=-6 �¶� k=372.96 b=-46.85��
                                       03 4~20ma��04 ������̼,
																			 05 ����ѹ�����루CPS131��ѹ����k=728.18 b=+30 ��λ��kpa���¶ȣ�k=436.91 b=-40 ��λ����
																			 06 ���������룬07 Ƶ�����룬08 0~1Vdc���룬09 RS485 
                                       10 ����ѹ����cps120��Һλ��������k=193.16 b=30����λ��kpa,����ѹ�� ������k=182.04  b=30����λ��kpa 
																			    �¶�=�����¶ȵ�ͨ��cps120 ������k=99.30  b=-40����λ���棩
																			 11 ��ѹҺλ;��ѹҺλ�LWP5050GD��������	k=1��b=0 ��λ��pa																	 
																			 12 ��ѹҺλ��LWP5050GD��P4����ʪ��ͨ�� 
																			   ��Һ��Һλ������1����k=9854.89.69 b=-1.02����λ��M(0-5.6M) ����ѹ��������ѹ����ʾ��kpa:k=1008.25,b=-10
																			   ��Һ��Һλ������1����k=9766.21��b=-1.03����ѹҺλ������8�� k=9766.21��b=0 
																			 ,�¶ȣ�����2��=�����¶�ͨ��LWP5050GD  ������k=524.29  b=-40����λ����	
	 ȱʡ���壺 ȱʡ���壺����1-P3-ͨ��0�����նȣ�  ����2-P4-ͨ��1������ʪ�ȣ�����3-P4-ͨ��1�������¶ȣ�    ����4-P2-DAT1-ͨ��2������ˮ�֣�����5-P2-DAT2-ͨ��3�������¶ȣ�
	           ����6-P5-ͨ��4��������̼������7-P7(AI03��BI03)/P6(24V+0��GND)-ͨ��5��RS485-����������8-��ͨ���������ѹҺλ*/
//����ˮ���¶ȴ������ӷ�����ɫ��+����ɫ������Ҫ������������������ɫ��ˮ�֣���ɫ���¶ȡ�
/*Modbus��վID*/
#define SLAVE_ID                 0x01

/*Modbus������*/
#define READ_COIL_STATUS         0x01   /*����Ȧ�Ĵ���  :��֧�ֹ㲥���涨Ҫ������ʼ��Ȧ����Ȧ��                  */
#define READ_INPUT_STATUS        0x02	/*��״̬�Ĵ���	:��֧�ֹ㲥���涨Ҫ����������ʼ��ַ���Լ������źŵ�����  */
#define READ_HOLDING_REGISTER    0x03	/*�����ּĴ���	:��֧�ֹ㲥���涨Ҫ���ļĴ�����ʼ��ַ���Ĵ���������      */
#define READ_INPUT_REGISTER      0x04 	/*������Ĵ���	:��֧�ֹ㲥���涨Ҫ���ļĴ�����ʼ��ַ���Ĵ���������      */
#define WRITE_SINGLE_COIL        0x05 	/*д����Ȧ�Ĵ���:֧�ֹ㲥���涨��������Ȧ��ON/OFF״̬��0xFF00ֵ������Ȧ��
														 ��ON״̬��0x0000ֵ������Ȧ����OFF״̬������ֵ����Ȧ��Ч */
#define WRITE_SINGLE_REGISTER    0x06 	/*д�����ּĴ���:֧�ֹ㲥�������Ԥ��ֵ�ڲ�ѯ������                      */
#define WRITE_MULTIPLE_COIL      0x0F	/*д����Ȧ�Ĵ���:֧�ֹ㲥                                                */
#define WRITE_MULTIPLE_REGISTER  0x10	/*д�ౣ�ּĴ���:֧�ֹ㲥                                                */

/*RS485�շ��л�*/
#define TXENABLE3		         GPIO_SetBits(GPIOD, GPIO_Pin_10)
#define RXENABLE3		         GPIO_ResetBits(GPIOD, GPIO_Pin_10)
#define TXENABLE4		         GPIO_SetBits(GPIOB, GPIO_Pin_9)
#define RXENABLE4		         GPIO_ResetBits(GPIOB, GPIO_Pin_9)
#define TXENABLE5		         GPIO_SetBits(GPIOB, GPIO_Pin_8)
#define RXENABLE5		         GPIO_ResetBits(GPIOB, GPIO_Pin_8)

#define false 0
#define true  1
#define bool u8

/*CRC��λ�ֽڱ�*/
const unsigned char crc_lo[256]={
	0x00,0xC0,0xC1,0x01,0xC3,0x03,0x02,0xC2,0xC6,0x06,0x07,0xC7,0x05,0xC5,0xC4,
	0x04,0xCC,0x0C,0x0D,0xCD,0x0F,0xCF,0xCE,0x0E,0x0A,0xCA,0xCB,0x0B,0xC9,0x09,
	0x08,0xC8,0xD8,0x18,0x19,0xD9,0x1B,0xDB,0xDA,0x1A,0x1E,0xDE,0xDF,0x1F,0xDD,
	0x1D,0x1C,0xDC,0x14,0xD4,0xD5,0x15,0xD7,0x17,0x16,0xD6,0xD2,0x12,0x13,0xD3,
	0x11,0xD1,0xD0,0x10,0xF0,0x30,0x31,0xF1,0x33,0xF3,0xF2,0x32,0x36,0xF6,0xF7,
	0x37,0xF5,0x35,0x34,0xF4,0x3C,0xFC,0xFD,0x3D,0xFF,0x3F,0x3E,0xFE,0xFA,0x3A,
	0x3B,0xFB,0x39,0xF9,0xF8,0x38,0x28,0xE8,0xE9,0x29,0xEB,0x2B,0x2A,0xEA,0xEE,
	0x2E,0x2F,0xEF,0x2D,0xED,0xEC,0x2C,0xE4,0x24,0x25,0xE5,0x27,0xE7,0xE6,0x26,
	0x22,0xE2,0xE3,0x23,0xE1,0x21,0x20,0xE0,0xA0,0x60,0x61,0xA1,0x63,0xA3,0xA2,
	0x62,0x66,0xA6,0xA7,0x67,0xA5,0x65,0x64,0xA4,0x6C,0xAC,0xAD,0x6D,0xAF,0x6F,
	0x6E,0xAE,0xAA,0x6A,0x6B,0xAB,0x69,0xA9,0xA8,0x68,0x78,0xB8,0xB9,0x79,0xBB,
	0x7B,0x7A,0xBA,0xBE,0x7E,0x7F,0xBF,0x7D,0xBD,0xBC,0x7C,0xB4,0x74,0x75,0xB5,
	0x77,0xB7,0xB6,0x76,0x72,0xB2,0xB3,0x73,0xB1,0x71,0x70,0xB0,0x50,0x90,0x91,
	0x51,0x93,0x53,0x52,0x92,0x96,0x56,0x57,0x97,0x55,0x95,0x94,0x54,0x9C,0x5C,
	0x5D,0x9D,0x5F,0x9F,0x9E,0x5E,0x5A,0x9A,0x9B,0x5B,0x99,0x59,0x58,0x98,0x88,
	0x48,0x49,0x89,0x4B,0x8B,0x8A,0x4A,0x4E,0x8E,0x8F,0x4F,0x8D,0x4D,0x4C,0x8C,
	0x44,0x84,0x85,0x45,0x87,0x47,0x46,0x86,0x82,0x42,0x43,0x83,0x41,0x81,0x80,
	0x40};


/*CRC��λ�ֽڱ�*/
const unsigned char crc_hi[256]={
	0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,
	0x40,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,
	0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,
	0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,
	0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,
	0x40,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,
	0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,
	0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
	0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,
	0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x01,0xC0,
	0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,
	0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
	0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,
	0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x01,0xC0,
	0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,
	0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
	0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,
	0x40};


 __attribute__((section("NO_INIT"),zero_init)) static u8 Collectors[65][16];//���նȡ�����ʪ�ȡ������¶ȡ�����ˮ�֣�Һλ���������¶ȣ������������������̼�����ౣ��
 __attribute__((section("NO_INIT"),zero_init)) static  u8 Controllers[32][8] ;
//ƽ̨���õ�1��������2100H��2104H��2108H��210CH����2��������2110H��2114H����������2100H-22FCHΪ��������������
	
//__attribute__((zero_init)) static u8 I_current_limit[32][8];
/*ƽ̨���õ�1������������2300H��2304H��2308H��230CH����2������������2130H��2134H������2300H-24FCHΪ4��ͨ����������������ʱ�����ã�������޴˹��ܣ�*/
	
 __attribute__((section("NO_INIT"),zero_init)) static u8 hand_auto_flg[73][2];
/*����ƽ̨�·������Զ��л����00H��00H-�ֶ���01H��00H-�Զ���
	��64�����Կػ�·��9�������Կػ�·��	2500HΪ��1���Կػ�·��2504HΪ��2���Կػ�·��������
	2500H-25FCHΪ64�����Կػ�·��2600H-2620HΪ9�������Կػ�·��
	hand_auto_flg���մ�������ƽ̨�·����Կ��л�����ͻظ�ƽ̨�Կ�״̬ʹ�ã�hand_auto_flg[n][0]��[1]
	�ֱ�Ϊ��00H��00H�ֶ���01H��00H�Զ�*/
static u8 hand_auto_count[73]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	                           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//���Զ�״̬�ϱ�ƽ̨������0���ϱ�ƽ̨	
 __attribute__((section("NO_INIT"),zero_init)) static u8 ZZ_Wired_flag[65];	
 __attribute__((section("NO_INIT"),zero_init)) static u8 ZZ_Wireles_flag[65];
/*���ߡ�����ͨ����վͨ����־��0x00������,0x01���ڣ�0x02�������ڣ����ڼ�⣻
	 ZZ_Wireles_flag[64]!=0��ʾ���س�ʼ�����
	 ZZ_Wired_flag[64]!=0��ʾ�����вɼ����� */
	
 __attribute__((section("NO_INIT"),zero_init)) static u8 crtl_cmd_num[32][4];//���յ���������ƽ̨�·��Ŀ���������¼��������վ�·�����Ĵ��������������Ƶ��·�
static u8 ctrl_j=0;//crtl_cmd_num[ctrl_i][ctrl_j],����վ�����������¼ÿ���������Ŀ��Ƶ����
//����վ���Ϳ�������������յ�ƽ̨�������Ŀ�������õ�Ԫ��3������ͨ����������վ����3�θ�����յ���վ�ظ��õ�Ԫ��0
//static u8 collector_j=0;//��ɼ������������վ�������������ݵ�ļ���;����һ���Է�������������
	
 __attribute__((section("NO_INIT"),zero_init)) static u8 crtl_cmd_numWX[32][4];
//����վ���Ϳ�������������յ�ƽ̨�������Ŀ�������õ�Ԫ��3������ͨ����������վ����3�θ�����յ���վ�ظ��õ�Ԫ��0	
static u8 ctrl_wx_j=0;//crtl_cmd_numWX[ctrl_i][ctrl_j],����վ�����������¼ÿ���������Ŀ��Ƶ����
//����վ���Ϳ�������������յ�ƽ̨�������Ŀ�������õ�Ԫ��3������ͨ����������վ����3�θ�����յ���վ�ظ��õ�Ԫ��0

 __attribute__((section("NO_INIT"),zero_init)) static u8 init_cmd_numYX[64];
//�����ϵ�ͨ����������վ���Ͳ�ѯ����Ĵ�������ԪֵΪ0������ѯ����ԪֵΪn���ظ���ѯn�Ρ��յ��ظ���0��תΪ������ѯ
__attribute__((section("NO_INIT"),zero_init)) static u8 init_cmd_numWX[64];
//�����ϵ�ͨ����������վ���Ͳ�ѯ����Ĵ�������ԪֵΪ0������ѯ����ԪֵΪn���ظ���ѯn�Ρ��յ��ظ���0��תΪ������ѯ

static u8 Query_Wired_WirelesYX=0;//����
static u8 set_param_flag[32];
static u8 CSH_countYX=0;//���߳�ʼ�����=1

#define CONTROLLERS_CMD         0x00
#define COLLECTORS_CMD          0x01
#define ZZ_QUERY_COLLECTOR      0x02
#define ZZ_QUERY_CONTROLLER     0x03

static u8  Query_Flag = CONTROLLERS_CMD;
static u8  Query_Index_Controller = 0;
static u8  Query_Index_Collector  = 0;
static u8  Query_IndexZZ_C_YX  = 0;
static u8  Query_IndexZZ_K_YX  = 0;

       u8  ReceiveData1[256];
static u8  ReportData1[128];
       u8  ReceiveData2[256];
static u8  ReportData2[256];
       u8  ReceiveData3[128];
static u8  ReportData3[128];
       u8  ReceiveData4[256];
static u8  ReportData4[256];
static u8  ReceiveData5[128];
static u8  ReportData5[128];
extern u8   RxFlag1;		//����1���ձ�־λ��0���ڽ��գ�1�������
extern u8   TxFlag1;		//����1���ͱ�־λ��0���ڷ��ͣ�1�������
extern u8   RxFlag2;		//����2���ձ�־λ��0���ڽ��գ�1�������
extern u8   TxFlag2;		//����2���ͱ�־λ��0���ڷ��ͣ�1�������
extern u8   RxFlag3;    //����2���ձ�־λ��0���ڽ��գ�1�������
extern u8   TxFlag3;		//����3���ͱ�־λ��0���ڷ��ͣ�1�������
extern u8   RxFlag4;		//����5���ձ�־λ��0���ڽ��գ�1�������
extern u8   TxFlag4;		//����5���ͱ�־λ��0���ڷ��ͣ�1�������
extern u8   RxFlag5;		//����5���ձ�־λ��0���ڽ��գ�1�������
extern u8   TxFlag5;		//����5���ͱ�־λ��0���ڷ��ͣ�1�������

extern u8   RecDataBuffer1[]; 
extern u8   RecLen1;
extern u8   RecDataBuffer2[]; 
extern u8   RecLen2;
extern u8   RecDataBuffer5[]; 
extern u8   RecLen5;
extern u8   RecDataBuffer3[]; 
extern u8   RecLen3;
extern u8   RecDataBuffer4[]; 
extern u8   RecLen4;
extern u8   USART1SendTCB[];
extern u8   USART1BufferCNT;
extern u8   USART2SendTCB[];
extern u8   USART2BufferCNT;
extern u8   USART3SendTCB[];
extern u8   USART3BufferCNT;
extern u8   UART4SendTCB[];
extern u8   UART4BufferCNT;
extern u8   UART5SendTCB[];
extern u8   UART5BufferCNT;


static void RxReport1(u8 len,u8 *pData);
static void RxReport2(u8 len,u8 *pData);
static void RxReport3(u8 len,u8 *pData);
static void RxReport4(u8 len,u8 *pData);
static void RxReport5(u8 len,u8 *pData);
static void RxReport1_csb_yw(u8 len,u8 *pData);

static u16 CRCReport4;//RxReport4��������ʱ��������
static u16  GetCRC16(u8 *Msg, u16 Len);
static  u8 gc_i,gc_CRCHigh,gc_CRCLow;//��CRC�ֽڳ�ʼ��,��CRC�ֽڳ�ʼ��;GetCRC16��������ʱ��������
static	u16 gc_index;		  //CRCѭ���е�����;GetCRC16��������ʱ��������
	
static void Send_slave_cmd(void);
static u8 bytelen3=0;//Send_slave_cmd��������ʱ��������

static void WriteDataToBuffer(u8 port,u8 *ptr,u8 start,u8 len);

static u8 WriteMultipleRegister(u8 Slave_ID,u16 addr,u16 num,u8 *pData,u8 *temp);
static u16 wm_CRC_Val;//WriteMultipleRegister��������ʱ��������
static u8  WriteSingleRegister(u8 Slave_ID,u16 addr,u8 *pData,u8 *temp);
	
static u8 ReadData(u8 Slave_ID,u8 function_code,u16 addr,u16 num,u8 *temp);
static u16 rd_CRC_Val;//ReadData��������ʱ��������

/*����Ϊ���������趨����*/
//static u8 factory_gateway_setflg=0;//���յ����������趨ȷ����=1��������ַ��EF_1E������0 ����factory_gateway_setflg=���趨ȷ����=1����factory_gateway_set[159]д��flash�����factory_gateway_setflg=0
/*�ɼ���ʽ�������������˲��������ɼ���ʽ��00 �޲ɼ���01 ���� k=100 b=0��02 ������ʪ�� ʪ��k=524.29 b=-6 �¶� k=372.96 b=-46.85��
                                       03 4~20ma��04 ������̼,05 ����ѹ�����룬06 ���������룬07 Ƶ�����룬08 0~1Vdc���룬09 RS485 
                                       10 ����Һλ��cps120��������k=193.16 b=30����λ��kpa																			
																			 ���¶�=�����¶ȵ�ͨ��cps120 ������k=99.30  b=-40����λ���棩
																			 12 ��ѹҺλ��LWP5050GD������ʪ��ͨ�� ������k=9854.89 b=-0.102����λ��M ����ѹ������
																			 12 ��ѹҺλ��LWP5050GD��P4����ʪ��ͨ�� 
																			   ��Һ��Һλ������1����k=9854.89.69 b=-1.02����λ��M(0-5.6M) ����ѹ��������ѹ����ʾ��kpa:k=1008.25,b=-10
																			   ��ѹҺλ������1����k=9766.21��b=-1.03����ѹҺλ������8�� k=9766.21��b=0 
																			 ,�¶ȣ�����2��=�����¶�ͨ��LWP5050GD  ������k=524.29  b=-40����λ����                                      																			 
																			 
	����Ϊ���زɼ�ͨ��5�趨��
	        11 ��ѹҺλ;��ѹҺλ�LWP5050GD��������
	        13 ���������������ݹ������������7 ˲ʱ����,��ʽ�ٳ�62.5,ת���ɵ�λΪ��������/Сʱ��
          14 ��������ƣ��Ϻ�����ưԣ�������7 �ۼ����� ��λ��L��
          15 ����1������Һλ������8-ת��Һλ���ݣ�����8�ı�������factory_gateway_set[28]�趨���ߣ���λ��cm
					16 ���������-��ѹҺλʵ�飬����7-�ۼ����� ��λ��L������8-��������  ��λ��mm������
					17 ���������-�����ͳ�����Һλʵ�飬����7-�������ۼ����� ��λ��L������8-��������  ��λ��mm������
					19 ��Һ��������Һλ�������ȣ�������8����λ��mm��k=1��b=0������
					20 ��Һ���߾�ѹ��ѹҺλ������8����λ��mm��k=1��b=0������
					21 ��Һ��������ƵҺλ������8����λ��mm��k=1��b=0������					
					   factory_gateway_set[19]=�����߶ȣ�factory_gateway_set[20]=�˲�������factory_gateway_set[28]=���ߣ�factory_gateway_set[29]=�������� 
					22 RS485(����+����)��ַ��0x01����о���Զ����Ǳ����޹�˾+(PM1.0+PM2.5+PM10)��ַ:0x02����������ӿƼ�(5������)
	        24 DO+ˮ�� (������վʵ��,�ɼ����ز��趨)
					25 ��ȼ���屨������վ��ַ��0x01��������Ѷ��ͨ���ӿƼ����޹�˾����ֵ��0x0016��25��=0.25%��k=100��b=0�������ͱ���0x0018:0x0096��150=Ũ��*10,15%LEL��
	����Ϊ���زɼ�ͨ��1�趨P4��
	18 �����⣬��λ:mm������2��mm��k=1��b=0 �������ͳ�����Һλ;P4-CLK--Trig/TX,P4-DAT4--Echo/RX
	23 ��Ӧʽ��ѩ����������λ��ÿ�������������k=1��b=0 ��P4-CLK--����,P4-DAT4--OUT
	
	�ɼ���ʽ������Ϊ��25��
	*/
 u8  factory_gateway_set[255]=            //�������·����������趨������Ҫд��flash0x0805 C000����256k��ʼд�룬0x0804��1��������64k,ÿ��д�����2Kbyte
     { 0x01,  //��������=1 ���ţ�2 �ƶ���3 ��̫����4 WiFi��5 USB��0 �����ӣ��������������� EF 06 00 00 01 00 9F 14�����ܺţ�0x06,��ַ��EF_00 ,factory_gateway_set[0]
			 0x03,  //Э������=1 TCP/IP;2 MQTT����÷����; 3 SDK�����룩; 4 ����MQTT����׼MQTT��;�����������������ܺţ�0x06,��ַ��EF_01,factory_gateway_set[1]
			 0xff,0xff,0xff,0xff, //����ID3~0;�����������ܺţ�0x06,��ַ��EF_02~EF_05,factory_gateway_set[2]~[5]
			 0xff,0xff,0xff,0xff, //�ɿ���ID3~0;���ܺţ�0x06,��ַ��EF_06~EF_09,factory_gateway_set[6]~[9]
			 0x00,  //GPS��λ=1����GPS��λ���ܣ�0���ޣ����ܺţ�0x06,��ַ��EF_0A,factory_gateway_set[10]
			 0x01,  //���زɼ�=1�������زɼ����ܣ�0���ޣ����ܺţ�0x06,��ַ��EF_0B,factory_gateway_set[11]
			 01,01,00,  //���زɼ�ͨ��0�����趨���ɼ���ʽ�������������˲��������ɼ���ʽ��00 �޲ɼ���01 ���գ�02 ������ʪ�ȣ�03 4~20ma��04 ������̼
			 02,02,00,  //���زɼ�ͨ��1�����趨; �ɼ���ʽ��05 ����ѹ�����룬06 ���������룬07 Ƶ�����룬08 0~1Vdc���룬09 RS485 ��������ɼ���ʽ
       03,01,15,  //���زɼ�ͨ��2�����趨; �����������ܺţ�0x06,��ַ��EF_0C~EF_1D,factory_gateway_set[12]~[29]
       03,01,15,  //���زɼ�ͨ��3�����趨;
       04,01,00,  //���زɼ�ͨ��4�����趨;
       25,01,00,	//���زɼ�ͨ��5�����趨;
			 	
//       0x15,0x31,0x31,0x35,0x2E,0x32,0x33,0x39,0x2E,0x31,0x33,0x34,0x2E,0x31,0x36,0x35,0x3A,0x30,0x30,0x35,0x30,0x32,//�Ų�ƽ̨
		   0x15,0x31,0x34,0x30,0x2E,0x31,0x34,0x33,0x2E,0x30,0x32,0x33,0x2E,0x31,0x39,0x39,0x3A,0x30,0x31,0x38,0x38,0x33,//����ƽ̨
				//��ʵ���ַ������ȣ�IP��ַ���˿ںš�ʵ���ַ�������Ϊ���ʱ��Ҫ�����
       //�������·����EF 10 00 1F 00 0F 1D 4D 43 47 53 53 54 52 3A 31 31 35 2E 32 33 39 2E 31 33 34 2E 31 36 35 3A 30 30 35 30 32 91 76 
       //�Ų�ƽ̨��д��IP��ַ�˿ڡ���15������21���ֽڣ��˴�Ϊ��115,239,134,165:00502 ��ǰ8���ֽ�ΪMCGS�Լ����룺4D 43 47 53 53 54 52 3A��MCGSSTR:����
			 //SDKƽ̨IP��ַ��140.143.023.199:01883 ����ƽ̨
			 //��WiFiƽ̨��ַ:183.159.037.034:03010 ;0x31,0x38,0x33,0x2E,0x31,0x35,0x39,0x2E,0x30,0x33,0x37,0x2E,0x30,0x33,0x34,0x3A,0x30,0x33,0x30,0x31,0x30,
				//����ԣ��ֽ���-8=ʵ��������ֽ���
				//�����������ܺţ�0x10,��ַ��EF_1F~EF_29,IP��ַ:�˿ں�Ϊfactory_gateway_set[30]~[51]
				
       0x10,0x4A,0x4D,0x57,0x47,0x49,0x44,0x30,0x31,0x3A,0x53,       //0x04,0x36,0x35,0x30,0x31
				    0x44,0x4B,0x30,0x30,0x30,0x30,0x0E,0x0D,0x0C,0x0B,
				    0x0A,0x09,0x08,0x07,0x06,0x05,0x04,0x03,0x02,0x01, 
				//	��ʵ���ַ������ȣ��ͻ���ID��,ʵ���ַ�������Ϊ���ʱ��Ҫ����factory_gateway_set[52]=0x04	
			 //д���ͻ���ID����15������30���ֽڣ��˴�Ϊ16���ֽڣ�JMWGID01:SMG0000��ǰ8���ֽ�ΪMCGS�Լ����룺��MCGSSTR:��4����������19������
			 //JMWGID02:SDK0000 ��16���ֽ� 0x4A,0x4D,0x57,0x47,0x49,0x44,0x30,0x31,0x3A,0x53,
			                              //0x44,0x4B,0x30,0x30,0x30,0x30
			 //�����������ܺţ�0x10,��ַ��EF_2A~EF_38,factory_gateway_set[52]~[82]
				
			 0x1B,0x76,0x31,0x2F,0x64,0x65,0x76,0x69,0x63,0x65,0x73,
            0x2F,0x6D,0x65,0x2F,0x72,0x70,0x63,0x2F,0x72,0x65,
            0x71,0x75,0x65,0x73,0x74,0x2F,0x2B,0x03,0x02,0x01,
            0x73,0x75,0x62,0x73,0x63,0x72,0x69,0x62,0x65,0x20,						
			 //	��ʵ���ַ������ȣ��������⡷,ʵ���ַ�������Ϊ���ʱ��Ҫ����factory_gateway_set[83]=0x1B
			 //	д���������⡷��20������40���ֽڣ�ǰ8���ֽ�ΪMCGS�Լ����룺��MCGSSTR:��4������;��24������;
			 //�����������ܺţ�0x10,��ַ��EF_39~EF_4C,factory_gateway_set[83]~[123]����������27���ֽ�v1/devices/me/rpc/request/+
			 
			 0x17,0x76,0x31,0x2F,0x64,0x65,0x76,0x69,0x63,0x65,0x73,
            0x2F,0x6D,0x65,0x2F,0x74,0x65,0x6C,0x65,0x6D,0x65,
            0x74,0x72,0x79,0x07,0x06,0x05,0x04,0x03,0x02,0x01,			 
			 //��ʵ���ַ������ȣ��������⡷,ʵ���ַ�������Ϊ���ʱ��Ҫ����factory_gateway_set[124]=0x11
       //д���������⡷��15������30���ֽڣ��˴�Ϊ17���ֽڣ�/JMWGID0000/SMG01��ǰ8���ֽ�ΪMCGS�Լ����룺��MCGSSTR:����
			 //�����������ܺţ�0x10,��ַ��EF_4D~EF_5B,factory_gateway_set[124]~[154]����������23���ֽ� v1/devices/me/telemetry
       
			 0x09,0x6A,0x69,0x61,0x6F,0x73,0x69,0x38,0x32,0x30,0x05,          //jiaosi502 
			      0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
			      0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x2A,0x2B,
			 //��ʵ���ַ������ȣ��û�����,ʵ���ַ�������Ϊ���ʱ��Ҫ����factory_gateway_set[155]=0x05
       //	д���û�������16������32���ֽڣ��˴�Ϊ5���ֽڣ�smg01��ǰ8���ֽ�ΪMCGS�Լ����룺��MCGSSTR:����
			 //jiaosi502 ��9���ֽ�  0x6A,0x69,0x61,0x6F,0x73,0x69,0x35,0x30,0x32
			 //jiaosi820 ��9���ֽ�  0x6A,0x69,0x61,0x6F,0x73,0x69,0x38,0x32,0x30
			 //�����������ܺţ�0x10,��ַ��EF_5C~EF_6A,factory_gateway_set[155]~[187]
			 
       0x00,0x73,0x6D,0x67,0x36,0x30,0x33,0x00,0x01,0x02,0x03,
			      0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,
						0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19
			 //��ʵ���ַ������ȣ��û������롷,ʵ���ַ�������Ϊ���ʱ��Ҫ�����
       //д���û������롷��16������32���ֽڣ��˴�Ϊ6���ֽڣ�smg603,(0x73,0x6D,0x67,0x36,0x30,0x33)��ǰ8���ֽ�ΪMCGS�Լ����룺��MCGSSTR:����
			 //�����������ܺţ�0x10,��ַ��EF_6B~EF_6D,factory_gateway_set[188]=0x06���Ϊ0����Ҫ����,factory_gateway_set[188]~[220]
			};//����33���ֽ�Ϊ3���ֻ����룬ÿ������Ϊ11���ֽ�
			
/*��������ƽ̨����*/
//ͨѶģ��ӿڿ��ƶ���			
#define RESET_CTL_ENABLE		 GPIO_SetBits(GPIOD, GPIO_Pin_13)
#define RESET_CTL_DISABLE		 GPIO_ResetBits(GPIOD, GPIO_Pin_13)
#define TERM_ON_CTL_ENABLE       GPIO_SetBits(GPIOD, GPIO_Pin_14)
#define TERM_ON_CTL_DISABLE      GPIO_ResetBits(GPIOD, GPIO_Pin_14)
#define WAKEUP_CTL_ENABLE        GPIO_SetBits(GPIOD, GPIO_Pin_15)
#define WAKEUP_CTL_DISABLE       GPIO_ResetBits(GPIOD, GPIO_Pin_15)

/*����Ϊ����Ա��վ�趨����8�飨8ҳ����ÿҳ�趨4����������ÿ��������4��ͨ����ÿ��ͨ��3������6���ֽڣ������ֽ���6*4*4=96���ֽ�
	3�������ֱ�Ϊ�������������ʱ����ʱ�䡢��������ʱ��,�ɼ���8��һ�飬*/
			
static u8 ctrlslave_param_set[4][24];
			//�������趨��ÿ���趨ҳ���趨4������������8���趨ҳ�棩���Կ�����Ϊ��λ��Ϊ4��,ÿ��12��������ÿ������ͨ��3��������
static s8 ctrlslave_param_flg[4]={0,0,0,0};
     //��¼���������������վ�·������Ĵ��������յ��������·��Ĳ�������Ӧ��վ���趨��־Ϊ3��ÿ����վ�·�һ�μ�1,���յ���վ�ظ���0.
static s8 ctrlslave_param_flgWX[4]={0,0,0,0};
     //��¼���������������վ�·������Ĵ��������յ��������·��Ĳ�������Ӧ��վ���趨��־Ϊ3��ÿ����վ�·�һ�μ�1,���յ���վ�ظ���0.
static u8 set_finish_flg=0;
     //����ȷ�ϱ�־����set_finish_flg=0������վ�����趨������������ɷ���0������������ctrlslave_param_flg[i]=0��i=0~3�������ߡ����߹���
static u8 set_finish_flgEF=0;
static u8 set_finish_flgEE=0;
static u8 set_finish_flgF1=0;
static u8 set_finish_flgF2=0;
static u8 set_finish_flgF3=0;
static u8 slave_xiabiao_I=0,slave_xiabiao_J=0;//���ߡ����߹���
static u8 YX_Slave_ID=0;
static u8 WX_Slave_ID=0;

static u8 first_xiabiao_I=0;//��¼��ǰҳ���趨��1����վ�趨����վ��ַ;����
static u8 firstwx_xiabiao_i=0;//��¼��ǰҳ���趨��1����վ�趨����վ��ַ;����

static u8 slave_set_finish=0;//����set_slave_param�������ж������·���վ�趨�����Ƿ�ȫ����ɣ�û����ɲ�����0
static u8 slave_set_finishWX=0;//����set_slave_paramWX�������ж������·���վ�趨�����Ƿ�ȫ����ɣ�û����ɲ�����0
static u16 slave_param_addr=0;//����16λ�����ĵ�ַ�����ߡ����߹��ã�
static void set_slave_param(void);//�����趨��վ�������������ߣ�
static void set_slave_paramWX(void);//�����趨��վ�������������ߣ�
static u8 slave_set_flg=0;
   //�������·����趨ȷ�ϱ�����slave_set_flg!=0��ʾ����ȷ�����ã����ؿ�������վ�·������ˡ�
   //slave_set_flg=1�����������趨;slave_set_flg=2�ɼ��������趨
static u8 slave_set_flgWX=0;
   //�������·����趨ȷ�ϱ�����slave_set_flgWX!=0��ʾ����ȷ�����ã����ؿ�������վ�·������ˡ�
   //slave_set_flgWX=1�����������趨;slave_set_flgWX=2�ɼ��������趨
static s8 cjqslave_param_flg[8]={0,0,0,0,0,0,0,0};
   //��¼����������ɼ���������վ�·������Ĵ��������յ��������·��Ĳ�������Ӧ��վ���趨��־Ϊ3��ÿ����վ�·�һ�μ�1,���յ���վ�ظ���0.
static s8 cjqslave_param_flgWX[8]={0,0,0,0,0,0,0,0};
   //��¼����������ɼ���������վ�·������Ĵ��������յ��������·��Ĳ�������Ӧ��վ���趨��־Ϊ3��ÿ����վ�·�һ�μ�1,���յ���վ�ظ���0.
static u8 cjqslave_param_set[8][36];
  //�ɼ����Ϳ������ɼ�ҳ���趨8���ɼ��������������8��ҳ�棩���Բɼ����������Ϊ��λ��Ϊ8�飬ÿ��18��������ÿ���ɼ�ͨ��3��������6ͨ���ɼ����������ߡ����߹���
static u8 set_slave_Index=0;//���߼�¼�·���վ��������վƫ�Ƶ�ַ����վ��ַ=first_xiabiao_I+set_slave_Index
static u8 set_slave_IndexWX=0;//���߼�¼�·���վ��������վƫ�Ƶ�ַ����վ��ַ=firstWX_xiabiao_i+set_slave_IndexWX
static u8 cjym_ID=0;//�ɼ���ҳ��ID
static u8 cjym_old_ID=0;
static u8 kzym_ID=0;//������ҳ��ID
static u8 kzym_old_ID=0;

static u8   send_flg      = 0x00;
//���������־λ��û��ͨ��ģ�飨����2��ʱ send_flg=0x00
//send_flg=0x01ͨ��ģ���ʼ����0x02��0x03Ϊ������ƽ̨�ϱ���ATָ�������ָ��,0x04��0x05���ڻظ�ƽ̨�·�����ָ���ATָ�������ָ�
static u8 send_message_type=0x00;
//0x01Ϊͨ��ģ���ʼ����atָ�����ͣ�0x02Ϊ��ƽ̨�ظ�����ָ������ͣ�0x03Ϊ��ƽ̨�ϱ����ݵ����ͣ�0x04����Ϊ���ٻظ�����ƽ̨�Ŀ���ָ��
//���յ�ͨ��ģ��ERROR����ʱʹ�á�
static u8 next_atdata_flg=0x00;//�ڽ��յ�ͨ��ģ���OK�ظ�ʱ����һ��Ҫִ�еĳ���	
			
#define CMD_WAIT_TIME    6000  //��CDMA�����������ȴ�ʱ�䣬ms��λ
#define DATA_WAIT_TIME   5000  //��CDMA���������������ȴ��ظ�ʱ�䣬ms��λ
#define Ethernet_WAIT_TIME   300  //��Ethernetģ�鷢����������ȴ�ʱ�䣬ms��λ
#define INTER_SEND       5000  //ÿ���ϱ��ļ��
#define INTER_MESS       150   //5ms(��20ms����0x14 TCP��   ������Ϣ֮��ȴ�10ms)			

static void Net_connect_init(void);//ƽ̨���ӳ�ʼ��

static void cdma_tcp_init(void);//����TCP/IP����
//static void cdma_mqtt_sdk_init(void);//����mqtt_sdk����			
//static void cdma_other_init(void);//����other����

static void gprs_tcp_init(void);//�ƶ�TCP/IP���ӣ��ú��������� �ƶ�mqtt���� gprs_mqtt_init���ƶ�sdk���� gprs_sdk_init	
static void gprs_other_init(void);//�ƶ�other����

static void Ethernet_tcp_init(void);//����TCP/IP����
static void Ethernet_mqtt_sdk_init(void);//����mqtt_sdk����			
static void Ethernet_other_init(void);//����other����

static void send_platform(void);//��ƽ̨��������

static void cdma_tcp_send(void);//����TCP/IP��������;�����ƶ�TCP/IP�������� gprs_tcp_send
static void cdma_mqtt_send(void);//����mqtt��������;�����ƶ�mqtt�������� 	gprs_mqtt_send		
static void cdma_sdk_send(void);//����sdk��������	;�����ƶ�sdk�������� 	gprs_sdk_send	
//static void cdma_other_send(void);//����other��������

static void gprs_other_send(void);//�ƶ�other��������

static void Ethernet_tcp_send(void);//����TCP/IP��������
static void Ethernet_mqtt_send(void);//����mqtt��������			
static void Ethernet_sdk_send(void);//����sdk��������			
static void Ethernet_other_send(void);//����other��������

static void cdma_cmd_receive(u8 len,u8 *pBuf);//����TCP/IP����CDMA����ش�����
static void cdma_tcp_receive(u8 len,u8 *pBuf);//����TCP/IP����ƽ̨���ݴ�������;������ �ƶ�TCP/IP����ƽ̨���� gprs_tcp_receive
static void cdma_SMS_receive(u8 len,u8 *pBuf);//����TCP/IP���ն��Ŵ�����;������ �ƶ�TCP/IP���ն��Ŵ����� gprs_SMS_receive
static void cdma_mqtt_receive(u8 len,u8 *pBuf);//����mqtt����ƽ̨����;�������ƶ�mqtt����ƽ̨���� gprs_mqtt_receive			
static void cdma_sdk_receive(u8 len,u8 *pBuf);//����sdk����ƽ̨����;�������ƶ�sdk����ƽ̨���� 	gprs_sdk_receive		
//static void cdma_other_receive(u8 len,u8 *pBuf);//����other����ƽ̨����

static void gprs_cmd_receive(u8 len,u8 *pBuf);//�ƶ�TCP/IP����CDMA����ش�����
static void gprs_other_receive(u8 len,u8 *pBuf);//�ƶ�other����ƽ̨����

static void Ethernet_tcp_receive(u8 len,u8 *pBuf);//����TCP/IP��������
static void Ethernet_mqtt_receive(u8 len,u8 *pBuf);//����mqtt��������			
static void Ethernet_sdk_receive(u8 len,u8 *pBuf);//����sdk��������			
static void Ethernet_other_receive(u8 len,u8 *pBuf);//����other��������

static void wg_reply_cmd(void);//�ظ�ƽ̨���������ѯ
static void jm_platform_reply(void);//Ϊ��ƽ̨�·������������ٵõ�����״̬������
static unsigned char jm_reply_cmd[16]= "{H2100:0000}";//���ؼ���ƽ̨�·��Ŀ�������

static void cdma_tcp_reply(void);//����TCP/IP�ظ�ƽ̨���������ѯ�������ƶ�TCP/IP�ظ�ƽ̨���������ѯgprs_tcp_reply
static void cdma_mqtt_reply(void);//����mqtt�ظ�ƽ̨���������ѯ�������ƶ�mqtt�ظ�ƽ̨���������ѯgprs_mqtt_reply			
static void cdma_sdk_reply(void);//����sdk�ظ�ƽ̨���������ѯ	������	�ƶ�sdk�ظ�ƽ̨���������ѯ	gprs_sdk_reply
static void cdma_other_reply(void);//����other�ظ�ƽ̨���������ѯ

static void gprs_other_reply(void);//�ƶ�other�ظ�ƽ̨���������ѯ

static void Ethernet_tcp_reply(void);//Ethernet TCP/IP�ظ�ƽ̨���������ѯ
static void Ethernet_mqtt_reply(void);//Ethernet mqtt�ظ�ƽ̨���������ѯ			
static void Ethernet_sdk_reply(void);//Ethernet sdk�ظ�ƽ̨���������ѯ			
static void Ethernet_other_reply(void);//Ethernet other�ظ�ƽ̨���������ѯ

static u8   cmd_flg       = 0x03; //��ʼ��ƽ̨���Ӳ����־λ��0xFF��ʾ�������
static u8   module_send_flg    = 0;//cdma_tcp_send()���ϱ��ı�־��0xFFΪĬ��ֵ

/*���س�ʼ������*/			
static void wg_init_readflash(void);//���س�ʼ����flash
static u8   net_connect_count=0;

/*������ƽ̨�������ݶ���*/
static unsigned char real_send[256]; //�ϱ�����
static u8 sendnum_mflg=0x00 ; //sendnum_mflg ���Ʋɼ������������ϱ�����˳��,sendnum_mflg�Ǽ�¼�ڼ����ϱ�
static void send_at_cdma(unsigned char send_data_len[],u8 at_cmd_len);		//�������ݵ�atָ�ͨѶģ�飬CDMA��at_cmd_len=21��GPRS:at_cmd_len=15 
static void send_at_gprs(unsigned char send_data_len[],u8 at_cmd_len);		//�������ݵ�atָ�ͨѶģ�飬CDMA��at_cmd_len=21��GPRS:at_cmd_len=15 
static void send_data_module(unsigned char send_data[],unsigned char send_data_len[] );		//�������ݵ�CDMA;ֻ����send_at_cdma����ִ����ɲ���ִ��
static void send_Ethernet_module(unsigned char send_data[],unsigned char send_data_len[] );		//�������ݵ�Ethernetģ��;

static void alter_send(u8 n,u8 flg);
static void alter_hand_auto(u8 n,u8 flg);
static void chge_coltsnd(u8 flg);
static unsigned char at_send_cdma[128]={"AT^IPSENDEX=1,2,094\r\n"}; //CDMAΪ"AT^IPSENDEX=1,2,094\r\n" ;��ƽ̨����ATָ��,MQTTΪ94��TCBΪ75��(��ʱ�趨��
static unsigned char at_send_gprs[128]={"AT^SISW=0,094\r\n"}; //CDMAΪ"AT^IPSENDEX=1,2,094\r\n" ;��ƽ̨����ATָ��,MQTTΪ94��TCBΪ75��(��ʱ�趨��
static unsigned char reply_xiafa_data[128];//��ƽ̨�ظ��յ������ݣ�reply_xiafa_cmd��ŵ��ǻظ�����

//�ֻ����Ŷ���
static bool send_mess     = false;  //���Ͷ����ϱ���־	
static u8 main_call[11]= {0x31,0x33,0x39,0x30,0x35,0x37,0x31,0x34,0x36,0x37,0x32};// {"13905714672"}
static u8 voice_call[11]={0x31,0x33,0x33,0x37,0x36,0x38,0x31,0x32,0x39,0x31,0x30};// {"13376812910"}
static u8 third_call[11]={0x31,0x35,0x31,0x36,0x38,0x34,0x31,0x33,0x33,0x32,0x31};//{"15168413321"}
static bool is_number(u8 *sorce,u8 len);//����ʹ��
static u8 match_str(u8 *dst,u8 dst_len,u8 *sor,u8 sor_len);//���ƥ�䷵����ͬ�ε����һλ�ĵ�ַ�����򷵻�0

void initialRealSendBuff(void);//��ʼ��ƽ̨�ϱ���������
static void handlecmd(u8 *reply_xiafa_cmd,u16 len);//����ƽ̨�·�ָ��
static void set_ctrl_data(u8 *_addr,u8 *_data,u8 len);		//len==2

//SDK����
static char SDK_topicString_pub1[128]= "v1/devices/me/rpc/response/";//27���ַ���MQTT���͵�����/gateway/v01/pub/EDF0F8AF681C4D30AAC4
void mqtt_publish1(unsigned char *real,int pub_len);//��������1
static u16 ctrl_adrr,ctrl_cmd,offset_addrX,offset_addrY;
static u8 ctrl_key[4],ctrl_value[4];
static char *mqtt_bcmd;//mqtt��������ָ��
static	u8 mqtt_bcmdxb;//mqtt����ָ���±�					
static	u8 sdk_ctrl_reply[128];
static	u8 sdk_len,sdk_bcmdxb;
static	u8 send_count=0;
//mqtt����
void mqtt_connect(void); 
void mqtt_subscribe(void);//MQTT���ⶩ��
void mqtt_publish(unsigned char *real,int pub_len);//MQTT���ⶩ���ͼ�����
static u8 mqtt_len;

/*�ú�����Ҫ����ƽ̨��2�ֽ���ʼ��ַ(keyX_addr)��2�ֽڵ�����int������(value)����������nת��Ϊ��key1:value1��key2:value2��...
    keyn:valuen ��ASC��,����װ��{key1:value1��key2:value2��... keyn:valuen}�͵�key_data_send������*/
 static u8 made_keyX_value(u16 keyX_addr,u8 *value,u8 n,u8 *key_data_send);
 
/*�ú�����Ҫ����ƽ̨��2�ֽ���ʼ��ַ(keyX_addr)��2�ֽڵ�����int������(value)����������nת��Ϊ��{"key1":value1}��{"key2":value2}��...
    keyn:valuen ��ASC��,����װ��[{"key1":value1}��{"key2:value2}��... {"keyn":valuen}]�͵�key_data_send������*/
 static u8 made_keyX_value4(u16 keyX_addr,u8 *value,u8 n,u8 *key_data_send);
 
/*�ú�����Ҫ����ƽ̨��2�ֽ���ʼ��ַ(keyX_addr)��2�ֽڵ�����int������(valueN)ת����4�ֽ���������(valueF)����������nת��Ϊ��key1:valueF1��key2:valueF2��...
    keyn:valueFn ��ASC��,����װ��{key1:valueF1��key2:valueF2��... keyn:valueFn}�͵�key_data_send������;valueFN��ֵΪXXXX.XX*/
 static u8 made_keyX_valueF(u16 keyX_addr,u8 *value,float *valueF_k_a,u8 n,u8 *key_data_send);
 
/*�ú�����Ҫ����ƽ̨��2�ֽ���ʼ��ַ(keyX_addr)��2�ֽڵ�����int������(valueN)ת����4�ֽ���������(valueF)����������nת��Ϊ��{"key1":valueF1}��{"key2":valueF2}��...
    {"keyn:valueFn"} ��ASC��,����װ��{{"key1":valueF1}��{"key2":valueF2}��... {"keyn":valueFn}}�͵�key_data_send������;valueFN��ֵΪXXXX.XX*/
 static u8 made_keyX_valueF4(u16 keyX_addr,u8 *value,float *valueF_k_a,u8 n,u8 *key_data_send); 
 
//433MHZ���߶���
static u8  SI4463_Channel  =  0;
//static void	Get_WX_Channel(void);//433MHZ�����ŵ�ѡ�񣬹���32���ŵ�
static void Clear_Buf(u8 buf[], u8 length, u8 data);
extern u8 	SI4463_RxBUFF[];
extern u8 	SI4463_RxLenth;
extern u8 	Int_BUFF[]; 
static void SI4463Receive(u8 len,u8 *pData);
static void SI4463_SENDZZ(void);
static u8 SI4463_TxBUFF[128];
static u8  WX_Query_Flag = CONTROLLERS_CMD;
static u8  Query_Index_ControllerWX = 0;
static u8  Query_Index_CollectorWX = 0;
static u8  Query_IndexZZ_C_WX = 0;
static u8  Query_IndexZZ_K_WX = 0;
static u8 Query_Wired_WirelesWX=0;//����
//static u8  Query_Index_setX  = 0;
static u8 WX_len=0;
static u8 CSH_countWX=0;//���߳�ʼ�����=1
       u8 close_433MHZ=1;//��ʹ��433MHZͨ�Ż�433MHZͨ�ų����⣬��close_433MHZ=0

//��վ��ַ�������ŵ��趨
/*set_slaveID_channel[]={�ϵ�ַ�� �µ�ַ�� ���ŵ��� ���ŵ��� ��վ�²ɼ��������� ��վ�¿����������� �²ɼ�����վʼ��ַ��
					 �¿�������վʼ��ַ;�²ɱ�����ַ;�²ɱ�������;�¿ر�����ַ;�¿ر�������;�¿ص�����ַ;�¿ص�������;
					 �ɼ�������3��λ���ɼ���վ��ַ;�ɼ�������3��λ��������վ��ַ};
�ر�˵�����ɼ�����Ϊ��վ�����ô���3��������λ�����ɼ���λ����վ��ַ�������λ����վ��ַͨ��������ͬ;Ҳ���Էֱ��趨�����������*/
static u8 set_slaveID_channel[16]={240,0,65,1,0,0,0,0,0,0,0,0,0,0,0,0};
static u8 slaveID_channel_flg=0;//�趨��ɱ�־���յ���������վ��ַ�趨���ֵ5�������վ�趨ֵΪ0
static u8 slaveID_channel_flgWX=0;//�趨��ɱ�־���յ���������վ��ַ�趨���ֵ5�������վ�趨ֵΪ0
 __attribute__((section("NO_INIT"),zero_init)) static u8 online_slaveID[64];//����������վ�ĵ�ַ
 __attribute__((section("NO_INIT"),zero_init)) static u8 online_slaveID_WX[64];//����������վ�ĵ�ַ
 __attribute__((section("NO_INIT"),zero_init)) static u8  ZZ_temp_stateYX;
 __attribute__((section("NO_INIT"),zero_init)) static u8  ZZ_temp_stateWX;

//�������ɼ����ݶ��忪ʼ
extern __IO uint16_t ADC_ConvertedValue[];
static void wgcollector_data(void);  //�������ɼ�����
 u8  ReadDataCNT = 0;
static u8  TD_param_num=0;
__attribute__((section("NO_INIT"),zero_init)) static u8  wgcollector_data_buff[16];
static void startadc(void);
#define MEASURE_PERIOD          800
#define TX5_CMD_PERIOD          2000
#define IO_CMD_PERIOD          3000

/*ADC�ɼ���������*/
u16 Get_Adclvbo(u8 TD_Xnumber,u16 TD_xiaxian,u16 TD_shangxian);	//�ɼ�100�����ݣ�����ȡ40~60ƽ��ֵ
u16 First_Getaverage(u8 td_xnumber,u8 maxlvbo_xnumber,u16 temp_adc);//��ʼ��ƽ��ֵ
u16 TD_Getaverage(u8 td_xnumber,u8 tdlvbo_xnumber,u16 temp_xadc,u8 tdcycle_xi);//ADCͨ����ƽ��ֵ
u16 SelectTD(u8 TD_number);//ѡ��ͨ�����ɼ�����ͨ����ֵ
 /*��ͨ��ƽ��ֵ����*/
const u8 MaxTD_number= 3;		  //2��ADC�ɼ�ͨ��
const u8 Maxlvbo_number= 50;	  //ƽ�������������

static u8 TDlvbo_number[MaxTD_number]={50,50,50};	//ͨ��0��50��������ƽ��ֵ��4��/��
		 								//ͨ��1��50��������ƽ��ֵ��4��/��
static  u16 Adc_average[MaxTD_number][Maxlvbo_number]; //ADC��ƽ��ֵ���飬�����ʷ�ɼ�����
static u8 tdcycle_i[MaxTD_number]={0,0,0}; //ѭ���������飬0~TDlvbo_number[i]-1��iΪͨ���ţ�i=0��1��....
static u8 First_adc_average[MaxTD_number]={0,0,0}; //�״��˲�����First_adc_average=0ʱ������First_Getaverage(u8 TD_number,u8 Maxlvbo_number,u16 temp_adc)
                               //�������  TD_Getaverage(u8 TD_number,u8 TDlvbo_number[TD_number],u16 temp_adc,u8 tdcycle_i[TD_number])
static u16 collector_temp,temp_adc,temp_flow,temp_level; //��������������ݴ����
static u16 dp_temp_level;
static u16 dr_temp_level,lag_value=0;
static  u32 PA1_pulse_time;
static u16 temp_level_up=1500;
static u8   TD_number;//ADCͨ����
//�������ɼ����ݶ������
//����1��5Ӧ�ó�����
static void IOxh_send_cmd(void);
const u8 uart1_cmd_csb[1]={0x55};//������Һλ��
//static u8 uart1_data_buf[2]={0x00,0x00};
static void uart5_send_cmd(void);
const u8 uart5_cmd_csb[8]={0x01,0x03,0x00,0x0A,0x00,0x02,0xE4,0x09};//�����������ƣ��������
const u8 uart5_cmd_dc[8]={0x01,0x03,0x00,0x03,0x00,0x02,0x34,0x0B};//�Ϻ�����ư�������
const u8 uart5_cmd_LEL[8]={0x01,0x03,0x00,0x06,0x00,0x01,0x64,0x0B};//��ȼ���屨������վ��ַ��0x01��������Ѷ��ͨ���ӿƼ����޹�˾
static u8 uart5_data_buf[6]={0x00,0x00,0x00,0x00,0x00,0x00};
//����5�ഫ��������;�ɼ���ʽ=22
static u8 switch_cmd_addr=0;
const u8 uart5_cmd_PM[8]={0x02,0x03,0x00,0x00,0x00,0x03,0x05,0xF8};//(PM1.0+PM2.5+PM10)����������ӿƼ�
static u8 data_buf_PM[6];
const u8 uart5_cmd_FSFX[8]={0x01,0x03,0x00,0x01,0x00,0x03,0x54,0x0B};//(����2�ֽ����ͣ�+���٣�4�ֽڸ��㣩)��ַ��0x01����о���Զ����Ǳ����޹�˾
static u8 data_buf_FXFS[4];
static union
{
      float  wind_speed_float;//���ٸ�����
      u8 wind_speed_int[4];
}data_buf_FS;
u16 report_last_rain=0;

//��������
static u8 halt_module=0;
static u8 halt_RxReport2=0;
static u16 dword_asc_hex(u8 *dword_asc);
static void bytelen_to_asc(u8 *zfc,u8 len);//��1���ֽ�ʮ��������ת��Ϊʮ������ֵ��asc��
static void byte_to_asc(u8 byte_hex,u8 *byte_asc);//��1���ֽ�ʮ��������ת��Ϊ��Ӧasc��
static void cycle_cmd(void);//���Ը����г����¼��Ƿ�����
static u8 error_num=0;//ATָ��ִ�д���Ĵ���
#define MAX_ERROR_NUM    0x05   //ATָ�������������ظ�ִ�еĴ���
#define OPEN_OUT24V		         GPIO_SetBits(GPIOE, GPIO_Pin_7)
#define CLOSE_OUT24V		         GPIO_ResetBits(GPIOE, GPIO_Pin_7)
static u8 RxReport2_step_flg=0;//RxReport2_step_flg!=0��ʾ�յ��������Ѵ���
static u8 RxReport2_useless_count=0;//��¼��Ч����Ĵ���������250�������³�ʼ��ƽ̨ͨ��ģ��
static u8 RxReport1_step_flg=0;//RxReport1_step_flg!=0��ʾ�յ��������Ѵ���
static char message_len_char1[3];
static u8 send_message_len1;
static char message_len_char2[3];
static u8 send_message_len2;
static u8 RxReport2_len=0;
static u8 RxReport3_len=0;
static u8 RxReport4_len=0;
static u8 RxReport5_len=0;
static u8 cj_init_value[16]={0x00,0x00,0x4A,0x0C,0x42,0x44,0x06,0x01,0x62,0x02,0x00,0x00,0x00,0x00,0x00,0x00};//Ϊ�˳�ʼ��ֵ��ʾ0�������в����������
/* ���ն�=X/100+0;����ʪ��=X/524.29-6;�����¶�=X/372.96-46.85;����ˮ��=X/10.43-25.12;�����¶�=X/8.69-70.14(-40~80��);������̼=X/1-0;����=X/1-0*/
static u8 kzcj_init_value[16]={0x06,0x01,0x06,0x01,0x06,0x01,0x06,0x01,0x06,0x01,0x06,0x01,0x06,0x01,0x06,0x01};//Ϊ�˳�ʼ��ֵ��ʾ0,262~1305��Ӧ0~10A
static u16 send_Collectors_time=500;
       u16 level1_temperature;
static u16 absolute_pressure_upper;
static u16 absolute_pressure_lower=9950;
__attribute__((section("NO_INIT"),zero_init))  u16 absolute_pressure_zero[50];
static u16 pressure_correct=20;
//���������忪ʼ
static union
{
      float k_b_float[65][8*2];//zero_rang.k_b_float[65]
      u8 array_k_b[65*8*2][4];
}zero_rang;

static u8 float_to_string(float data, u8 *str);
//�������������
//USART3 DMAͨ�Ŷ���
static void WriteDataToDMA_BufferTX1(uint16_t size);
static void WriteDataToDMA_BufferTX2(uint16_t size);
static void WriteDataToDMA_BufferTX3(uint16_t size);
static void WriteDataToDMA_BufferTX4(uint16_t size);

//Ƶ�ʼ�ⶨ��
__attribute__((section("NO_INIT"),zero_init)) u16  TIM2_FrequencyPA0[61];//����TIM2����ͨ��1��CC1������ʽ��������̼
__attribute__((section("NO_INIT"),zero_init)) u16  TIM2_FrequencyPC0[61];//EXTI_Line0�жϷ�ʽ�����ն�
__attribute__((section("NO_INIT"),zero_init)) u16  TIM2_FrequencyPA1[61];//EXTI_Line1�жϷ�ʽ,������ʪ��
                                              u8   freq_I;
       u8 TIM5_pulsePA1_NUM =32;
       u8 level_num_err=0;//���20�η��ͼ���������û���յ�TIME5D1�ж�,��ϵͳ��������TIME5�жϳ�����level_num_err����;��IOxh_send_cmd�����м�1
__attribute__((section("NO_INIT"),zero_init)) u32  TIM5_pulsePA1[50];//PA1�����ȣ���λ��us
                                       static u32 TIM5_sort_pulsePA1[50];//PA1�����ȣ���λ��us
                                       static  s16 temp_levelS;      
//������վ�趨��������
static u8 formula_param_finish[22];
static u8 formula_read_finishF1=1;
static u8 slave_CJparam_set[65][36];
static u8 collector_param_finish[11];
static u8 collector_read_finishF2=1;
static u8 slave_KZparam_set[32][24];
static u8 ctrl_param_finish[4];
static u8 ctrl_read_finishF3=1;
//��wifi MD5����
static u8 md5_devId_devKey[64]={"testCode1-DTU-20170925-000068e8f64bff906049dea06e2646af9701a7"};
//static u8 md5_devId_devKey[68]={"testCode1-DTU-20170925-000068e8f64bff-9060-49de-a06e-2646af9701a7"};
static u8 md5_password[16];
static u8 md5_password_ascii[32];
//static u8 test[100];
void Period_Events_Handle(u32 Events)
{
	 u8 i;
	if(Events&SYS_INIT_EVT)
	{	
		if(RCC_GetFlagStatus(RCC_FLAG_PORRST)!=RESET) 
		 {
			 //�ɼ������������Ĳɼ�������ʼ����ʼ
			for(i=0;i<=31;i++)
		  {
			 memcpy(Collectors[i],cj_init_value,16);			
			}
      for(i=32;i<=63;i++)
		  {
			 memcpy(Collectors[i],kzcj_init_value,16);			
			}
      memcpy(Collectors[64],cj_init_value,16);
      //�ɼ������������Ĳɼ�������ʼ������			
			memset(Controllers,0x00,sizeof(Controllers));
  		memset( set_param_flag, 0, sizeof( set_param_flag));
			memset(ZZ_Wireles_flag, 0, sizeof(ZZ_Wireles_flag));
      memset(ZZ_Wired_flag, 0, sizeof(ZZ_Wired_flag));			
		  memset(crtl_cmd_num, 0, sizeof(crtl_cmd_num));
			memset(crtl_cmd_numWX, 0, sizeof(crtl_cmd_numWX)); 
			memset(online_slaveID, 0, sizeof(online_slaveID));
      memset(online_slaveID_WX, 0, sizeof(online_slaveID_WX));
			memset(hand_auto_flg, 0, sizeof(hand_auto_flg)); 
			memset(wgcollector_data_buff, 0, sizeof(wgcollector_data_buff)); 
			memset(Adc_average, 0, sizeof(Adc_average)); 
			memset(TIM2_FrequencyPA0, 0, sizeof(TIM2_FrequencyPA0));
			memset(TIM2_FrequencyPC0, 0, sizeof(TIM2_FrequencyPC0)); 
			memset(TIM2_FrequencyPA1, 0, sizeof(TIM2_FrequencyPA1)); 
      memset(TIM5_pulsePA1, 0, sizeof(TIM5_pulsePA1));
			for(i=0;i<sizeof(absolute_pressure_zero);i++)
			{
				absolute_pressure_zero[i]=9950;
			}  
     for(i=0;i<sizeof(TIM5_pulsePA1);i++)
			{
				TIM5_pulsePA1[i]=9695;//0x9696us=6553.5mm,2�ֽ�Һλ�������9695us=1648.15mm
			}        				
	/*�Բ�����վ��������Ҫ�ǿ�������վ�Ƿ���ڣ�����������ڷ��Ϳ�������ɼ����Ϳ�������64����վ������������ѯ����Ӱ�졣
		ͨ��������������ѯ�������������Ƿ���ڣ���������������ˢ���ٶȼ��������������·��ӿ죬3��ʾ��������ѯ����û��
		�յ��ظ�������Ϊ�ÿ����������ڡ�������ѯ���Ƿ��ͣ�������������·���һ���յ��ظ����������Ӧ��Ԫ������3*/			
			memset(init_cmd_numYX, 10, sizeof(init_cmd_numYX));
			memset(init_cmd_numWX, 3, sizeof(init_cmd_numWX));			
			ZZ_temp_stateYX=0;
			ZZ_temp_stateWX=0;			
		 }
		RCC_ClearFlag();		 
    wg_init_readflash();
    initialRealSendBuff();
//		Get_WX_Channel();//�����ŵ�ѡ���ʼ�� 
    if(close_433MHZ!=0)
      {
			 SI4463_Init();
			 Start_timerEx(WX_SENDZZ_EVT,3000);
			}//��������������Ӧ�������
//    RXENABLE3;//����		
    RXENABLE4;		
    RXENABLE5;		
    ZZ_Wired_flag[64]=factory_gateway_set[11];//�������زɼ����ܣ���=1			
		cmd_flg=0x03;
		send_flg = 0x00;
    OPEN_OUT24V;
    PCout(3)=0;			
    if(factory_gateway_set[1]==4)
		{
		 MD5_CTX md5;  
     MD5Init(&md5);
		 memset( md5_devId_devKey, 0, sizeof( md5_devId_devKey));			
     memcpy(md5_devId_devKey,factory_gateway_set+156,factory_gateway_set[155]);
     memcpy(md5_devId_devKey+factory_gateway_set[155],factory_gateway_set+189,factory_gateway_set[188]);
     MD5Update(&md5,md5_devId_devKey,strlen((char *)md5_devId_devKey));  
     MD5Final(&md5,md5_password);
     for(i=0;i<=15;i++)
      {
			 byte_to_asc(md5_password[i],md5_password_ascii+2*i);
			}			
		 }			
		Start_timerEx(WG_SENDZZ_EVT,3000);		
		Start_timerEx(NET_INIT_EVT,3000);
		if(factory_gateway_set[11]!=0)
    {
				u16  logic_ture;
				if((factory_gateway_set[12]>=6&&factory_gateway_set[12]<=7)||factory_gateway_set[12]==10)//��������Ƶ���ź�����ܽ�PC0����20k���裻ͨ��0�����գ�=10 cps120ѹ������
				{
					LIGHT_DTA_R;//���ն����ݹܽ�PC0��������
					GPIOC->ODR=1<<0;//PC0�ߵ�ƽ����	�������޷�����ߵ�ƽ			
					if(factory_gateway_set[12]==7)//PC0Ƶ������
					{
						EXTI->PR=1<<0;  //���LINE0�ϵ��жϱ�־λ
						EXTI->IMR|=1<<0;//������line0�ϵ��жϣ��ж�ʹ��
						EXIT0_Enable(ENABLE);			 
					}			
				}
				logic_ture=factory_gateway_set[15]==6||factory_gateway_set[15]==7||factory_gateway_set[15]==10||factory_gateway_set[15]==12;//PA1
				logic_ture=logic_ture||factory_gateway_set[15]==23;
				if(logic_ture!=0)
					//��������Ƶ���ź�����ܽ�PA1����20k���裻ͨ��1��������ʪ�ȣ�=10 cps120ѹ�����룻ͨ��1��������ʪ�ȣ�=12 ��ѹҺλ����
				{
					DATA_INPUT;//	������ʪ�����ݹܽ�PA1,����/����ģʽ���룬��PxODR�Ĵ���Լ��
					GPIOA->ODR=1<<1;//PA1�ߵ�ƽ����	�������޷�����ߵ�ƽ	
					if(factory_gateway_set[15]==7||factory_gateway_set[15]==23)//PA1Ƶ������
					{
						EXTI->PR=1<<1;  //���LINE1�ϵ��жϱ�־λ
						EXTI->IMR|=1<<1;//������line1�ϵ��жϣ��ж�ʹ��
						EXTI1_Enable(ENABLE);					 
					}
				}		
				
				if((factory_gateway_set[24]>=6&&factory_gateway_set[24]<=7)||factory_gateway_set[24]==10)//��������Ƶ���ź�����ܽ�PA0����20k����
				{
					CPS_DTA_R;//	������̼���ݹܽ�PA0����
					GPIOA->ODR=1<<0;//PA0�ߵ�ƽ����	�������޷�����ߵ�ƽ	
					if(factory_gateway_set[24]==7)//PA0ΪƵ������
					{
					 TIM_ClearITPendingBit(TIM2,TIM_IT_CC1); //���TIM2�ϵ�TIM_IT_CC1�жϱ�־λ���涨PA0
					 TIM_ITConfig(TIM2,TIM_IT_CC1,ENABLE);//������TIM2�ϵ�TIM_IT_CC1���жϣ��ж�ʹ��
					}			   
				}
			logic_ture=factory_gateway_set[12]==7||factory_gateway_set[15]==7||factory_gateway_set[24]==7;
      logic_ture=logic_ture||factory_gateway_set[15]==23;				
			if(logic_ture)//��Ƶ�����룬��TIM2����Ӧ���ж�
				{
					TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
					TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE );
//					TIM_ITConfig(TIM2,TIM_IT_CC1,ENABLE);
					TIM_Cmd(TIM2,ENABLE);
				} 
      if(factory_gateway_set[15]==18)	//�����ȼ�⣬��λ��us				
				{
					DATA_INPUT;//	������ʪ�����ݹܽ�PA1,����/����ģʽ���룬��PxODR�Ĵ���Լ��
					GPIOA->ODR=0<<1;//PA1�͵�ƽ����	�������޷���֤�͵�ƽ			
					TIM_ClearITPendingBit(TIM5, TIM_IT_CC2);					
          TIM_ClearITPendingBit(TIM5,TIM_IT_Update);
					TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE );
					TIM_ITConfig(TIM5,TIM_IT_CC2,ENABLE);					
					TIM_Cmd(TIM5,ENABLE);					
				}
      		
      startadc();      			
		  Start_timerEx(WGCOLLECTOR_DATA_EVT,2500);				
		  Start_timerEx(TX5_CMD_EVT,2600);      
			Start_timerEx(IO_XUNHUAN_CMD_EVT,5000);			 				
		}//�����вɼ��������ɼ�����
		ctrl_wx_j=0;
		
    Start_timerEx(CYCLE_CMD_EVT,65000);//�����¼����������Ƿ�������  
	}
  YX_LED_TOGGLE;  //ϵͳ����ָʾ����˸
	
	if(Events&RX4_DELAY_EVT)//������ͨ��;PC10--TX4,PC11--RX4,PB9--�л�����;AI02��BI02����4
	{		
		RxFlag4=1;    	
		RxReport4(RecLen4,ReceiveData4);		
	}

	if(Events&RX3_DELAY_EVT)//��������վͨ�ţ�PD8--TX,PD9--RX,PD10--�л����ƣ�����ͨ�ţ�AI01��BI01 ����3
	{ 
//		if(RxReport3_len!=0x15){test_count++;}
		RxReport3(RxReport3_len,ReceiveData3);
    			
	}
	if(Events&RX3_TIMEOUT_EVT)
	{	
		
		RecDataBuffer3[0]=RecLen3-1;
		RecLen3 =1; 
    RxReport3_len=RecDataBuffer3[0];
    memcpy(ReceiveData3,RecDataBuffer3+1,RecDataBuffer3[0]);		
		Start_timerEx(RX3_DELAY_EVT,3);//2*RecDataBuffer3[0]	
	}
	if(Events&CYCLE_CMD_EVT)
	{
	 cycle_cmd();//����ѭ��ִ�У����κ������ж�����ģ�������Ƿ�����; 
	}
  if(Events&WG_SENDZZ_EVT)    //��������վ���Ϳ��������ѯ�������ͨ�ţ�AI01��BI01 ����3
	{
		Send_slave_cmd();				
	}	
	
	if(Events&SET_SLAVEPARAM_EVT)    //������������վ���Ͳ����趨�������ͨ�ţ�AI01��BI01 ����3
	{
		set_slave_param();				
	}	
	
	
	if(Events&NET_INIT_EVT)//��ʼ��ģ������ƽ̨
	{
		net_connect_count++;
		if(net_connect_count>70)
   {
	 __set_FAULTMASK(1);
		 NVIC_SystemReset();
	   while(1);
	 }
		halt_module=0;
		Stop_timerEx(WG_REPLY_EVT);
	  Stop_timerEx(SEND_PLATFORM_EVT);
		Net_connect_init();			
	}
	if(Events&SEND_PLATFORM_EVT)//ģ�鷢�����ݵ�ƽ̨
	{ 		
	  if((halt_RxReport2>=10)){while(1);}
		if(factory_gateway_set[0]<=2&&(factory_gateway_set[0]!=0)) halt_RxReport2++;
		send_platform();//ģ�鷢�����ݵ�ƽ̨
	}
	if(Events&RX2_DELAY_EVT) //ģ�����ƽ̨����
	{			
		halt_RxReport2=0x00;//��SEND_PLATFORM_EVT�¼����ж�
		RxFlag2 = 1;
		RxReport2_step_flg=0;//RxReport2_step_flg��Ϊ�˼���CPU���и���
		//RxReport2_step_flg=1��ʾ��·���������cdma_cmd_receive	;RxReport2_step_flg=2��ʾЭ�麯�������cdma_tcp_receive��cdma_sdk_receive��
		RxReport2(RecLen2,ReceiveData2);
		if(RxReport2_step_flg==0)//SDKƽ̨�����Ͽ���·�������ݵ�ƽ̨��ƽ̨�ղ�������������������
     {
		   RxReport2_useless_count++;
			 if(RxReport2_useless_count>20){while(1);}
		 }
		 else{ RxReport2_useless_count=0;}
	}
if(Events&IO_XUNHUAN_CMD_EVT)//������Һλ
	 {
	  IOxh_send_cmd(); //IOѭ���������ɼ�����
		Start_timerEx( IO_XUNHUAN_CMD_EVT, IO_CMD_PERIOD );
	 }	
if(Events&RX1_DELAY_EVT) //ģ�����ƽ̨����
	{	
    if(factory_gateway_set[0]==3)	
		{			
			RxFlag1 = 1;
			RxReport1_step_flg=0;//RxReport2_step_flg��Ϊ�˼���CPU���и���
			//RxReport1_step_flg=1��ʾ��·���������cdma_cmd_receive	;RxReport2_step_flg=2��ʾЭ�麯�������cdma_tcp_receive��cdma_sdk_receive��
			RxReport1(RecLen1,ReceiveData1);
		}
		if(factory_gateway_set[27]==15)
    {
		  RxReport1_csb_yw(RecLen1,ReceiveData1);//������̫�������ó�����Һλ
		}			
	}	

if(Events&WG_REPLY_EVT)    //���ػظ�ƽ̨���͵Ŀ�����������趨�� 
	{
		wg_reply_cmd();				
	}
if(Events&JM_PLATFORM_REPLY_EVT)    //���ٻָ��·��Ŀ������ 
	{
		jm_platform_reply();				
	}
		
if(close_433MHZ!=0)//��ʹ��433MHZͨ�Ż�433MHZͨ�ų����⣬��close_433MHZ=0;
	{	
  if(Events&WX_CMD_EVT)//����SI4463���ڽ���״̬���ȴ������ж�
	{
		Clear_Buf(SI4463_RxBUFF,SI4463_RxLenth,0);
		SI4463_SET_PROPERTY_1( PKT_FIELD_1_LENGTH_12_8, 0x00 );
		SI4463_SET_PROPERTY_1( PKT_FIELD_1_LENGTH_7_0, 0x01 );
	 	SI4463_START_RX( SI4463_Channel, 0, PACKET_LENGTH, 8, 3, 3 );  
	}

	if(Events&WX_RECEIVE_EVT)//433MHZ���߽���
	{		
		SI4463_RxLenth = SI4463_READ_PACKET(SI4463_RxBUFF);			
		SI4463Receive(SI4463_RxLenth,SI4463_RxBUFF);		
	}

	if(Events&WX_SENDZZ_EVT)
	{ 
		WX_LED_TOGGLE;
		SI4463_SENDZZ(); //���߷��������վ
		Start_timerEx(WX_CMD_EVT,50);   
	}	

  if(Events&WX_SET_SLAVEPARAM_EVT) //������������վ���Ϳ��������ѯ�������ͨ�ţ�SPI1
	{
		WX_LED_TOGGLE;
		set_slave_paramWX();
    Start_timerEx(WX_CMD_EVT,120); 		
	}
 }
if(Events&WGCOLLECTOR_DATA_EVT)
	 {
	  wgcollector_data(); //�������ɼ�����
		pidcollector_data(); //pid���ݷ���
		Start_timerEx( WGCOLLECTOR_DATA_EVT, MEASURE_PERIOD ); //800ms�ɼ�һ��
	 }
if(Events&TX5_CMD_EVT)//���ش���5�ɼ���������;PC12--TX5,PD2--RX5,PD1--�л�����;AI03��BI03����5
	 {
	  uart5_send_cmd(); //�������ɼ�����
		Start_timerEx( TX5_CMD_EVT, TX5_CMD_PERIOD );
	 }
if(Events&RX5_DELAY_EVT)//���ش���5���մ��������ݣ�PC12--TX5,PD2--RX5,PD1--�л����ƣ�����ͨ�ţ�AI03��BI03 ����5
	{   
      		
  	RxFlag5=1;
		RxReport5(RxReport5_len,ReceiveData5);
    			
	}
	if(Events&RX5_TIMEOUT_EVT)
	{	
		
		RecDataBuffer5[0]=RecLen5-1;
		RecLen5=1; 
    RxReport5_len=RecDataBuffer5[0];
    memcpy(ReceiveData5,RecDataBuffer5+1,RecDataBuffer5[0]);
		Start_timerEx(RX5_DELAY_EVT,1);//2*RecDataBuffer5[0]	
	}	 
}

  
void Scan_Events_Handle(void)
{
	IWDG_ReloadCounter();//�������Ź�ι��
}

static u8  WriteMultipleRegister(u8 Slave_ID,u16 addr,u16 num,u8 *pData,u8 *temp)
	//WriteMultipleRegister(Query_Index_set+33,0x0004,4,I_current_limit[Query_Index_set],senddata)
{	
	temp[0] = Slave_ID;
	temp[1] = WRITE_MULTIPLE_REGISTER;
	temp[2] = (addr&0xFF00)>>8;
	temp[3] = addr&0x00FF;
	temp[4] = (num&0xFF00)>>8;
	temp[5] = num&0x00FF;
	temp[6] = (num*2)&0x00FF;
	memcpy(temp+7,pData,num*2);

	wm_CRC_Val = GetCRC16(temp,7+num*2);
	
	temp[7+num*2] = wm_CRC_Val&0x00FF;
	temp[8+num*2] = (wm_CRC_Val&0xFF00)>>8;	
	return (9+num*2);
}

 u8  WriteSingleRegister(u8 Slave_ID,u16 addr,u8 *pData,u8 *temp)
	//WriteSingleRegister(Query_Index_Controller+33,ctrl_j,Controllers[Query_Index_Controller]+ctrl_j*2,ReportData3);
{	
	temp[0] = Slave_ID;
	temp[1] = WRITE_SINGLE_REGISTER;
	temp[2] = (addr&0xFF00)>>8;
	temp[3] = addr&0x00FF;
  temp[4] = * pData;
  temp[5] = * (pData+1);
	//	memcpy(temp+4,pData,2);
	wm_CRC_Val = GetCRC16(temp,6);	
	temp[6] = wm_CRC_Val&0x00FF;
	temp[7] = (wm_CRC_Val&0xFF00)>>8;
	return (8);
}
 u8 ReadData(u8 Slave_ID,u8 function_code,u16 addr,u16 num,u8 *temp)
{
//	u16 CRC_Val;

	temp[0] = Slave_ID;
	temp[1] = function_code;
	temp[2] = (addr&0xFF00)>>8;
	temp[3] = addr&0x00FF;
	temp[4] = (num&0xFF00)>>8;
	temp[5] = num&0x00FF;
	rd_CRC_Val = GetCRC16(temp,6);
	temp[6] = rd_CRC_Val&0x00FF;
	temp[7] = (rd_CRC_Val&0xFF00)>>8;        
  return 8;
}

static void WriteDataToBuffer(u8 port,u8 *ptr,u8 start,u8 len)
{ 

	if(port==3)
	{
		memcpy(USART3SendTCB,ptr+start,len);

		USART3BufferCNT=len; 
		TXENABLE3;
		TxFlag3=1;		
		USART_ITConfig(USART3, USART_IT_TXE, ENABLE);	
	}	

	if(port==5)
	{
		memcpy(UART5SendTCB,ptr+start,len);
		UART5BufferCNT=len;
		TXENABLE5;
		TxFlag5=1;
		USART_ITConfig(UART5, USART_IT_TXE, ENABLE); 
	}
	if(port==1)
	{
		memcpy(USART1SendTCB,ptr+start,len);

		USART1BufferCNT=len; 
//		TXENABLE1;
		TxFlag1=0;
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);	
	}	
	if(port==2)
	{
		memcpy(USART2SendTCB,ptr+start,len);
		USART2BufferCNT=len; 
		TxFlag2=0;
			
		USART_ITConfig(USART2, USART_IT_TXE, ENABLE);	
		//USART_SendData�����У�u32 i=0x004FFFFF;//��ֹȫ˫��Ӳ��ͨ��ʱ��Ӳ��CTS����������ɵȴ�״̬��������watchdog(3S)��ͣ������;2S���ң��Կ��Ź���������Ϊ׼
	}	
	if(port==4)
	{
		memcpy(UART4SendTCB,ptr+start,len);
		UART4BufferCNT=len; 
		TXENABLE4;
		TxFlag4=0;
		USART_ITConfig(UART4, USART_IT_TXE, ENABLE);	
	}	
		return; 
} 

static u16 GetCRC16(u8 *Msg, u16 Len)
{
    gc_CRCHigh = 0xFF;//��CRC�ֽڳ�ʼ��
    gc_CRCLow  = 0xFF;//��CRC�ֽڳ�ʼ��

	for(gc_i=0;gc_i<Len;gc_i++)
	{
		gc_index   = gc_CRCHigh^*Msg++;
		gc_CRCHigh = gc_CRCLow^crc_hi[gc_index];
		gc_CRCLow  = crc_lo[gc_index];
	}
	return(gc_CRCLow<<8|gc_CRCHigh);
}


void RxReport4(u8 len,u8 *pData)
{	
	if(GetCRC16(pData,len)==0)
	{
		u16 param_addr_start,param_length;
		switch(pData[0]) //���ݴ�������վ��ַ���������г�������
	  {
			case 0xEA://����Ա��վ�趨(�������趨)
				if(pData[1]==0x06)
				{	          					
//				 WriteDataToBuffer(5,pData,0,8);//��Ӧ������յ�����ȫһ��;
					memcpy(UART4SendTCB,pData,8);
          WriteDataToDMA_BufferTX4(8);
				  slave_param_addr=pData[2]<<8|pData[3];
         if(slave_param_addr<=383)
				 {
           slave_KZparam_set[slave_param_addr/12][(slave_param_addr%12)*2]=pData[4];//ÿһ���ɼ�����18��������ÿ������2���ֽ�
           slave_KZparam_set[slave_param_addr/12][(slave_param_addr%12)*2+1]=pData[5];//ÿһ���ɼ�����18��������ÿ������2���ֽ�						 
					if(kzym_ID!=kzym_old_ID)
            {
						first_xiabiao_I=slave_param_addr/12;//����ÿһ���趨ҳ��ĵ�һ����������ַ�������߲����趨������ʹ��
						firstwx_xiabiao_i=slave_param_addr/12;//����ÿһ���趨ҳ��ĵ�һ����������ַ�������߲����趨������ʹ��		
						kzym_old_ID=kzym_ID;
						}
					slave_xiabiao_I=(slave_param_addr/12)-first_xiabiao_I;//ÿ����������12�����					
					slave_xiabiao_J=(slave_param_addr%12)*2;//ÿ������2���ֽڣ����ֽ���ǰ
				 	ctrlslave_param_set[slave_xiabiao_I][slave_xiabiao_J]=pData[4];
					ctrlslave_param_set[slave_xiabiao_I][slave_xiabiao_J+1]=pData[5];
					ctrlslave_param_flg[slave_xiabiao_I]=20;
					ctrlslave_param_flgWX[slave_xiabiao_I]=20;
         	set_finish_flg=0;					
				 }
          if((slave_param_addr==384)&&(pData[4]!=0x00))//��ַ==385-1
					{
						set_finish_flg=1;//�趨�����0�����ص�������
						slave_set_flg=1;//��������վ�����趨
            slave_set_flgWX=1;//��������վ�����趨							
						Stop_timerEx(WG_SENDZZ_EVT);						
						Stop_timerEx(WX_SENDZZ_EVT);
           	Start_timerEx(SET_SLAVEPARAM_EVT,3000);
            Start_timerEx(WX_SET_SLAVEPARAM_EVT,3000);
            Flash_Write(0x0807E800,(unsigned char *)slave_KZparam_set[0],32*12*2) ;//32����������ÿ��������3��*4ͨ��=12��������ÿ������2���ֽ�						
					 }
					if((slave_param_addr==385)&&(pData[4]!=0x00))//��ַ==386-1;��������ͬһ�趨ҳ�治���¼�����ʼ�±�
					{
					  kzym_ID=pData[4];
					}
				break;
				 }					 
        if(pData[1]==0x03&&pData[2]==0x01&&pData[3]==0x80)//��ַ==385-1
				{
					pData[2]=2;					
					pData[3]=set_finish_flg;
					pData[4]=0;
					CRCReport4 = GetCRC16(pData,5); //����5���ֽڣ���ַ��1�������ܺţ�1�����ֽ�����1��,0x00��ֵ��0x00��ֵ
			    pData[5] = CRCReport4&0x00FF;      //CRC��λ
			    pData[6] = (CRCReport4&0xFF00)>>8; //CRC��λ				
//			    WriteDataToBuffer(5,pData,0,7);
          memcpy(UART4SendTCB,pData,7);
          WriteDataToDMA_BufferTX4(7);					
				}				
			break;
      case 0xEB://���������Զ��л��趨��ѯ����
				if(pData[1]==0x06&&pData[2]==0)//������д���Զ�״̬��־����ַ ����06 ������ַ����2����3��״̬01 00 �� 00 00��������״̬���4��5��
				{
//					WriteDataToBuffer(5,pData,0,8);//��Ӧ������յ�����ȫһ��;
					memcpy(UART4SendTCB,pData,8);
          WriteDataToDMA_BufferTX4(8);
				  hand_auto_flg[pData[3]][0]=pData[4];//������ַ��=0~72��
					hand_auto_flg[pData[3]][1]=pData[5];
					hand_auto_count[pData[3]]=5;//�յ��µ����Զ�״̬����ƽ̨�ϱ�5��
					break;	
				}
				if(pData[1]==0x03&&pData[2]==0)
				{
					ReportData4[0]=pData[0];
					ReportData4[1]=pData[1];
					ReportData4[2]=pData[5]*2;//�ֽ���=������*2				 	
					memcpy(ReportData4+3,hand_auto_flg[pData[3]],pData[5]*2);
					CRCReport4 = GetCRC16(ReportData4,3+pData[5]*2);
					ReportData4[3+pData[5]*2]=CRCReport4&0x00FF;      //CRC��λ
					ReportData4[3+pData[5]*2+1] = (CRCReport4&0xFF00)>>8; //CRC��λ
//					WriteDataToBuffer(5,ReportData4,0,3+pData[5]*2+2);
					memcpy(UART4SendTCB,ReportData4,3+pData[5]*2+2);
          WriteDataToDMA_BufferTX4(3+pData[5]*2+2);
          break;					
				}
      break;				
			case 0xEC://רҵ��Ա��վ�趨���ɼ����趨��
				if(pData[1]==0x06)
				{					
//				 WriteDataToBuffer(5,pData,0,8);//��Ӧ������յ�����ȫһ��;	
				 memcpy(UART4SendTCB,pData,8);
         WriteDataToDMA_BufferTX4(8);
         slave_param_addr=pData[2]<<8|pData[3];
         if(slave_param_addr<=1151)
				 {
           slave_CJparam_set[slave_param_addr/18][(slave_param_addr%18)*2]=pData[4];//ÿһ���ɼ�����18��������ÿ������2���ֽ�
           slave_CJparam_set[slave_param_addr/18][(slave_param_addr%18)*2+1]=pData[5];//ÿһ���ɼ�����18��������ÿ������2���ֽ�				 
					 if(cjym_ID!=cjym_old_ID)
            {
						first_xiabiao_I=slave_param_addr/18;//����ÿһ���趨ҳ��ĵ�һ���ɼ�����ַ�������߲����趨������ʹ��
						firstwx_xiabiao_i=slave_param_addr/18;//����ÿһ���趨ҳ��ĵ�һ���ɼ�����ַ�������߲����趨������ʹ��		
						cjym_old_ID=cjym_ID;
						}
          			
					slave_xiabiao_I=(slave_param_addr/18)-first_xiabiao_I;//ÿ���ɼ�����18�����
					slave_xiabiao_J=(slave_param_addr%18)*2;//ÿ������2���ֽڣ����ֽ���ǰ
				 	cjqslave_param_set[slave_xiabiao_I][slave_xiabiao_J]=pData[4];
					cjqslave_param_set[slave_xiabiao_I][slave_xiabiao_J+1]=pData[5];
					cjqslave_param_flg[slave_xiabiao_I]=20;
					cjqslave_param_flgWX[slave_xiabiao_I]=20;					
					set_finish_flg=0;	
				 }
          if((slave_param_addr==1152)&&(pData[4]!=0x00))//��ַ==1153-1
					{	
						u8 i;
            set_finish_flg=1;//�趨����վ�Ƿ���ɱ�־,δ�����1�������0�����ص�������						
						slave_set_flg=2;//�ɼ������趨
            slave_set_flgWX=2;//�ɼ������趨						
						Stop_timerEx(WG_SENDZZ_EVT);						
						Stop_timerEx(WX_SENDZZ_EVT);
           	Start_timerEx(SET_SLAVEPARAM_EVT,3000);
            Start_timerEx(WX_SET_SLAVEPARAM_EVT,3000);
						for(i=0;i<36;i=i+2){slave_CJparam_set[64][i]=factory_gateway_set[12+i/2];slave_CJparam_set[64][i+1]=0;}						
            Flash_Write(0x0807D800,(unsigned char *)slave_CJparam_set[0],32*18*2) ;//ǰ32���ɼ�������
            Flash_Write(0x0807E000,(unsigned char *)slave_CJparam_set[32],33*18*2) ;//��33�������������زɼ���������						
					 }
					if((slave_param_addr==1153)&&(pData[4]!=0x00))//��ַ==1154-1;��������ͬһ�趨ҳ�治���¼�����ʼ�±�
					{
					  cjym_ID=pData[4];
					}
				break;
				 }					 
        if(pData[1]==0x03&&pData[2]==0x04&&pData[3]==0x80)//��ַ==1153-1
				{
					pData[2]=2;
					pData[3]=set_finish_flg;
					pData[4]=0;
					CRCReport4 = GetCRC16(pData,5); //����5���ֽڣ���ַ��1�������ܺţ�1�����ֽ�����1��,0x00��ֵ��0x00��ֵ
			    pData[5] = CRCReport4&0x00FF;      //CRC��λ
			    pData[6] = (CRCReport4&0xFF00)>>8; //CRC��λ				
//			    WriteDataToBuffer(5,pData,0,7);	
          memcpy(UART4SendTCB,pData,7);
          WriteDataToDMA_BufferTX4(7);					
				}				
			break;
			case 0xEE://��ʽ�����趨
				param_addr_start=pData[2]<<8|pData[3];
			  if(pData[1]==0x10&&param_addr_start<2080)
				{
					memcpy(zero_rang.array_k_b[param_addr_start/2],pData+7,pData[6]);
          CRCReport4 = GetCRC16(pData,6); //����ǰ6���ֽڣ���ַ��1�������ܺţ�1����������ʼ��ַ��2��������������2��
			    pData[6] = CRCReport4&0x00FF;      //CRC��λ
			    pData[7] = (CRCReport4&0xFF00)>>8; //CRC��λ				
//			    WriteDataToBuffer(5,pData,0,8);
          memcpy(UART4SendTCB,pData,8);
          WriteDataToDMA_BufferTX4(8);				  
				}
       	if(param_addr_start==2080)//�������ġ�ȷ������EE��
				{
					if(pData[1]==0x06&&pData[4]!=0x00) //��ȷ������EE��=1;��ʽ�����趨����65*16*4=4160���ֽڣ���3��bankд��
				 	{
//						WriteDataToBuffer(5,pData,0,8);//��Ӧ������յ�����ȫһ��;
            memcpy(UART4SendTCB,pData,8);
            WriteDataToDMA_BufferTX4(8);						
						Flash_Write(0x0807B800,(unsigned char *)  zero_rang.array_k_b,384*4) ;//����ʱע�ⲻ֪һ���Ƿ��ܹ�д�����2kbyte;��ʽ�������ù���780*4�ֽڷֶ���д��
						Flash_Write(0x0807C000,(unsigned char *)  zero_rang.array_k_b+384*4,384*4) ;//����ʱע�ⲻ֪һ���Ƿ��ܹ�д�����2kbyte
						Flash_Write(0x0807C800,(unsigned char *)  zero_rang.array_k_b+768*4,272*4) ;//0x0807C800~0x0807CC3F;�������ݿ��Դ�0x0807CC40��ʼ����
						set_finish_flgEE=0;
//						wg_init_readflash();						
           //�������·���ʽ�趨������Ҫд��flash0x0804 0000����256k��ʼд�룬0x0804��1��������64k;
					//�ر�ע��Flash_Write���������д�������ֽ���Ϊ512���������޸�Flash_Write�����еĶ��壬��������������HardFault_Handler������
						
					}
					if(pData[1]==0x06&&pData[4]==0x00) //��ȷ������EE��=0
				 	{
//						WriteDataToBuffer(5,pData,0,8);//��Ӧ������յ�����ȫһ��;
            memcpy(UART4SendTCB,pData,8);
            WriteDataToDMA_BufferTX4(8);						
					}
					if(pData[1]==0x03) //��������ѯ��ȷ������EE��������0����ַ��1�������ܺţ�1�����ֽ�����1��,0x00��ֵ��0x00��ֵ
				 	{
						pData[2]=2;
						pData[3]=set_finish_flgEE;
						pData[4]=0;
						CRCReport4 = GetCRC16(pData,5); //����5���ֽڣ���ַ��1�������ܺţ�1�����ֽ�����1��,0x00��ֵ��0x00��ֵ
			      pData[5] = CRCReport4&0x00FF;      //CRC��λ
			      pData[6] = (CRCReport4&0xFF00)>>8; //CRC��λ				
//			      WriteDataToBuffer(5,pData,0,7);
            memcpy(UART4SendTCB,pData,7);
            WriteDataToDMA_BufferTX4(7);						
					}
				 }			
			break;
			case 0xEF://���������趨
				if(pData[1]==0x06&&pData[3]<=0x1D)//0x1D=29��ǰ30���趨����
				{				 	
					memcpy(UART4SendTCB,pData,8);
          WriteDataToDMA_BufferTX4(8);
					if(pData[3]==15&&pData[4]==18)//�����������⣬PA1����Ϊ��ͣ���ж�
					{
						DATA_INPUT;//	������ʪ�����ݹܽ�PA1,����/����ģʽ���룬��PxODR�Ĵ���Լ��
						GPIOA->ODR=0<<1;//PA1�͵�ƽ����	�������޷���֤�͵�ƽ			
						TIM_ClearITPendingBit(TIM5, TIM_IT_CC2);					
						TIM_ClearITPendingBit(TIM5,TIM_IT_Update);
						TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE );
						TIM_ITConfig(TIM5,TIM_IT_CC2,ENABLE);					
						TIM_Cmd(TIM5,ENABLE);
          }
          else
					{
						TIM_ClearITPendingBit(TIM5, TIM_IT_CC2);					
						TIM_ClearITPendingBit(TIM5,TIM_IT_Update);
						TIM_ITConfig(TIM5,TIM_IT_Update,DISABLE );
						TIM_ITConfig(TIM5,TIM_IT_CC2,DISABLE);					
						TIM_Cmd(TIM5,DISABLE);
					  DATA_INPUT;//	������ʪ�����ݹܽ�PA1,����/����ģʽ���룬��PxODR�Ĵ���Լ��
						GPIOA->ODR=1<<1;//PA1�ߵ�ƽ����	�������޷���֤�ߵ�ƽ							
					}
          factory_gateway_set[pData[3]]=pData[4];//ֻȡ���ֽ��������ֽ����ݲ���					
				}
				
				if(pData[1]==0x06&&pData[3]==124)//�����趨�����Ĵ�����������ַ-1
				{
				 	close_433MHZ=pData[4];
//					WriteDataToBuffer(5,pData,0,8);//��Ӧ������յ�����ȫһ��;
					memcpy(UART4SendTCB,pData,8);
          WriteDataToDMA_BufferTX4(8);
				}
				
				if(pData[1]==0x10) //������д���ܺ�0x10ֻ�����ַ�������������0x06
				{
				 switch(pData[3])
				 {
					 case 0x1F://IP��ַ�˿�,�ɴ��������͹��������������21���ֽ�+1���ֽڱ���ʵ���ֽ����������±꣺30~51��������������ַ��0x1F~0x29
						factory_gateway_set[30]=pData[6]-8;//��̼��뱾���趨��ʵ���ֽ������������������ģ�
						memcpy(factory_gateway_set+31,pData+15,pData[6]-8); 
					 break;
					 case 0x2A://�ͻ���ID,��ֽ���Ϊ30�������±꣺52~82��������������ַ��0x2A~0x38
						factory_gateway_set[52]=pData[6]-8;//52=30+21+1
						memcpy(factory_gateway_set+53,pData+15,pData[6]-8); 
					 break;
					 case 0x39://��������,��ֽ���Ϊ40�������±꣺83~123��������������ַ��0x39~0x4C
						factory_gateway_set[83]=pData[6]-8;//83=52+30+1
						memcpy(factory_gateway_set+84,pData+15,pData[6]-8); 
					 break;
					 case 0x4D://��������,��ֽ���Ϊ30�������±꣺124~154��������������ַ��0x4D~0x5B
						factory_gateway_set[124]=pData[6]-8;
						memcpy(factory_gateway_set+125,pData+15,pData[6]-8); 
					 break;
					 case 0x5C://�û���,��ֽ���Ϊ32�������±꣺155~187��������������ַ��0x5C~0x6C
						factory_gateway_set[155]=pData[6]-8;
						memcpy(factory_gateway_set+156,pData+15,pData[6]-8);           					 
					 break;
					 case 0x6C://�û�������,��ֽ���Ϊ32�������±꣺188~220��������������ַ��0x6D~0x7D
						factory_gateway_set[188]=pData[6]-8;
						memcpy(factory_gateway_set+189,pData+15,pData[6]-8);           					 
					 break;
					 default:
      			break;
				 }
				 CRCReport4 = GetCRC16(pData,6); //����ǰ6���ֽڣ���ַ��1�������ܺţ�1����������ʼ��ַ��2��������������2��
			   pData[6] = CRCReport4&0x00FF;      //CRC��λ
			   pData[7] = (CRCReport4&0xFF00)>>8; //CRC��λ				
//			   WriteDataToBuffer(5,pData,0,8);
				 memcpy(UART4SendTCB,pData,8);
         WriteDataToDMA_BufferTX4(8);
//				 set_finish_flg=1;
			  }
				
				if(pData[3]==0x1E)//�������ġ�ȷ������EF��
				{
					if(pData[1]==0x06&&pData[4]!=0x00) //��ȷ������EF��=1
				 	{
//						WriteDataToBuffer(5,pData,0,8);//��Ӧ������յ�����ȫһ��;
						memcpy(UART4SendTCB,pData,8);
            WriteDataToDMA_BufferTX4(8);
						memcpy(factory_gateway_set+221,main_call,11);
		        memcpy(factory_gateway_set+232,voice_call,11);
		        memcpy(factory_gateway_set+243,third_call,11);
						factory_gateway_set[254]=close_433MHZ;
						Flash_Write(0x0807B000, factory_gateway_set,255) ;
						set_finish_flgEF=0;
//						wg_init_readflash();
           //�������·����������趨������Ҫд��flash0x0807 0000����256k��ʼд�룬0x0804��1��������64k;
					//�ر�ע��Flash_Write���������д�������ֽ���Ϊ512���������޸�Flash_Write�����еĶ��壬��������������HardFault_Handler������
						
					}
					if(pData[1]==0x06&&pData[4]==0x00) //��ȷ������EF��=0
				 	{
//						WriteDataToBuffer(5,pData,0,8);//��Ӧ������յ�����ȫһ��;
            memcpy(UART4SendTCB,pData,8);
            WriteDataToDMA_BufferTX4(8);						
					}
					if(pData[1]==0x03) //��������ѯ��ȷ������EF��������0����ַ��1�������ܺţ�1�����ֽ�����1��,0x00��ֵ��0x00��ֵ
				 	{
						pData[2]=2;
						pData[3]=set_finish_flgEF;
						pData[4]=0;
						CRCReport4 = GetCRC16(pData,5); //����5���ֽڣ���ַ��1�������ܺţ�1�����ֽ�����1��,0x00��ֵ��0x00��ֵ
			      pData[5] = CRCReport4&0x00FF;      //CRC��λ
			      pData[6] = (CRCReport4&0xFF00)>>8; //CRC��λ				
//			      WriteDataToBuffer(5,pData,0,7);
            memcpy(UART4SendTCB,pData,7);
            WriteDataToDMA_BufferTX4(7);						
					}
				 }
			break;//���������趨����
				 
			case 0xF0://רҵ��Ա��վ��ַ�趨
				if(pData[1]==0x06)
				{				 	
//					WriteDataToBuffer(5,pData,0,8);//��Ӧ������յ�����ȫһ��;
					memcpy(UART4SendTCB,pData,8);
          WriteDataToDMA_BufferTX4(8);
					
    /*pData[3]=0 �ϵ�ַ��=1 �µ�ַ��=2 ���ŵ���=3 ���ŵ���=4 ��վ�²ɼ���������=5 ��վ�¿�����������=6 �²ɼ�����վʼ��ַ��
					=7 �¿�������վʼ��ַ��=8 �²ɱ�����ַ��=9 �²ɱ���������=10 �¿ر�����ַ��=11 �¿ر���������
				  =12�¿ص�����ַ�� =13 �¿ص���������=14 �ɼ���λ����վ��ַ��=15 ������λ����վ��ַ�� 
					=16 ȷ������F0���ر�˵�����ɼ�����Ϊ��վ�����ô���3��������λ�����ɼ���λ����վ��ַ�������λ����վ��ַͨ��������ͬ��
					Ҳ���Էֱ��趨�����������*/
					
					if(pData[3]<=0x0F)
					{
						set_slaveID_channel[pData[3]]=pData[4];//��ַ ���ܺ� ������ַ�� ������ַ�� ���ݵ� ���ݸ�
						slaveID_channel_flg=20;//����վ����վ��ַ�������ŵ��趨����Ĵ���
						slaveID_channel_flgWX=20;//����վ����վ��ַ�������ŵ��趨����Ĵ���						
//						set_finish_flg=0;            						
					}
					if(pData[3]==0x10&&pData[4]!=0x00)//��ַpData[3]==16;�趨����վ�Ƿ���ɱ�־,δ�����1�������0�����ص�������
					{
					  set_finish_flg=1;//�趨����վ�Ƿ���ɱ�־,δ�����1�������0�����ص�������						
						slave_set_flg=3;//��վ��ַ�������ŵ��������趨��־
            slave_set_flgWX=3;//��վ��ַ�������ŵ��������趨��־
            						
						Stop_timerEx(WG_SENDZZ_EVT);
						Stop_timerEx(WX_SENDZZ_EVT);
           	Start_timerEx(SET_SLAVEPARAM_EVT,100);
            Start_timerEx(WX_SET_SLAVEPARAM_EVT,150);			
					}
				}
				if(pData[1]==0x03&&pData[2]==0x00&&pData[3]==0x10)//��ַ==16;�趨����վ�Ƿ���ɱ�־,δ�����1�������0�����ص�������
				{
					pData[2]=2;
					pData[3]=set_finish_flg;
					pData[4]=0;
					CRCReport4 = GetCRC16(pData,5); //����5���ֽڣ���ַ��1�������ܺţ�1�����ֽ�����1��,0x00��ֵ��0x00��ֵ
			    pData[5] = CRCReport4&0x00FF;      //CRC��λ
			    pData[6] = (CRCReport4&0xFF00)>>8; //CRC��λ				
//			    WriteDataToBuffer(5,pData,0,7);
          memcpy(UART4SendTCB,pData,7);
          WriteDataToDMA_BufferTX4(7);							
				}	
       	if(pData[1]==0x04)//������վ��ַ���ص�������
				{
					u8 i,i_offset;
					i_offset=0;
					ReportData4[0]=pData[0];
					ReportData4[1]=pData[1];
					ReportData4[2]=pData[5]*2;//�ֽ���=������*2
					if(pData[3]<=71)
					{
						if(pData[3]>=40){i_offset=32;}//64����վ��2�ζ�ȡ����һ�ζ�ȡ��ַΪ0���ڶ��ζ�ȡ��ַΪ40
						for(i=0;i<pData[5];i++)
						 {
							ReportData4[2*i+3]=online_slaveID[i+i_offset];
							ReportData4[2*i+4]=0;
						 }	
				   }
          if(pData[3]>=77)
					{
						if(pData[3]>=110){i_offset=32;}//64��������վ��2�ζ�ȡ����һ�ζ�ȡ��ַΪ77~108���ڶ��ζ�ȡ��ַΪ110~141
						for(i=0;i<pData[5];i++)
						 {
							ReportData4[2*i+3]=online_slaveID_WX[i+i_offset];
							ReportData4[2*i+4]=0;
						 }
					 }									
					CRCReport4 = GetCRC16(ReportData4,3+pData[5]*2);
					ReportData4[3+pData[5]*2]=CRCReport4&0x00FF;      //CRC��λ
					ReportData4[3+pData[5]*2+1] = (CRCReport4&0xFF00)>>8; //CRC��λ
//					WriteDataToBuffer(5,ReportData4,0,3+pData[5]*2+2);
          memcpy(UART4SendTCB,ReportData4,3+pData[5]*2+2);
          WriteDataToDMA_BufferTX4(3+pData[5]*2+2);							 
				}			
			break;//רҵ��Ա��վ��ַ�趨����
			case 0xF1://���ع�ʽ����
				Flash_Read(0x0807B800,(unsigned char *)  zero_rang.array_k_b,384*4);
        Flash_Read(0x0807C000,(unsigned char *)  zero_rang.array_k_b+384*4,384*4);
        Flash_Read(0x0807C800,(unsigned char *)  zero_rang.array_k_b+768*4,272*4);			  	
				param_addr_start=pData[2]<<8|pData[3];
			  param_length=pData[4]<<8|pData[5];
			  if(pData[1]==0x03&&param_addr_start<2121&&param_length>1)//���ع�ʽ����,�����ɼ���һ��16*3����������������Ϊ96���������ֽ���192��
				{
					u8 i;//21*96(60H)+1*64(40H)*2=4160���ֽ�				  				
          ReportData4[0]=pData[0];
					ReportData4[1]=pData[1];
					ReportData4[2]=pData[5]*2;//�ֽ���=������*2	
          memcpy(ReportData4+3,(unsigned char *)zero_rang.array_k_b+(param_addr_start/96)*96*2,param_length*2);
          CRCReport4 = GetCRC16(ReportData4,3+pData[5]*2);
					ReportData4[3+pData[5]*2]=CRCReport4&0x00FF;      //CRC��λ
					ReportData4[3+pData[5]*2+1] = (CRCReport4&0xFF00)>>8; //CRC��λ          					
					memcpy(UART4SendTCB,ReportData4,5+param_length*2);
          WriteDataToDMA_BufferTX4(5+param_length*2);
					formula_param_finish[param_addr_start/96]=1;
					formula_read_finishF1=1;	
					for(i=0;i<22;i++){formula_read_finishF1=formula_read_finishF1&formula_param_finish[i];}
					if(formula_read_finishF1==1){memset(formula_param_finish,0x00,sizeof(formula_param_finish));set_finish_flgF1=0;}
				 }
				if(param_addr_start==2124)//�������ġ�ȷ������F1��
				{
					if(pData[1]==0x06&&pData[4]!=0x00) //��ȷ������F1��=1
				 	{
//						WriteDataToBuffer(5,pData,0,8);//��Ӧ������յ�����ȫһ��;
            memcpy(UART4SendTCB,pData,8);
            WriteDataToDMA_BufferTX4(8);
						set_finish_flgF1=1;						
           //�������·���ʽ�趨������Ҫд��flash0x0804 0000����256k��ʼд�룬0x0804��1��������64k;
					//�ر�ע��Flash_Write���������д�������ֽ���Ϊ512���������޸�Flash_Write�����еĶ��壬��������������HardFault_Handler������						
					}
					if(pData[1]==0x06&&pData[4]==0x00) //��ȷ������F1��=0				 	
						{
//						WriteDataToBuffer(5,pData,0,8);//��Ӧ������յ�����ȫһ��;
            memcpy(UART4SendTCB,pData,8);
            WriteDataToDMA_BufferTX4(8);					
					  }
					if(pData[1]==0x03) //��������ѯ��ȷ������F1������ɷ���0����ַ��1�������ܺţ�1�����ֽ�����1��,0x00��ֵ��0x00��ֵ
				 	{
						pData[2]=2;
						pData[3]=set_finish_flgF1;
						pData[4]=0;
						CRCReport4 = GetCRC16(pData,5); //����5���ֽڣ���ַ��1�������ܺţ�1�����ֽ�����1��,0x00��ֵ��0x00��ֵ
			      pData[5] = CRCReport4&0x00FF;      //CRC��λ
			      pData[6] = (CRCReport4&0xFF00)>>8; //CRC��λ				
//			      WriteDataToBuffer(5,pData,0,7);
            memcpy(UART4SendTCB,pData,7);
            WriteDataToDMA_BufferTX4(7);						
					}
				 }			
			break;//���ع�ʽ��������
			case 0xF2://���زɼ�����
				Flash_Read(0x0807D800,(unsigned char *)slave_CJparam_set[0],32*18*2) ;//ǰ32���ɼ����ɼ���������
        Flash_Read(0x0807E000,(unsigned char *)slave_CJparam_set[32],33*18*2) ;//��33���������ɼ������زɼ���������			 
			  param_addr_start=pData[2]<<8|pData[3];
			  param_length=pData[4]<<8|pData[5];
			  if(pData[1]==0x04&&param_addr_start<1180&&param_length>1)//���زɼ�����,6���ɼ�������һ��18*6��������������Ϊ108���������ֽ���216��
				{
					u8 i;					
					ReportData4[0]=pData[0];
				  ReportData4[1]=pData[1];
				  ReportData4[2]=pData[5]*2;//�ֽ���=������*2
					memcpy(ReportData4+3,(unsigned char *)slave_CJparam_set+(param_addr_start/108)*108*2,param_length*2);
					CRCReport4 = GetCRC16(ReportData4,3+pData[5]*2);
					ReportData4[3+pData[5]*2]=CRCReport4&0x00FF;      //CRC��λ
					ReportData4[3+pData[5]*2+1] = (CRCReport4&0xFF00)>>8; //CRC��λ          					
					memcpy(UART4SendTCB,ReportData4,5+param_length*2);
          WriteDataToDMA_BufferTX4(5+param_length*2);
					collector_param_finish[param_addr_start/108]=1;
					collector_read_finishF2=1;	
					for(i=0;i<11;i++){collector_read_finishF2=collector_read_finishF2&collector_param_finish[i];}
					if(collector_read_finishF2==1){memset(collector_param_finish,0x00,sizeof(collector_param_finish));set_finish_flgF2=0;}
				 }
			  if(param_addr_start==1181)//�������ġ����ɼ��������F2����������ַ-1
				{
					if(pData[1]==0x06&&pData[4]!=0x00) //�����ɼ��������F2��=1
				 	{
            memcpy(UART4SendTCB,pData,8);
            WriteDataToDMA_BufferTX4(8);
						set_finish_flgF2=1;					
           					
					}
					if(pData[1]==0x06&&pData[4]==0x00) //�����ɼ��������F2��=0				 	
						{
              memcpy(UART4SendTCB,pData,8);
              WriteDataToDMA_BufferTX4(8);					
					  }
					if(pData[1]==0x03) //��������ѯ�����ɼ��������F2������ɷ���0����ַ��1�������ܺţ�1�����ֽ�����1��,0x00��ֵ��0x00��ֵ
				 	{
						pData[2]=2;
						pData[3]=set_finish_flgF2;
						pData[4]=0;
						CRCReport4 = GetCRC16(pData,5); //����5���ֽڣ���ַ��1�������ܺţ�1�����ֽ�����1��,0x00��ֵ��0x00��ֵ
			      pData[5] = CRCReport4&0x00FF;      //CRC��λ
			      pData[6] = (CRCReport4&0xFF00)>>8; //CRC��λ	
            memcpy(UART4SendTCB,pData,7);
            WriteDataToDMA_BufferTX4(7);						
					}
				 }
			break;//���زɼ���������
				 
			case 0xF3://���ؿ��Ʋ���
			  Flash_Read(0x0807E800,(unsigned char *)slave_KZparam_set[0],32*12*2) ;//����32������������   
//			  memcpy(test,pData,8);
			  param_addr_start=pData[2]<<8|pData[3];
			  param_length=pData[4]<<8|pData[5];
			  if(pData[1]==0x04&&param_addr_start<387&&param_length>1)//���ؿ��Ʋ���,6���ɼ�������һ��18*6��������������Ϊ108���������ֽ���216��
				{
					u8 i;					
					ReportData4[0]=pData[0];
				  ReportData4[1]=pData[1];
				  ReportData4[2]=pData[5]*2;//�ֽ���=������*2
					memcpy(ReportData4+3,(unsigned char *)slave_KZparam_set+(param_addr_start/96)*96*2,param_length*2);
					CRCReport4 = GetCRC16(ReportData4,3+pData[5]*2);
					ReportData4[3+pData[5]*2]=CRCReport4&0x00FF;      //CRC��λ
					ReportData4[3+pData[5]*2+1] = (CRCReport4&0xFF00)>>8; //CRC��λ          					
					memcpy(UART4SendTCB,ReportData4,5+param_length*2);
          WriteDataToDMA_BufferTX4(5+param_length*2);
					ctrl_param_finish[param_addr_start/96]=1;
					ctrl_read_finishF3=1;	
					for(i=0;i<4;i++){ctrl_read_finishF3=ctrl_read_finishF3&ctrl_param_finish[i];}
					if(ctrl_read_finishF3==1){memset(ctrl_param_finish,0x00,sizeof(ctrl_param_finish));set_finish_flgF3=0;}
				 }
			  if(param_addr_start==388)//�������ġ����ɼ��������F2����������ַ389-1
				{
					if(pData[1]==0x06&&pData[4]!=0x00) //�������Ʋ������F3��=1
				 	{
            memcpy(UART4SendTCB,pData,8);
            WriteDataToDMA_BufferTX4(8);
						set_finish_flgF3=1;					
           					
					}
					if(pData[1]==0x06&&pData[4]==0x00) //�������Ʋ������F3��=0				 	
						{
              memcpy(UART4SendTCB,pData,8);
              WriteDataToDMA_BufferTX4(8);					
					  }
					if(pData[1]==0x03) //��������ѯ�������Ʋ������F3������ɷ���0����ַ��1�������ܺţ�1�����ֽ�����1��,0x00��ֵ��0x00��ֵ
				 	{
						pData[2]=2;
						pData[3]=set_finish_flgF3;
						pData[4]=0;
						CRCReport4 = GetCRC16(pData,5); //����5���ֽڣ���ַ��1�������ܺţ�1�����ֽ�����1��,0x00��ֵ��0x00��ֵ
			      pData[5] = CRCReport4&0x00FF;      //CRC��λ
			      pData[6] = (CRCReport4&0xFF00)>>8; //CRC��λ	
            memcpy(UART4SendTCB,pData,7);
            WriteDataToDMA_BufferTX4(7);						
					}
				 }
			break;//���ؿ��Ʋ�������
			default:  //����������������ͨѶ
				if(pData[1]==0x04&&pData[0]<=65)
				{	
//					memcpy(test,pData,30);
          if(pData[3]==0x00)
					{						
						ReportData4[0]=pData[0];
						ReportData4[1]=pData[1];
						ReportData4[2]=pData[5]*2;//�ֽ���=������*2
						ReportData4[3]=ZZ_Wired_flag[pData[0]-1];
						ReportData4[4]=0x00;
						ReportData4[5]=ZZ_Wireles_flag[pData[0]-1];
						ReportData4[6]=0x00;
						memcpy(ReportData4+7,Collectors[pData[0]-1],(pData[5]-2)*2);
						CRCReport4 = GetCRC16(ReportData4,3+pData[5]*2);
						ReportData4[3+pData[5]*2]=CRCReport4&0x00FF;      //CRC��λ
						ReportData4[3+pData[5]*2+1] = (CRCReport4&0xFF00)>>8; //CRC��λ
//						WriteDataToBuffer(5,ReportData4,0,3+pData[5]*2+2);
						memcpy(UART4SendTCB,ReportData4,3+pData[5]*2+2);
            WriteDataToDMA_BufferTX4(3+pData[5]*2+2);		
          }
         	if(pData[3]!=0x00)
					{						
						ReportData4[0]=pData[0];
						ReportData4[1]=pData[1];
						ReportData4[2]=pData[5]*2;//�ֽ���=������*2						
						memcpy(ReportData4+3,Collectors[pData[0]-1]+(pData[3]-1),(pData[5])*2);
						CRCReport4 = GetCRC16(ReportData4,3+pData[5]*2);
						ReportData4[3+pData[5]*2]=CRCReport4&0x00FF;      //CRC��λ
						ReportData4[3+pData[5]*2+1] = (CRCReport4&0xFF00)>>8; //CRC��λ
//						WriteDataToBuffer(5,ReportData4,0,3+pData[5]*2+2);
						memcpy(UART4SendTCB,ReportData4,3+pData[5]*2+2);
            WriteDataToDMA_BufferTX4(3+pData[5]*2+2);		
          }				
				}
				if(pData[1]==0x03&&pData[0]<=64&&pData[0]>=33)
				{
					ReportData4[0]=pData[0];
					ReportData4[1]=pData[1];
					ReportData4[2]=pData[5]*2;//�ֽ���=������*2				 	
					memcpy(ReportData4+3,Controllers[pData[0]-33],pData[5]*2);
					CRCReport4 = GetCRC16(ReportData4,3+pData[5]*2);
					ReportData4[3+pData[5]*2]=CRCReport4&0x00FF;      //CRC��λ
					ReportData4[3+pData[5]*2+1] = (CRCReport4&0xFF00)>>8; //CRC��λ
//					WriteDataToBuffer(5,ReportData4,0,3+pData[5]*2+2);
          memcpy(UART4SendTCB,ReportData4,3+pData[5]*2+2);
          WriteDataToDMA_BufferTX4(3+pData[5]*2+2);					
				}
				if(pData[1]==0x06&&pData[0]<=64&&pData[0]>=33)	
			  {	
//         	WriteDataToBuffer(5,pData,0,8);//��Ӧ������յ�����ȫһ��;
          memcpy(UART4SendTCB,pData,8);
          WriteDataToDMA_BufferTX4(8);					
				 if(pData[3]<=0x03)
				 {
				  Controllers[pData[0]-33][pData[3]*2]= pData[4];					
				  Controllers[pData[0]-33][pData[3]*2+1]= pData[5];
					crtl_cmd_num[pData[0]-33][pData[3]]=50;//��վ���߷��ͼ�¼�������ظ�����3�Σ��յ���0��������������͡�
          crtl_cmd_numWX[pData[0]-33][pData[3]]=10;//��վ���߷��ͼ�¼�������ظ�����3�Σ��յ���0��������������͡�						 
       	  }
				}				
			break;
	  }
	}					
}
static void wg_init_readflash(void)     //���س�ʼ����flash;flash��0x0800 0000~0x0807 FFFF����512k
{
  u8 init_flash_flg[2]={0,0};
	   Flash_Read(0x0807B000,  init_flash_flg, 2);//�������·����������趨������Ҫд��flash0x0807 C000����256k��ʼд�룬0x0804��1��������64k,ÿ��д�����2Kbyte
	   if(init_flash_flg[0]!=0xFF&&init_flash_flg[1]!=0xFF) //����Ѿ������������ã������������趨������ʹ�ó����ֵ
		 {
			 Flash_Read(0x0807B000,  factory_gateway_set, 221);
			 Flash_Read(0x0807B000+221,  main_call, 11);
			 Flash_Read(0x0807B000+232,  voice_call, 11);
			 Flash_Read(0x0807B000+243,  third_call, 11);
			 Flash_Read(0x0807B000+254, &close_433MHZ, 1);
		 }
		 Flash_Read(0x0807B800,  init_flash_flg, 2);//�������·����������趨������Ҫд��flash0x0804 C000����256k��ʼд�룬0x0804��1��������64k,ÿ��д�����2Kbyte
	   if(init_flash_flg[0]!=0xFF&&init_flash_flg[1]!=0xFF) //����Ѿ������������ã������������趨������ʹ�ó����ֵ
		 {
			 Flash_Read(0x0807B800,(unsigned char *)  zero_rang.array_k_b,384*4);			
		 }
		   Flash_Read(0x0807C000,  init_flash_flg, 2);//�������·����������趨������Ҫд��flash0x0804 C000����256k��ʼд�룬0x0804��1��������64k,ÿ��д�����2Kbyte
	   if(init_flash_flg[0]!=0xFF&&init_flash_flg[1]!=0xFF) //����Ѿ������������ã������������趨������ʹ�ó����ֵ
		 {
			 Flash_Read(0x0807C000,(unsigned char *)  zero_rang.array_k_b+384*4,384*4);			
		 }
		 Flash_Read(0x0807C800,  init_flash_flg, 2);//�������·����������趨������Ҫд��flash0x0804 C000����256k��ʼд�룬0x0804��1��������64k,ÿ��д�����2Kbyte
	   if(init_flash_flg[0]!=0xFF&&init_flash_flg[1]!=0xFF) //����Ѿ������������ã������������趨������ʹ�ó����ֵ
		 {
			 Flash_Read(0x0807C800,(unsigned char *)  zero_rang.array_k_b+768*4,272*4);//0x0807C800~0x0807CC3F;�������ݿ��Դ�0x0807CC40��ʼ����			
		 }
		 Flash_Read(0x0807D000,  init_flash_flg, 2);//�������·����������趨������Ҫд��flash0x0804 C000����256k��ʼд�룬0x0804��1��������64k,ÿ��д�����2Kbyte
	   if(init_flash_flg[0]!=0xFF) //����Ѿ������������ã����������õ�433MHZ�ŵ��趨������ʹ�ó����ֵSI4463_Channel=0
		 {
			 SI4463_Channel=init_flash_flg[0];	
		 }
}

static void Send_slave_cmd(void)
{     
//      slave_set_flg=0;//�������������������վ
  		switch(Query_Flag)
  		{
    		case CONTROLLERS_CMD:
 //��ʼ��δ��� �� ������վ����ɹ��յ��ظ����跢�ͣ�ͬʱ��������վIDС�ڵ���32��������ķ�����վ��������;
          while((crtl_cmd_num[Query_Index_Controller][ctrl_j]<=0)&&(Query_Index_Controller<32)&&ctrl_j<=3)//CSH_Wired_finish==0|ctrl_jΪ���Ƶ����,ÿ��������4��
              {	
                ctrl_j++;								
								if(ctrl_j>=4)
								{								 
							   Query_Index_Controller++;
								 ctrl_j=0;
								}
							 }
					if(Query_Index_Controller<32&&ctrl_j<=3)
					{           						
						bytelen3=WriteSingleRegister(Query_Index_Controller+33,ctrl_j,Controllers[Query_Index_Controller]+ctrl_j*2,ReportData3);//ÿ����2���ֽ�
						memcpy(USART3SendTCB,ReportData3,bytelen3);
						WriteDataToDMA_BufferTX3(bytelen3);
												
						if(crtl_cmd_num[Query_Index_Controller][ctrl_j]>0) crtl_cmd_num[Query_Index_Controller][ctrl_j]--;//��վ���ͼ�¼��ÿ����һ�μ�1���յ��ظ���0�������ظ�����3��
						ctrl_j++;								
						if(ctrl_j>=4)
						{								 
						 Query_Index_Controller++;
						 ctrl_j=0;
						}
					}
      			if(Query_Index_Controller>=32)
  				{  
    				Query_Index_Controller=0;
						ctrl_j=0;
    				Query_Flag=COLLECTORS_CMD;						
  				}      			
	  			Start_timerEx(WG_SENDZZ_EVT,send_Collectors_time); 					
      			break;
      
    		case COLLECTORS_CMD:
				 while((online_slaveID[Query_Index_Collector]==0)&&(Query_Index_Collector<64))
					{									 
						 Query_Index_Collector++;						
					 }
          if(Query_Index_Collector<64)
          {						
				  bytelen3=ReadData(Query_Index_Collector+1,READ_HOLDING_REGISTER,0x0000,0x0008,ReportData3);  //����д��ReportData3
  				memcpy(USART3SendTCB,ReportData3,bytelen3);                                                 //������������
					WriteDataToDMA_BufferTX3(bytelen3);                                                           //DMA����
//					if(test_addr==Query_Index_Collector+1)test_send++;
          Query_Index_Collector++;
					}						
      		if(Query_Index_Collector >63)//�ɼ�����ѯ��ϣ���ʼ��ѯ������
				   {  
        			Query_Index_Collector=0;					    
							Query_Flag= ZZ_QUERY_COLLECTOR;
						  Start_timerEx(WG_SENDZZ_EVT,send_Collectors_time);
						  break;
						
				    }
          else
					 {						
							Query_Flag=CONTROLLERS_CMD;     		
							Start_timerEx(WG_SENDZZ_EVT,send_Collectors_time);
							
							break;
				    }
					 
        case ZZ_QUERY_COLLECTOR:
					 while((online_slaveID[Query_IndexZZ_C_YX]!=0)&&(Query_IndexZZ_C_YX<32))
					   {									 
						   Query_IndexZZ_C_YX++;						
					    }
           if(Query_IndexZZ_C_YX<32)
            {						
							bytelen3=ReadData(Query_IndexZZ_C_YX+1,READ_HOLDING_REGISTER,0x0000,0x0008,ReportData3);  
							memcpy(USART3SendTCB,ReportData3,bytelen3);
						  WriteDataToDMA_BufferTX3(bytelen3);
							if(init_cmd_numYX[Query_IndexZZ_C_YX]>0){init_cmd_numYX[Query_IndexZZ_C_YX]--;}
							else {Query_IndexZZ_C_YX++;}
					   }
						if(Query_IndexZZ_C_YX >31){Query_IndexZZ_C_YX=0;}//�ɼ�����ѯ��ϣ���ʼ��ѯ������						
						 Query_Flag=ZZ_QUERY_CONTROLLER;     		
						 Start_timerEx(WG_SENDZZ_EVT,send_Collectors_time);
             	
        		break;
        		
        case ZZ_QUERY_CONTROLLER:
					 while((online_slaveID[Query_IndexZZ_K_YX+32]!=0)&&(Query_IndexZZ_K_YX<32))
					   {									 
						   Query_IndexZZ_K_YX++;						
					    }
           if(Query_IndexZZ_K_YX<32)
            {						
							bytelen3=ReadData(Query_IndexZZ_K_YX+33,READ_HOLDING_REGISTER,0x0000,0x0008,ReportData3);  
							memcpy(USART3SendTCB,ReportData3,bytelen3);
						  WriteDataToDMA_BufferTX3(bytelen3);
							if(init_cmd_numYX[Query_IndexZZ_K_YX+32]>0){init_cmd_numYX[Query_IndexZZ_K_YX+32]--;}
							else {Query_IndexZZ_K_YX++;}
					   }
						if(Query_IndexZZ_K_YX >31)//�ɼ�����ѯ��ϣ���ʼ��ѯ������
						{						 
						  Query_IndexZZ_K_YX=0;
							
						 if(ZZ_Wireles_flag[64]==0)
						 {
							 u8 i;
							 CSH_countYX=0;
							 for (i=0;i<64;i++)
								{
								 CSH_countYX=CSH_countYX|init_cmd_numYX[i];
								}
							 if((CSH_countYX==0)&&(CSH_countWX==0)){ZZ_Wireles_flag[64]=1;}	
						 }
						 	if(Query_Wired_WirelesYX>0){ZZ_Wired_flag[Query_Wired_WirelesYX-1]=ZZ_temp_stateYX;}
              else {ZZ_Wired_flag[63]=ZZ_temp_stateYX;}									
							ZZ_temp_stateYX=ZZ_Wired_flag[Query_Wired_WirelesYX];
							ZZ_Wired_flag[Query_Wired_WirelesYX]=0x02;//��վ�������ڼ��ͨ�ű�־
//							online_slaveID[Query_Wired_WirelesYX]=0x00;//��վ�������ڼ��ͨ�ű�־
							if(ZZ_temp_stateYX==0x02){ZZ_temp_stateYX=0;}							
							Query_Wired_WirelesYX++;
              if(Query_Wired_WirelesYX >63){Query_Wired_WirelesYX=0;}							
						 }
						 Query_Flag=CONTROLLERS_CMD;     		
						 Start_timerEx(WG_SENDZZ_EVT,send_Collectors_time);
             	
        		break;									 
    		default:
					Start_timerEx(WG_SENDZZ_EVT,500);
      			break;
  		}
}  
static void RxReport3(u8 len,u8 *pData) //����վͨ�ţ�������վ�ظ�
{	
	if(GetCRC16(pData,len)==0)
	{
		 online_slaveID[pData[0]-1]=pData[0];//��ʾ����վ���ߣ���¼����
		 ZZ_Wired_flag[pData[0]-1]=1;//�յ���վ����ͨ�ţ����ñ�־��0x02���ߣ���Ϊ0��ʾ����վ����
		 init_cmd_numYX[pData[0]-1]=0;
     Start_timerEx(WG_SENDZZ_EVT,120);		 
		 if(pData[1]==0x03)
		{	      
       memcpy(Collectors[pData[0]-1],pData+3,pData[2]);
//			 if(test_send==0){test_recive=0;}
//			 if(test_addr==pData[0])test_recive++;
       return;			
		}
		
		if(pData[1]==0x06)
		{
     	if((pData[2]==0x00)&&(pData[3]<=0x03)&&(pData[0]>=33)&&(pData[0]<=64))
					//slave_set_flg��Send_slave_cmd��RxReport5�������趨��slave_set_flg==0����������վ�ظ�
				{					
					crtl_cmd_num[pData[0]-33][pData[3]]=0;//��ʾ��վ�յ�����ָ������ٷ���д����
					return;	
				 }		
		 }
		if(pData[1]==0x10)
		{    		
        if((pData[0]>=33)&&(pData[3]>=4)&&(pData[3]<=15)&&(slave_set_flg==1))//slave_set_flg==1�����趨����������վ�ظ�
				{
				  ctrlslave_param_flg[pData[0]-33]=0;
					return;	
				}
				if((pData[3]>=16)&&(pData[3]<=33)&&(slave_set_flg==2))//slave_set_flg==2�����趨�����ɼ����Ϳ�������վ�ظ�
				{
				  cjqslave_param_flg[pData[0]-firstwx_xiabiao_i-1]=0;
					return;	
				}
				if((pData[3]>=34)&&(pData[3]<=35)&&(slave_set_flg==3))//slave_set_flg==3�����趨��վ��ַ���ŵ��Ļظ�
				{
				  slaveID_channel_flg=0;
					return;	
				}
		 }		
	 }
}

static void WriteDataToDMA_BufferTX1(uint16_t size)
{	  
	  TxFlag1=1;
//    USART_DMACmd(USART2, USART_DMAReq_Tx, DISABLE);  // ��������DMA����
//    DMA_Cmd(DMA1_Channel7, DISABLE);        //��ʼDMA����	
    DMA1_Channel4->CNDTR = (uint16_t)size; // ����Ҫ���͵��ֽ���Ŀ
	  USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);  // ��������DMA����
    DMA_Cmd(DMA1_Channel4, ENABLE);        //��ʼDMA����
}

static void WriteDataToDMA_BufferTX2(uint16_t size)
{	  
	  TxFlag2=1;
//    USART_DMACmd(USART2, USART_DMAReq_Tx, DISABLE);  // ��������DMA����
//    DMA_Cmd(DMA1_Channel7, DISABLE);        //��ʼDMA����	
    DMA1_Channel7->CNDTR = (uint16_t)size; // ����Ҫ���͵��ֽ���Ŀ
	  USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);  // ��������DMA����
    DMA_Cmd(DMA1_Channel7, ENABLE);        //��ʼDMA����
}
static void WriteDataToDMA_BufferTX3(uint16_t size)
{
	  TXENABLE3;
	  TxFlag3=1; 
    DMA1_Channel2->CNDTR = (uint16_t)size; // ����Ҫ���͵��ֽ���Ŀ
	  USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);  // ��������DMA����
    DMA_Cmd(DMA1_Channel2, ENABLE);        //��ʼDMA����
}
static void WriteDataToDMA_BufferTX4(uint16_t size)
{
//	  test_DMA=size;
	  TXENABLE4;
	  TxFlag4=1; 
    DMA2_Channel5->CNDTR = (uint16_t)size; // ����Ҫ���͵��ֽ���Ŀ
	  USART_DMACmd(UART4, USART_DMAReq_Tx, ENABLE);  // ��������DMA����
    DMA_Cmd(DMA2_Channel5, ENABLE);        //��ʼDMA����
}
void initialRealSendBuff(void)
{	
	real_send[0]=0x01;
	real_send[1]=0x00;
	real_send[2]=0x00;
	real_send[3]=0x00;
	real_send[4]=0x45;
	real_send[5]=0x00;
	real_send[6]=factory_gateway_set[2];
	real_send[7]=factory_gateway_set[3];
	real_send[8]=factory_gateway_set[4]; 	
	real_send[9]=factory_gateway_set[5];	  //��Ե����ID
	real_send[10]=0xCC;
  real_send[11]=0x01;
	real_send[12]=0x02;
	real_send[13]=0x03;
	real_send[14]=factory_gateway_set[6];
	real_send[15]=factory_gateway_set[7];
	real_send[16]=factory_gateway_set[8]; 	
	real_send[17]=factory_gateway_set[9];	
	real_send[18]=factory_gateway_set[5];	  //��Ե����ID
	real_send[19]=0x04;	//0x06 �����־
	real_send[20]=0x00;
	real_send[21]=0x01; //�Ĵ�����ʼ��ַ
	real_send[22]=0x50;
	real_send[23]=0x00;	//���ݳ���80���ֽ�	
}

/*��ƽ̨������·������*/
static void Net_connect_init(void)//���س�ʼ���������ӵ�ƽ̨ͨ·
{    
 switch(factory_gateway_set[0]) //��������=1 ���ţ�2 �ƶ���3 ��̫����4 WiFi��5 USB��6 �����ӣ�
 {
	//������  ��ʼ########################################
 case 0x00:
		if(factory_gateway_set[1]==0)net_connect_count=0;
		Start_timerEx(NET_INIT_EVT,1000);
	          break;
//������ ����######################################### 
//���� cdma ��ʼ---------------------------------
 case 0x01:		
		switch(factory_gateway_set[1]) //Э������=1 TCP/IP;2 MQTT; 3 SDK; 4 ������
		{
			//����TCP/IP
			case 0x01:
				cdma_tcp_init();//������cdma_mqtt_sdk_init
			break;			//����MQTT
			case 0x02:
				cdma_tcp_init();
			break;
			//����SDK
			case 0x03:
				cdma_tcp_init();
			break;
			//����other
			case 0x04:
				cdma_tcp_init();
			break;
			default:
				    Start_timerEx(NET_INIT_EVT,1000);
      			break;
		}			
	break;
//���� cdma ����------------------------------------
		
//�ƶ� gprs	��ʼ====================================
case 0x02:
		switch(factory_gateway_set[1]) //Э������=1 TCP/IP;2 MQTT; 3 SDK; 4 ������
		{
			//�ƶ�TCP/IP
			case 0x01:
				gprs_tcp_init();//�ú���������gprs_mqtt_init()��gprs_sdk_init();
			break;
			//�ƶ�MQTT
			case 0x02:
				gprs_tcp_init();//�ú���������gprs_mqtt_init()��gprs_sdk_init();
			break;
			//�ƶ�SDK
			case 0x03:
				gprs_tcp_init();//�ú���������gprs_mqtt_init()��gprs_sdk_init();
			break;
			//�ƶ�other
			case 0x04:
				gprs_other_init();
			break;
			default:
				    Start_timerEx(NET_INIT_EVT,1000);
      			break;
		}		
	 break;
//�ƶ� gprs	����====================================
	
//��̫�� ��ʼ++++++++++++++++++++++++++++++++++++++
case 0x03:
		switch(factory_gateway_set[1]) //Э������=1 TCP/IP;2 MQTT; 3 SDK; 4 ������
		{
			//����TCP/IP
			case 0x01:
				Ethernet_tcp_init();
			break;			//����MQTT
			case 0x02:
				Ethernet_mqtt_sdk_init();
			break;
			//����SDK
			case 0x03:
				Ethernet_mqtt_sdk_init();
			break;
			//����other
			case 0x04:
				Ethernet_other_init();
			break;
			default:
				Start_timerEx(NET_INIT_EVT,1000);
      			break;
		}			        
	 break;       
//��̫�� ����++++++++++++++++++++++++++++++++++++++++
	
//WiFi ��ʼ&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
case 0x04:
		Start_timerEx(NET_INIT_EVT,1000);
	          break;

//WiFi ����&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
	
//USB  ��ʼ########################################
case 0x05:
		Start_timerEx(NET_INIT_EVT,1000);
	          break;
//USB ����#########################################
	default:
		Start_timerEx(NET_INIT_EVT,1000);
      			break;
 }
 
} 

/* ��ģ��ͨѶ�����˼·����������������cdmaģ�鷢��������ݣ�1.��ʼ��cdmaģ�麯cdma_tcp_init��
2.�ظ�ƽ̨���������cmd_reply��3.CDMA��ƽ̨�ϱ����ݺ�cdma_tcp_send������cdma��������ݵĺ���ΪRxReport2����ˣ�����
send_message_type������¼�������ݵ����ͣ���0x01Ϊ��ʼ��cdmaģ�飬0x02Ϊ�ظ�ƽ̨�������0x03Ϊ��ƽ̨�ϱ����ݣ�0x04����Ϊ���ٻظ�����ƽ̨�Ŀ���ָ���
����cmd_flg����cdma_tcp_init��ʼ��ģ�����ִ�е�˳��module_send_flg_flg����cdma_tcp_send����ִ�е�˳�򣻺���cmd_replyֻ��һ��ָ��
���������ִ��˳��at_data_flg��־0x00Ϊ��atָ�0x01Ϊ����ָ�send_flag=0x01��ֹ��cdma�������������
����ָ����ϳ��⣩��send_flag=0x00������cdma������������ݡ��ڽ���cdma��������ݵĺ���ΪRxReport2��cdma_ok��־��
������������Ƿ���ȷ����ȷcdma_ok=1�����cdma����ERROR�����ظ�ִ��һ�θ�ָ����������������cdma��ʼ����
*/

static void cdma_tcp_init(void)  //����TCP/IP����
{
	send_flg=0x01;
	send_message_type=0x01;
	switch(cmd_flg) 
	{
		/*cmd_flg=0xff,�������*/
		case 0x01://ϵͳ��������
			cmd_flg=0x02;
			RESET_CTL_ENABLE;		  
		  Start_timerEx(NET_INIT_EVT,150);//ԭ��150:CDMA
			break;
		
		case 0x02:
			cmd_flg=0x05;
			RESET_CTL_DISABLE;		  
			Start_timerEx(NET_INIT_EVT,5000);//ԭ��20000��20s��
			break;
		
		case 0x03:
			cmd_flg=0x04;//cdma����
			TERM_ON_CTL_ENABLE;
		  Start_timerEx(NET_INIT_EVT,150);//CDAM:150
			break;
		case 0x04:
			cmd_flg=0x01;
			TERM_ON_CTL_DISABLE;
			Start_timerEx(NET_INIT_EVT,5000);//ԭ��20000;20s
			break;
			
		case 0x05:
//			WriteDataToBuffer(2,(unsigned char *)"ATE0\r\n",0,6); //�رջ���
      memcpy(USART2SendTCB,(unsigned char *)"ATE0\r\n",6);
      WriteDataToDMA_BufferTX2(6);	
			next_atdata_flg=0x06;
			Start_timerEx(NET_INIT_EVT,CMD_WAIT_TIME);
			break;
		
		case 0x06:			
//			WriteDataToBuffer(2,(unsigned char *)"AT^RSSIREP=0\r\n",0,14);//GSM:"AT+CNMI=3,2,0,0,0\r\n",19,800);��GSM�����ϱ����� 
		  memcpy(USART2SendTCB,(unsigned char *)"AT^RSSIREP=0\r\n",14);
      WriteDataToDMA_BufferTX2(14);	
		  next_atdata_flg=0x07;
			Start_timerEx(NET_INIT_EVT,CMD_WAIT_TIME);			
			break;
		
		case 0x07:
//			WriteDataToBuffer(2,(unsigned char *)"AT+IFC=2,2\r\n",0,12);//����DTE��DCE���ݴ�����Ʒ�ʽ,RTS��CTS
			memcpy(USART2SendTCB,(unsigned char *)"AT+IFC=2,2\r\n",12);
      WriteDataToDMA_BufferTX2(12);	
		  next_atdata_flg=0x08;
			Start_timerEx(NET_INIT_EVT,CMD_WAIT_TIME);		 
			break;
		
		case 0x08:
//			memcpy(USART2SendTCB,(unsigned char *)"AT+CMGD=0,4\r\n",13);//4��ʾɾ�����еĶ��ţ�����δ������;CDMAΪ0,4
//      WriteDataToDMA_BufferTX2(13);	
		  next_atdata_flg=0x09;
		  cmd_flg=0x09;
			Start_timerEx(NET_INIT_EVT,CMD_WAIT_TIME);      	
			break;
		case 0x09:
//			WriteDataToBuffer(2,(unsigned char *)"AT^IPDATMODE=1\r\n",0,16);//CDMA:"AT^IPDATMODE=1\r\n",16,800 �����µ�TCP/UP���ݵ����Ƿ������ϱ���1�ϱ�
			memcpy(USART2SendTCB,(unsigned char *)"AT^IPDATMODE=1\r\n",16);
      WriteDataToDMA_BufferTX2(16);	
		  next_atdata_flg=0x0A;
			Start_timerEx(NET_INIT_EVT,CMD_WAIT_TIME); 		  
			break;
		case 0x0A:
//			WriteDataToBuffer(2,(unsigned char *)"AT^RSSIREP=0\r\n",0,14);//TCP/UDP���ӳ�ʼ����CDMA:"AT^IPINIT=,\"CARD\",\"CARD\"\r\n",26,6000
  		memcpy(USART2SendTCB,(unsigned char *)"AT^RSSIREP=0\r\n",14);
      WriteDataToDMA_BufferTX2(14);	
		  next_atdata_flg=0x0B;
			Start_timerEx(NET_INIT_EVT,CMD_WAIT_TIME);      	
			break; 
		
		case 0x0B:			
//				WriteDataToBuffer(2,(unsigned char *)"AT+CNMI=1,2,0,0,0\r\n",0,19);//����Profile�����һ��ָ��  
		    //cdma�յ�����ֱ���ϱ�
		    memcpy(USART2SendTCB,(unsigned char *)"AT+CNMI=1,2,0,0,0\r\n",19);
        WriteDataToDMA_BufferTX2(19);	
       	next_atdata_flg=0x0C;				
			  Start_timerEx(NET_INIT_EVT,CMD_WAIT_TIME);
        break;					
		case 0x0C:				
//				WriteDataToBuffer(2,(unsigned char *)"AT^IPINIT=,\"CARD\",\"CARD\"\r\n",0,26);//����Profile����ڶ���ָ��
       //TCP/UDP ��·��ʼ�� CDMA:"AT^IPINIT=,\"CARD\",\"CARD\"\r\n",26
        memcpy(USART2SendTCB,(unsigned char *)"AT^IPINIT=,\"CARD\",\"CARD\"\r\n",26);
        WriteDataToDMA_BufferTX2(26);			
				next_atdata_flg=0x0D;
				Start_timerEx(NET_INIT_EVT,5*CMD_WAIT_TIME);
    		break;					
		case 0x0D://����Profile���������ָ��	
		   {
        unsigned char _ipopen[48]={"AT^IPOPEN=1,\"TCP\",\"115.239.134.165\",00502,5002\r\n"};//30���ֽ�	
				memcpy(_ipopen+19,factory_gateway_set+31,15);		//IP��ַ
				memcpy(_ipopen+36,factory_gateway_set+47,5);//�˿ں�	
        memcpy(USART2SendTCB,_ipopen,48);
        WriteDataToDMA_BufferTX2(48);				
				if(factory_gateway_set[1]==1)next_atdata_flg=0x15;//MQTTΪ0x0F,TCPΪ0x15
	      if(factory_gateway_set[1]==2)next_atdata_flg=0x0F;//MQTTΪ0x0F,TCPΪ0x15
	      if(factory_gateway_set[1]==3)next_atdata_flg=0x0F;//MQTTΪ0x0F,TCPΪ0x15
				if(factory_gateway_set[1]==4)next_atdata_flg=0x0F;//MQTTΪ0x0F,TCPΪ0x15
				Start_timerEx(NET_INIT_EVT,CMD_WAIT_TIME);
				break;
	     }
			 //MQTT��ʼ����ʼ
	 case 0x0F:
		   cmd_flg=0x10;			
				Start_timerEx(NET_INIT_EVT,DATA_WAIT_TIME);//����TCP��·��Ҫ�ĵȴ�ʱ��				
			 break;
		case 0x10:	//MQTT�������ӿ�ʼATָ��
         mqtt_connect();//���������ӵ������͵�ReportData2�������㳤�ȵ�mqtt_len
         bytelen_to_asc((unsigned char *)message_len_char1,mqtt_len);		
				 send_at_cdma((unsigned char *)message_len_char1,21);//mqtt���ݰ�����Ϊ41,�ȷ�atָ����淢����
				 next_atdata_flg=0x11;
         Start_timerEx(NET_INIT_EVT,CMD_WAIT_TIME);
        break;
		case 0x11:   //MQTT�������ӿ�ʼDATAָ��
        mqtt_connect();//���������ӵ������͵�ReportData2�������㳤�ȵ�mqtt_len
        bytelen_to_asc((unsigned char *)message_len_char1,mqtt_len);						
				send_data_module(ReportData2,(unsigned char *)message_len_char1);//mqtt���ݰ�����Ϊ41
				next_atdata_flg=0x12;			
				Start_timerEx(NET_INIT_EVT,CMD_WAIT_TIME);				
			 break;
   case 0x12://MQTT �����ظ��ȴ�ʱ��
		    cmd_flg=0x13;
        send_flg=0x00;//�������MQTT�ظ�
        send_message_type=0x00;	 
				Start_timerEx(NET_INIT_EVT,DATA_WAIT_TIME);				
			 break;
		//MQTT�������ӽ������������⿪ʼ
		case 0x13:
       send_flg=0x01;
       mqtt_subscribe();		  //�������� ������������������͵�ReportData2�������㳤�ȵ�mqtt_len
		   bytelen_to_asc((unsigned char *)message_len_char1,mqtt_len);	
			 send_at_cdma((unsigned char *)message_len_char1,21);//�����������ݰ�����Ϊ:message_len_char���ȷ�atָ����淢����
			 next_atdata_flg=0x14;
		   Start_timerEx(NET_INIT_EVT,3*CMD_WAIT_TIME);				
			 break;
    case 0x14:
			 mqtt_subscribe();		  //�������� ������������������͵�ReportData2�������㳤�ȵ�mqtt_len
		   bytelen_to_asc((unsigned char *)message_len_char1,mqtt_len);	
			 send_data_module(ReportData2,(unsigned char *)message_len_char1);//�����������ݰ�����Ϊ:message_len_char
			 next_atdata_flg=0x15;			 		     
	    Start_timerEx(NET_INIT_EVT,CMD_WAIT_TIME);
		   break;
		//�����������
		//MQTT��ʼ������
		case 0x15: 
		  send_flg=0x00;//����ظ�ƽ̨���������cmd_reply����ƽ̨�ϱ����ݺ���report8����gsm��cdmaģ�鷢����
		  send_message_type=0x00;
		  module_send_flg = 0x01;//TCBЭ������CDMA��ƽ̨��������ִ�в�����Ʊ�־
		  cmd_flg=0xFF;//CDMA��ʼ�����
		  net_connect_count=0;
		  send_message_type=0x00;
		  Start_timerEx(SEND_PLATFORM_EVT, DATA_WAIT_TIME );//����CDMA��ƽ̨��������
			break;
	  default:
			if(cmd_flg!=0xFF){while(1);}
    break;
	}
}

//static void cdma_other_init(void) //����other����
//{
//}

static void gprs_tcp_init(void) //�ƶ�TCP/IP����
{
	send_flg=0x01;
	send_message_type=0x01;
	switch(cmd_flg) 
	{
		/*cmd_flg=0xff,���³�ʼ��cdmaģ�飻0xfd��rest��ʼ��0xfc��rest����(10ms<restʱ��<2s)��0xfb CDMA����ʹ�ܣ�0xfaʹ�ܽ���*/
		case 0x01://ϵͳ��������
			cmd_flg=0x01;//gprsģ��REST��ʹģ��������ֻ�йػ�������������������
		  TERM_ON_CTL_ENABLE;//gprs�ػ�
//			RESET_CTL_ENABLE;
		  Start_timerEx(NET_INIT_EVT,1100);//ԭ��150:CDMA;GSM:50
			break;
		case 0x02:
			cmd_flg=0x05;
		  TERM_ON_CTL_DISABLE;
//			RESET_CTL_DISABLE;
			Start_timerEx(NET_INIT_EVT,3000);//ԭ��20000��20s��
			break;		
		case 0x03:
			cmd_flg=0x04;//cdma����
			TERM_ON_CTL_ENABLE;
		  Start_timerEx(NET_INIT_EVT,1100);//CDAM:150;GSM:1500(>1S,<2.6S)
			break;
		case 0x04:
			cmd_flg=0x05;
			TERM_ON_CTL_DISABLE;
			Start_timerEx(NET_INIT_EVT,3000);//ԭ��20s
			break;
		
		case 0x05:
			memcpy(USART2SendTCB,(unsigned char *)"ATE0\r\n",6);
      WriteDataToDMA_BufferTX2(6);	
//		  WriteDataToBuffer(2,"ATE0\r\n",0,6); //�رջ���				
			next_atdata_flg=0x06;
			Start_timerEx(NET_INIT_EVT,CMD_WAIT_TIME);
			break;
		
		case 0x06:
      memcpy(USART2SendTCB,(unsigned char *)"AT+CNMI=3,2,0,0,0\r\n",19);
      WriteDataToDMA_BufferTX2(19);				
//			WriteDataToBuffer(2,"AT+CNMI=3,2,0,0,0\r\n",0,19);//GSM:"AT+CNMI=3,2,0,0,0\r\n",19,800);��GSM�����ϱ����� 
		  next_atdata_flg=0x07;
			Start_timerEx(NET_INIT_EVT,CMD_WAIT_TIME);			
		//CDMA: 0xF8,0xF9,0xF7,"AT^RSSIREP=0\r\n",14,800
			break;
		
		case 0x07:
			memcpy(USART2SendTCB,(unsigned char *)"AT+IFC=2,2\r\n",12);
      WriteDataToDMA_BufferTX2(12);	
//			WriteDataToBuffer(2,"AT+IFC=2,2\r\n",0,12);//����DTE��DCE���ݴ�����Ʒ�ʽ,RTS��CTS
			next_atdata_flg=0x08;
			Start_timerEx(NET_INIT_EVT,CMD_WAIT_TIME);		 
			break;
		
		case 0x08:
			memcpy(USART2SendTCB,(unsigned char *)"AT+CMGD=1,4\r\n",13);
      WriteDataToDMA_BufferTX2(13);	
//			WriteDataToBuffer(2,"AT+CMGD=1,4\r\n",0,13);//4��ʾɾ�����еĶ��ţ�����δ������;CDMAΪ0,4;GSMΪ1,4;������GSM��ͬ
			next_atdata_flg=0x09;
			Start_timerEx(NET_INIT_EVT,CMD_WAIT_TIME);      	
			break;
		case 0x09:
			memcpy(USART2SendTCB,(unsigned char *)"AT^SICS=0,conType,GPRS0\r\n",25);
      WriteDataToDMA_BufferTX2(25);
//			WriteDataToBuffer(2,"AT^SICS=0,conType,GPRS0\r\n",0,25);//CDMA:"AT^IPDATMODE=1\r\n",16,800 �����µ�TCP/UP���ݵ����Ƿ������ϱ���1�ϱ�
		//GSM:��������Profile��Ҫ����ָ������ǵ�һ��ָ��.�����µ�TCP/UP���ݵ��������ϱ���GSM��Ĭ��ֵ�������趨
			next_atdata_flg=0x0A;
			Start_timerEx(NET_INIT_EVT,CMD_WAIT_TIME); 		  
			break;
		case 0x0A:
			memcpy(USART2SendTCB,(unsigned char *)"AT^SICS=0,apn,jmwg\r\n",20);
      WriteDataToDMA_BufferTX2(20);	
//			WriteDataToBuffer(2,"AT^SICS=0,apn,jmwg\r\n",0,20);//TCP/UDP���ӳ�ʼ����CDMA:"AT^IPINIT=,\"CARD\",\"CARD\"\r\n",26,6000
		//GSM:��������Profile�ĵڶ���ָ�"AT^SICS=0,apn,jmwg\r\n",20,800	
			next_atdata_flg=0x0B;
			Start_timerEx(NET_INIT_EVT,CMD_WAIT_TIME);      	
			break; 
		
		/*����ҳƽ̨������·*/
		 //WriteDataToBuffer(2,"AT^SISC=0\r\n",0,11);//�ر���·1#��CDMAΪ"AT^IPCLOSE=1\r\n",0,14��GSMΪ"AT^SISC=1\r\n",0,11
			
		case 0x0B:
        memcpy(USART2SendTCB,(unsigned char *)"AT^SISS=0,srvType,socket\r\n",26);
        WriteDataToDMA_BufferTX2(26);			
//				WriteDataToBuffer(2,"AT^SISS=0,srvType,socket\r\n",0,26);//����Profile�����һ��ָ��				
				next_atdata_flg=0x0C;				
			  Start_timerEx(NET_INIT_EVT,CMD_WAIT_TIME);
        break;					
		case 0x0C:
        memcpy(USART2SendTCB,(unsigned char *)"AT^SISS=0,conId,0\r\n",19);
        WriteDataToDMA_BufferTX2(19);				
//				WriteDataToBuffer(2,"AT^SISS=0,conId,0\r\n",0,19);//����Profile����ڶ���ָ��				
				next_atdata_flg=0x0D;
				Start_timerEx(NET_INIT_EVT,CMD_WAIT_TIME);
        break;					
		case 0x0D://����Profile���������ָ��	
		   {
				unsigned char _ipopen[53]={"AT^SISS=0,address,\"socktcp://115.239.134.165:00502\"\r\n"};//29���ֽ�,GSM���CDMA����ȫ����	
				memcpy(_ipopen+29,factory_gateway_set+31,15);		//IP��ַ
				memcpy(_ipopen+45,factory_gateway_set+47,5);//�˿ں�	 
//				memcpy(_ipopen+29,server_ip,20);		//IP��ַ:�˿ں�"
//				_ipopen[49]='"';
//				_ipopen[50]='\r';_ipopen[51]='\n';	
        memcpy(USART2SendTCB,_ipopen,53);
        WriteDataToDMA_BufferTX2(53);					
//				WriteDataToBuffer(2,_ipopen,0,52);//����ΪAT^SISS=0,address,"socktcp://183.056.016.057:09988"				
				next_atdata_flg=0x0E; 
				Start_timerEx(NET_INIT_EVT,CMD_WAIT_TIME);
				break;
	     }
	case 0x0E:
        memcpy(USART2SendTCB,(unsigned char *)"AT^SISO=0\r\n",11);//�ƶ�ʹ��
        WriteDataToDMA_BufferTX2(11);				
//				WriteDataToBuffer(2,"AT^SISO=0\r\n",0,11);//����Profile���������ָ��;��0#����
				if(factory_gateway_set[1]==1)next_atdata_flg=0x15;//MQTTΪ0x0F,TCPΪ0x15
	      if(factory_gateway_set[1]==2)next_atdata_flg=0x0F;//MQTTΪ0x0F,TCPΪ0x15
	      if(factory_gateway_set[1]==3)next_atdata_flg=0x0F;//MQTTΪ0x0F,TCPΪ0x15
				Start_timerEx(NET_INIT_EVT,2*CMD_WAIT_TIME);
        break;				
			//MQTT��ʼ����ʼ
	  case 0x0F:
		   cmd_flg=0x10;			
				Start_timerEx(NET_INIT_EVT,DATA_WAIT_TIME);//����TCP��·��Ҫ�ĵȴ�ʱ��				
			 break;
		case 0x10:	//MQTT�������ӿ�ʼATָ��
         mqtt_connect();//���������ӵ������͵�ReportData2�������㳤�ȵ�mqtt_len
         bytelen_to_asc((unsigned char *)message_len_char1,mqtt_len);		
				 send_at_gprs((unsigned char *)message_len_char1,15);//mqtt���ݰ�����Ϊ41,�ȷ�atָ����淢����
				 next_atdata_flg=0x11;
         Start_timerEx(NET_INIT_EVT,CMD_WAIT_TIME);
        break;
		case 0x11:   //MQTT�������ӿ�ʼDATAָ��
        mqtt_connect();//���������ӵ������͵�ReportData2�������㳤�ȵ�mqtt_len
        bytelen_to_asc((unsigned char *)message_len_char1,mqtt_len);						
				send_data_module(ReportData2,(unsigned char *)message_len_char1);//mqtt���ݰ�����Ϊ41
				next_atdata_flg=0x12;			
				Start_timerEx(NET_INIT_EVT,CMD_WAIT_TIME);				
			 break;
   case 0x12://MQTT �����ظ��ȴ�ʱ��
		    cmd_flg=0x13;
        send_flg=0x00;//�������MQTT�ظ�
        send_message_type=0x00;	 
				Start_timerEx(NET_INIT_EVT,DATA_WAIT_TIME);				
			 break;
		//MQTT�������ӽ������������⿪ʼ
		case 0x13:
       send_flg=0x01;
       mqtt_subscribe();		  //�������� ������������������͵�ReportData2�������㳤�ȵ�mqtt_len
		   bytelen_to_asc((unsigned char *)message_len_char1,mqtt_len);	
			 send_at_gprs((unsigned char *)message_len_char1,15);//�����������ݰ�����Ϊ:message_len_char���ȷ�atָ����淢����
			 next_atdata_flg=0x14;
		   Start_timerEx(NET_INIT_EVT,3*CMD_WAIT_TIME);				
			 break;
    case 0x14:
			 mqtt_subscribe();		  //�������� ������������������͵�ReportData2�������㳤�ȵ�mqtt_len
		   bytelen_to_asc((unsigned char *)message_len_char1,mqtt_len);	
			 send_data_module(ReportData2,(unsigned char *)message_len_char1);//�����������ݰ�����Ϊ:message_len_char
			 next_atdata_flg=0x15;			 		     
	    Start_timerEx(NET_INIT_EVT,CMD_WAIT_TIME);
		   break;
//		//�����������
//		//MQTT��ʼ������
		case 0x15:      	
			send_flg=0x00;//����ظ�ƽ̨���������cmd_reply����ƽ̨�ϱ����ݺ���report8����gsm��cdmaģ�鷢����
		  send_message_type=0x00;
		  module_send_flg = 0x01;
		  cmd_flg=0xFF;//��ʼ��������cmd_flg��Ϊ0xFF�����������й�����һֱ����
		  net_connect_count=0;		  
		  Start_timerEx(SEND_PLATFORM_EVT, DATA_WAIT_TIME );//��CDMA������		      	  
			break;
		default:
			if(cmd_flg!=0xFF){while(1);}
    break;
	}
}

static void gprs_other_init(void) //�ƶ�other����
{
 Start_timerEx(NET_INIT_EVT,CMD_WAIT_TIME);
}

static void Ethernet_tcp_init(void) //����TCP/IP����
{
  send_flg=0x00;//����ظ�ƽ̨���������cmd_reply����ƽ̨�ϱ����ݺ���report8����gsm��cdmaģ�鷢����
	send_message_type=0x00;
	module_send_flg = 0x01;//TCBЭ������CDMA��ƽ̨��������ִ�в�����Ʊ�־
	cmd_flg=0xFF;//CDMA��ʼ�����
	net_connect_count=0;
	Start_timerEx(SEND_PLATFORM_EVT, DATA_WAIT_TIME );//����CDMA��ƽ̨��������
}

static void Ethernet_mqtt_sdk_init(void) //����mqtt_sdk����
{
 Start_timerEx(NET_INIT_EVT,CMD_WAIT_TIME);
}

static void Ethernet_other_init(void) //����other����
{
	Start_timerEx(NET_INIT_EVT,CMD_WAIT_TIME);
}

/*��ƽ̨�������ݺ�����-------------------------------------------------------------------------------------*/
static void send_platform(void) //��ƽ̨��������
{
 switch(factory_gateway_set[0]) //��������=1 ���ţ�2 �ƶ���3 ��̫����4 WiFi��5 USB��0 �����ӣ�
 {
	 //������
	case 0x00:
		halt_RxReport2++;
		if(factory_gateway_set[1]==0){halt_RxReport2=0;cmd_flg=0xFF;}
    Start_timerEx(SEND_PLATFORM_EVT, DATA_WAIT_TIME );//����CDMA��ƽ̨��������	
	break;
//���� cdma ��ʼ---------------------------------
  case 0x01:		
		switch(factory_gateway_set[1]) //Э������=1 TCP/IP;2 MQTT; 3 SDK; 4 ������
		{
			//����TCP/IP
			case 0x01:
				cdma_tcp_send();
			break;
			//����MQTT
			case 0x02:
				cdma_mqtt_send();
			break;
			//����SDK
			case 0x03:
				cdma_sdk_send();
			break;
			//����other
			case 0x04:
				cdma_sdk_send();
			break;
		  default:
			Start_timerEx(SEND_PLATFORM_EVT, DATA_WAIT_TIME );//����CDMA��ƽ̨��������
      			break;
		}			
	break;
//���� cdma ����------------------------------------
		
//�ƶ� gprs	��ʼ====================================
	case 0x02:
		switch(factory_gateway_set[1]) //Э������=1 TCP/IP;2 MQTT; 3 SDK; 4 ������
		{
			//�ƶ�TCP/IP
			case 0x01:
				cdma_tcp_send();//�����ƶ�TCP/IP�������� gprs_tcp_send
			break;
			//�ƶ�MQTT
			case 0x02:
				cdma_mqtt_send();//�����ƶ�mqtt�������� gprs_tcp_send
			break;
			//�ƶ�SDK
			case 0x03:
				cdma_sdk_send();//�����ƶ�sdk�������� gprs_tcp_send
			break;
			//�ƶ�other
			case 0x04:
				gprs_other_send();
			break;
			default:
				Start_timerEx(SEND_PLATFORM_EVT, DATA_WAIT_TIME );//����CDMA��ƽ̨��������
      			break;
		}
	 break;
//�ƶ� gprs	����====================================
	
//��̫�� ��ʼ++++++++++++++++++++++++++++++++++++++
	case 0x03:
		switch(factory_gateway_set[1]) //Э������=1 TCP/IP;2 MQTT; 3 SDK; 4 ������
		{
			//����TCP/IP
			case 0x01:
				Ethernet_tcp_send();
			break;
			//����MQTT
			case 0x02:
				Ethernet_mqtt_send();
			break;
			//����SDK
			case 0x03:
				Ethernet_sdk_send();
			break;
			//����other
			case 0x04:
				Ethernet_other_send();
			break;
		  default:
				Start_timerEx(SEND_PLATFORM_EVT, DATA_WAIT_TIME );//����CDMA��ƽ̨��������
      			break;
		}			
	  break;
//��̫�� ����++++++++++++++++++++++++++++++++++++++++
	
//WiFi ��ʼ&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
	case 0x04:
		halt_RxReport2++;
		Start_timerEx(SEND_PLATFORM_EVT, DATA_WAIT_TIME );//����CDMA��ƽ̨��������
	          break;

//WiFi ����&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
	
//USB  ��ʼ########################################
	case 0x05:
		halt_RxReport2++;
		Start_timerEx(SEND_PLATFORM_EVT, DATA_WAIT_TIME );//����CDMA��ƽ̨��������
	          break;
//USB ����#########################################
	default:
		halt_RxReport2++;
		Start_timerEx(SEND_PLATFORM_EVT, DATA_WAIT_TIME );//����CDMA��ƽ̨��������
  break;
 }
}

/*��ƽ̨���ͺ���--------------------------------------------------------------------------------------------------------*/
/*�ɼ�����ַ˵����
0x0100-0x011F 0#�ɼ�����0x0120-0x013F 1#�ɼ�����������
0x0100-0x01E0 0#-7#�ɼ�����0x0200-0x02E0 8#-15#�ɼ�����0x0300-0x03E0 16#-23#�ɼ�����0x0400-0x04E0 24#-31#�ɼ�����
0x0500-0x05E0 32#-39#���Ʋɼ�����0x0600-0x06E0 40#-47#���Ʋɼ�����0x0700-0x07E0 40#-47#���Ʋɼ�����0x0800-0x08E0 48#-63#���Ʋɼ�����
0x0900-0x091F ���زɼ�����0x0920-0x0923 �ϵ�ָʾ�ϱ�*/

static void cdma_tcp_send(void) //����TCP/IP�������ݣ��ʱ��600���ϱ�һ�����ݣ��ɼ�������+2*������������*6��	
{
	u16 reportCrcValue = 0;    		
	if(send_mess)		//���ڷ��Ͷ����ϱ�ʱ���ȴ�2s������ִ��
	{
		Start_timerEx(SEND_PLATFORM_EVT,2000);
		return;
	}	
	
//	if(send_flg ==0x01||send_flg ==0x04||send_flg ==0x05) //����վ��ʼ���������ZZ_Wired_flag[65]=1��||ZZ_Wireles_flag[65]==1
// 	{		
//		Start_timerEx(SEND_PLATFORM_EVT,500);
//		return;
//	}	
	if(send_message_type !=0x03&&send_message_type !=0x00) //����վ��ʼ���������ZZ_Wired_flag[65]=1��||ZZ_Wireles_flag[65]==1
 	{		
		Start_timerEx(SEND_PLATFORM_EVT,500);
		return;
	}	
	send_message_type=0x03;	//���յ�GSM��ERROR����ʱʹ�ã�0x03Ϊ��ƽ̨�ϱ�����
	
	/*send_message_type��¼��cdma������������ͣ�0x01Ϊ��ʼ������0x02Ϊ��ƽ̨���ؿ������0x03Ϊ��ƽ̨�ϱ����ݡ�send_flg��¼
	��cdma���������С���ͣ�0x01Ϊ��ʼ�������atָ���dataָ�0x02Ϊƽ̨�ϱ����ݵ�atָ�0x03Ϊƽ̨�ϱ����ݵ�dataָ��
	0x04Ϊƽ̨���ؿ��Ƶ�atָ�0x05Ϊƽ̨���ؿ��Ƶ�dataָ���Ҫ��ƽ̨��ָ���һ���лظ�,
	�ڽ��ճ�������next_atdata_flg���Ƴ���ִ��˳�򣬲���ƽ̨��ָ��ģ�����module_send_flg���Ƴ���ִ�С�	
	*/

	switch(module_send_flg)//0x0100~0x04FF�ɼ���������;0x500~0x08FF������������;0x0900~0x091F���ؼ�����;0x0920�ϵ�ָʾ
	{
		case 0x01:
      send_message_len1=6+18+4+3;//6+18+ʵ���ֽ���ֵ+3
      bytelen_to_asc((unsigned char *)message_len_char1,send_message_len1);//send_message_lenȫ�ֱ���      		
		  if(factory_gateway_set[0]==1)send_at_cdma((unsigned char *)message_len_char1,21);//TCB,CDMA	����Ϊ31=6+��Ϣ����=6+25=31��CDMA��ATָ���Ϊ21��GPRS��ATָ���Ϊ15
		  if(factory_gateway_set[0]==2)send_at_gprs((unsigned char *)message_len_char1,15);//TCB,GPRS
			next_atdata_flg = 0x02;
			send_flg   = 0x02; //��ʾ��ƽ̨�ϱ���atָ������ִ�У�����ֻ��ִ��dataָ��
			Start_timerEx(SEND_PLATFORM_EVT,CMD_WAIT_TIME);		 
			break;
		case 0x02:
			real_send[4] =0x19;//��Ϣ����25,18+ʵ���ֽ���ֵ+3=18+real_send[22]+3=18+4+3=25
			real_send[20]=0x20;//0x0920�ϵ�ָʾƽ̨��ַ���ֽ�;0x0100~0x04FF�ɼ���������
			real_send[21]=0x09;//0x0920�ϵ�ָʾƽ̨��ַ���ֽ�
			
			real_send[22]=0x04;//ʵ�ʷ����ֽ������ֽ�
			real_send[23]=0x00;//ʵ�ʷ����ֽ������ֽ�
			
			real_send[24]=0x00;
			real_send[25]=0x00;
			real_send[26]=0x01;
			real_send[27]=0x00;//���͵��ֽڣ�ÿ������4�ֽڣ�ǰ2���ֽھ�Ϊ0����2���ֽ�Ϊ��Чֵ�����ֽ���ǰ

			reportCrcValue = GetCRC16(real_send+11, 17);//8(�ɿ���ID)+1(����)+2(��ʼ��ַ)+2(�ֽ�������)=13+4(�ֽ�ֵ)=17

			real_send[28] = reportCrcValue&0x00FF;
			real_send[29] = (reportCrcValue&0xFF00)>>8;
			real_send[30] = 0xDD;
		  send_message_len1=6+18+real_send[22]+3;//6+18+ʵ���ֽ���ֵ+3
      bytelen_to_asc((unsigned char *)message_len_char1,send_message_len1);//send_message_lenȫ�ֱ���	
		  send_data_module(real_send,(unsigned char *)message_len_char1);//TCB	
			next_atdata_flg = 0x03;
			send_flg   = 0x03;//��ʾ��ƽ̨�ϱ���dataָ������ִ��
			Start_timerEx(SEND_PLATFORM_EVT,CMD_WAIT_TIME);		 
			break;
		case 0x03:
			module_send_flg=0x04;
			send_flg=0x00;
		  send_message_type=0x00;
		  real_send[4]=0x25;//��Ϣ����37��18+ʵ���ֽ���ֵ+3=18+real_send[22]+3=18+16+3=37=0x25
		  real_send[20]=0x00;//ƽ̨��ַ���ֽ�
		  real_send[21]=0x21;//ƽ̨��ַ���ֽ�
			real_send[22]=0x10;//ʵ�ʷ����ֽ������ֽ�,2*2*4(���Ƶ�)��1��������Ϊһ���ϱ�
		  real_send[23]=0x00;//ʵ�ʷ����ֽ������ֽ�
		  sendnum_mflg=0x00;
			Start_timerEx(SEND_PLATFORM_EVT,INTER_SEND);
			break;
		
		//����������״̬�ϱ���ʼ
		case 0x04:  //module_send_flg
	 //����ϵͳ��ʼ����ɺ󣬿�������վ������ڣ�ͬʱ��������վIDС��32����0~31��������ķ�����վ��������;
      while((ZZ_Wireles_flag[32+sendnum_mflg]==0)&&(ZZ_Wired_flag[32+sendnum_mflg]==0)&&(sendnum_mflg<32))//CSH_Wired_finish==0|
       {
				sendnum_mflg++;
			 }
			if(sendnum_mflg<32)
			{           						
    	 if(send_flg==0x00)
				{
			   //send_at_cdma("094",21);//MQTT����Ϊ94��TCP����Ϊ75��+19���ֽڣ����������ⳤ�Ȳ��ܱ䣬������ӣ������Ӧ���ֽ���
         send_message_len1=6+18+real_send[22]+3;//6+18+ʵ���ֽ���ֵ+3
         bytelen_to_asc((unsigned char *)message_len_char1,send_message_len1);//send_message_lenȫ�ֱ���						
			   if(factory_gateway_set[0]==1)send_at_cdma((unsigned char *)message_len_char1,21);//TCB,CDMA	����Ϊ31=6+��Ϣ����=6+25=31��CDMA��ATָ���Ϊ21��GPRS��ATָ���Ϊ15
		     if(factory_gateway_set[0]==2)send_at_gprs((unsigned char *)message_len_char1,15);//TCB,GPRS  TCP ����Ϊ43=6+��Ϣ����=6+37=43��CDMA��ATָ���Ϊ21��GPRS��ATָ���Ϊ15	
      	 send_flg=0x02;
         next_atdata_flg = 0x04;
         Start_timerEx(SEND_PLATFORM_EVT,CMD_WAIT_TIME);		 
			break;					
			  }
			if(send_flg==0x02)
			{
			real_send[20]=0x00;//ƽ̨��ַ���ֽ�
		  real_send[21]=0x21;//ƽ̨��ַ���ֽ�
			alter_send(0x10,sendnum_mflg);//����ÿ��������ϱ�״̬���ݵĿ�ʼ��ַ,�޸���real_send�����ݵ�ַ(2100H,2130H,2160H,...)
			//mqtt_publish(real_send,75);//MQTT ʵ�ʷ���75���ֽ�,�����75����Ϊ�˳���ɶ���û���á�	
			//send_data_module(mqtt_real_send,"094");
      send_message_len1=6+18+real_send[22]+3;//6+18+ʵ���ֽ���ֵ+3
      bytelen_to_asc((unsigned char *)message_len_char1,send_message_len1);//send_message_lenȫ�ֱ���					
	    send_data_module(real_send,(unsigned char *)message_len_char1);//TCB����Ϊ43=6+��Ϣ����=6+37=43��CDMA��ATָ���Ϊ21��GPRS��ATָ���Ϊ15	
			next_atdata_flg=0x4;
			send_flg=0x03;					
			Start_timerEx(SEND_PLATFORM_EVT,CMD_WAIT_TIME);
			break;
			}
			if(send_flg==0x03)
			{
			module_send_flg=0x4;//1�������ϱ���ɣ��ȴ�5S������1�������ϱ�
			send_flg=0x00;
			send_message_type=0x00;
			sendnum_mflg++;				
			Start_timerEx(SEND_PLATFORM_EVT,INTER_SEND);
			break;
			}
		}
			else
		{
			module_send_flg=0x5;
      next_atdata_flg=0x05;			
			send_flg=0x00;
			send_message_type=0x00;
			real_send[4]=0x35;//��Ϣ����45��18+ʵ���ֽ���ֵ+3=18+real_send[22]+3=18+24+3=45=0x2D
			real_send[20]=0x00;//ƽ̨��ַ���ֽ�
		  real_send[21]=0x01;//ƽ̨��ַ���ֽ�
			real_send[22]=0x20;//ʵ�ʷ����ֽ������ֽ�2*2*(6+2)(��������)=32=0x20
			real_send[23]=0x00;//ʵ�ʷ����ֽ������ֽ�
			sendnum_mflg=0x00;		
			Start_timerEx(SEND_PLATFORM_EVT,INTER_MESS);
		  break;
		 }		
		
  /*����������״̬�����ϱ�����*/

	//�ɼ������ݡ����������ݼ����زɼ������ϱ���ʼ
		case 0x05:  //module_send_flg
			//����ϵͳ��ʼ����ɺ󣬲ɼ����Ϳ�������վ�����ڣ�ͬʱ��������վIDС�ڵ���64����0~65��������ķ�����վ��������;
			  
     while((ZZ_Wireles_flag[sendnum_mflg]==0)&&(ZZ_Wired_flag[sendnum_mflg]==0)&&(sendnum_mflg<=64))//CSH_Wired_finish==0|
      {
			 sendnum_mflg++;
			}
		if(sendnum_mflg<=64) //���������Ƿ��ѷ�����ɣ�û�з����꣬��������.��sendnum_mflg�Ǽ�¼�ڼ�����վ�ϱ�
		{
      if(send_flg==0x00)
      {	
       send_message_len1=6+18+real_send[22]+3;//6+18+ʵ���ֽ���ֵ+3
      bytelen_to_asc((unsigned char *)message_len_char1,send_message_len1);//send_message_lenȫ�ֱ���	 				
      //send_at_cdma("094",21);//MQTT����Ϊ94��TCP����Ϊ75��+19���ֽڣ����������ⳤ�Ȳ��ܱ䣬������ӣ������Ӧ���ֽ���(�Ϊ256)
		  if(factory_gateway_set[0]==1)send_at_cdma((unsigned char *)message_len_char1,21);//TCB,CDMA	����Ϊ31=6+��Ϣ����=6+25=31��CDMA��ATָ���Ϊ21��GPRS��ATָ���Ϊ15
		  if(factory_gateway_set[0]==2)send_at_gprs((unsigned char *)message_len_char1,15);//TCB,GPRS  TCB ����=6+��Ϣ����=6+45=51��CDMA��ATָ���Ϊ21��GPRS��ATָ���Ϊ15	
			send_flg=0x02;
      next_atdata_flg = 0x05;
      Start_timerEx(SEND_PLATFORM_EVT,CMD_WAIT_TIME);		 
			break;					
			}
			if(send_flg==0x02)
			{
			real_send[20]=0x00;//ƽ̨��ַ���ֽ�
		  real_send[21]=0x01;//ƽ̨��ַ���ֽ�	
		  chge_coltsnd(sendnum_mflg);//����ÿ��ɼ����ϱ����ݵĿ�ʼ��ַ,�޸���real_send�����ݵ�ַ(0100H,0130H,0160H,...)
			send_message_len1=6+18+real_send[22]+3;//6+18+ʵ���ֽ���ֵ+3
      bytelen_to_asc((unsigned char *)message_len_char1,send_message_len1);//send_message_lenȫ�ֱ���	 		
			//mqtt_publish(real_send,75);//MQTT ʵ�ʷ���75���ֽ�,�����75����Ϊ�˳���ɶ���û����.		
		  //send_data_module(mqtt_real_send,"094");	 
	    send_data_module(real_send,(unsigned char *)message_len_char1);//TCB����=6+��Ϣ����=6+2*4+45=51+8��
			next_atdata_flg=0x5;
			send_flg=0x03;					
			Start_timerEx(SEND_PLATFORM_EVT,CMD_WAIT_TIME);
			break;
			}
			if(send_flg==0x03)
			{
			module_send_flg=0x5;//1�������ϱ���ɣ��ȴ�5S������1�������ϱ�
			send_flg=0x00;
			send_message_type=0x00;
			sendnum_mflg++;				
			Start_timerEx(SEND_PLATFORM_EVT,INTER_SEND);
			break;
			}      		
		}
		else
		{
			module_send_flg=0x06;
			send_flg=0x00;
			send_message_type=0x00;
		  real_send[4]=0x19;//��Ϣ����25��18+ʵ���ֽ���ֵ+3=18+real_send[22]+3=18+4+3=25=0x19
		  real_send[20]=0x00;//ƽ̨��ַ���ֽ�
		  real_send[21]=0x25;//ƽ̨��ַ���ֽ�
			real_send[22]=0x04;//ʵ�ʷ����ֽ������ֽ�,2*2*1(�Կػ�·)��1���Կػ�·Ϊһ���ϱ�
		  real_send[23]=0x00;//ʵ�ʷ����ֽ������ֽ�
		  sendnum_mflg=0x00;
			Start_timerEx(SEND_PLATFORM_EVT,INTER_SEND);
			break;
		 }	  
		//���زɼ������ϱ�����
		 
		 //�Կ�״̬�ϱ���ʼ
		case 0x06:  //module_send_flg
	 //����ϵͳ��ʼ����ɺ󣬿�������վ������ڣ�ͬʱ��������վIDС��32����0~31��������ķ�����վ��������;
      while((hand_auto_count[sendnum_mflg]==0)&&(sendnum_mflg<73))//CSH_Wired_finish==0|
       {
				sendnum_mflg++;
			 }			 
			if(sendnum_mflg<73)
			{           						
    	 if(send_flg==0x00)
				{
			   //send_at_cdma("094",21);//MQTT����Ϊ94��TCP����Ϊ75��+19���ֽڣ����������ⳤ�Ȳ��ܱ䣬������ӣ������Ӧ���ֽ���
         send_message_len1=6+18+real_send[22]+3;//6+18+ʵ���ֽ���ֵ+3
         bytelen_to_asc((unsigned char *)message_len_char1,send_message_len1);//send_message_lenȫ�ֱ���						
			   if(factory_gateway_set[0]==1)send_at_cdma((unsigned char *)message_len_char1,21);//TCB,CDMA	����Ϊ31=6+��Ϣ����=6+25=31��CDMA��ATָ���Ϊ21��GPRS��ATָ���Ϊ15
		     if(factory_gateway_set[0]==2)send_at_gprs((unsigned char *)message_len_char1,15);//TCB,GPRS  TCP ����Ϊ43=6+��Ϣ����=6+37=43��CDMA��ATָ���Ϊ21��GPRS��ATָ���Ϊ15	
      	 send_flg=0x02;
         next_atdata_flg = 0x06;
         Start_timerEx(SEND_PLATFORM_EVT,CMD_WAIT_TIME);		 
			break;					
			  }
			if(send_flg==0x02)
			{
			real_send[20]=0x00;//ƽ̨��ַ���ֽ�
		  real_send[21]=0x25;//ƽ̨��ַ���ֽ�
			alter_hand_auto(0x04,sendnum_mflg);//����ÿ���Կػ�·�ϱ�״̬���ݵĿ�ʼ��ַ,�޸���real_send�����ݵ�ַ(2500H,2504H,2508H,...)
			//mqtt_publish(real_send,75);//MQTT ʵ�ʷ���75���ֽ�,�����75����Ϊ�˳���ɶ���û���á�	
			//send_data_module(mqtt_real_send,"094");
      send_message_len1=6+18+real_send[22]+3;//6+18+ʵ���ֽ���ֵ+3
      bytelen_to_asc((unsigned char *)message_len_char1,send_message_len1);//send_message_lenȫ�ֱ���					
	    send_data_module(real_send,(unsigned char *)message_len_char1);//TCB����Ϊ43=6+��Ϣ����=6+37=43��CDMA��ATָ���Ϊ21��GPRS��ATָ���Ϊ15	
			next_atdata_flg=0x6;
			send_flg=0x03;					
			Start_timerEx(SEND_PLATFORM_EVT,CMD_WAIT_TIME);
			break;
			}
			if(send_flg==0x03)
			{
			module_send_flg=0x6;//1�������ϱ���ɣ��ȴ�5S������1�������ϱ�
			send_flg=0x00;
			send_message_type=0x00;
			if(hand_auto_count[sendnum_mflg]>=1){hand_auto_count[sendnum_mflg]--;}
			sendnum_mflg++;				
			Start_timerEx(SEND_PLATFORM_EVT,INTER_MESS);
			break;
			}
		}
			else
		{
			module_send_flg=0x01;//ȡ����ʱʱ�䣬6�뷢һ�����ݣ������Ҫ��ʱʱ��Ϊ0xFF
      send_flg=0x00;
      send_message_type=0x00;			
			sendnum_mflg=0x00;			
			Start_timerEx(SEND_PLATFORM_EVT,INTER_MESS);
		  break;		 
		 }		
		
  /*�Կ�״̬�����ϱ�����*/
	default:
		Start_timerEx(SEND_PLATFORM_EVT, DATA_WAIT_TIME );//����CDMA��ƽ̨��������
  break;
	}
}

static void cdma_mqtt_send(void) //����mqtt��������
{
 u16 reportCrcValue = 0;    		
	if(send_mess)		//���ڷ��Ͷ����ϱ�ʱ���ȴ�2s������ִ��
	{
		Start_timerEx(SEND_PLATFORM_EVT,2000);
		return;
	}	
	
//	if(send_flg ==0x01||send_flg ==0x04||send_flg ==0x05) //����վ��ʼ���������Wired_Wireles_flag[65]=1��||Wired_Wireles_flag[65]==0
// 	{		
//		Start_timerEx(SEND_PLATFORM_EVT,500);
//		return;
//	}	
	if(send_message_type !=0x03&&send_message_type !=0x00) //����վ��ʼ���������ZZ_Wired_flag[65]=1��||ZZ_Wireles_flag[65]==1
 	{		
		Start_timerEx(SEND_PLATFORM_EVT,500);
		return;
	}	
	send_message_type=0x03;	//���յ�GSM��ERROR����ʱʹ�ã�0x03Ϊ��ƽ̨�ϱ�����
	
	/*send_message_type��¼��cdma������������ͣ�0x01Ϊ��ʼ������0x02Ϊ��ƽ̨���ؿ������0x03Ϊ��ƽ̨�ϱ����ݡ�send_flg��¼
	��cdma���������С���ͣ�0x01Ϊ��ʼ�������atָ���dataָ�0x02Ϊƽ̨�ϱ����ݵ�atָ�0x03Ϊƽ̨�ϱ����ݵ�dataָ��
	0x04Ϊƽ̨���ؿ��Ƶ�atָ�0x05Ϊƽ̨���ؿ��Ƶ�dataָ���Ҫ��ƽ̨��ָ���һ���лظ�,
	�ڽ��ճ�������next_atdata_flg���Ƴ���ִ��˳�򣬲���ƽ̨��ָ��ģ�����module_send_flg���Ƴ���ִ�С�	
	*/

	switch(module_send_flg)//0x0100~0x04FF�ɼ���������;0x500~0x08FF������������;0x0900~0x091F���ؼ�����;0x0920�ϵ�ָʾ
	{
		case 0x01:
			real_send[4] =0x19;//��Ϣ����25,18+ʵ���ֽ���ֵ+3=18+real_send[22]+3=18+4+3=25
			real_send[20]=0x20;//0x0920�ϵ�ָʾƽ̨��ַ���ֽ�;0x0100~0x04FF�ɼ���������
			real_send[21]=0x09;//0x0920�ϵ�ָʾƽ̨��ַ���ֽ�
			
			real_send[22]=0x04;//ʵ�ʷ����ֽ������ֽ�
			real_send[23]=0x00;//ʵ�ʷ����ֽ������ֽ�
			
			real_send[24]=0x00;
			real_send[25]=0x00;
			real_send[26]=0x01;
			real_send[27]=0x00;//���͵��ֽڣ�ÿ������4�ֽڣ�ǰ2���ֽھ�Ϊ0����2���ֽ�Ϊ��Чֵ�����ֽ���ǰ
			reportCrcValue = GetCRC16(real_send+11, 17);//8(�ɿ���ID)+1(����)+2(��ʼ��ַ)+2(�ֽ�������)=13+4(�ֽ�ֵ)=17
			real_send[28] = reportCrcValue&0x00FF;
			real_send[29] = (reportCrcValue&0xFF00)>>8;
			real_send[30] = 0xDD;
      send_message_len1=6+18+real_send[22]+3;//6+18+ʵ���ֽ���ֵ+3
		  mqtt_publish(real_send,send_message_len1);//MQTT ʵ�ʷ���75���ֽ�(��������������)��
      bytelen_to_asc((unsigned char *)message_len_char1,mqtt_len);//mqtt_lenȫ�ֱ�������mqtt_publish�����и�ֵ
     	if(factory_gateway_set[0]==1)send_at_cdma((unsigned char *)message_len_char1,21);//TCB,CDMA	����Ϊ31=6+��Ϣ����=6+25=31��CDMA��ATָ���Ϊ21��GPRS��ATָ���Ϊ15
		  if(factory_gateway_set[0]==2)send_at_gprs((unsigned char *)message_len_char1,15);//TCB,GPRS  TCP ����Ϊ43=6+��Ϣ����=6+37=43��CDMA��ATָ���Ϊ21��GPRS��ATָ���Ϊ15	     
			next_atdata_flg = 0x02;
			send_flg   = 0x02; //��ʾ��ƽ̨�ϱ���atָ������ִ�У�����ֻ��ִ��dataָ��
			Start_timerEx(SEND_PLATFORM_EVT,CMD_WAIT_TIME);		 
			break;
		case 0x02:			
		  send_message_len1=6+18+real_send[22]+3;//6+18+ʵ���ֽ���ֵ+3
		  mqtt_publish(real_send,send_message_len1);//MQTT ʵ�ʷ���75���ֽ�(��������������)��
		  bytelen_to_asc((unsigned char *)message_len_char1,mqtt_len);//send_message_lenȫ�ֱ���		
			send_data_module(ReportData2,(unsigned char *)message_len_char1);      
//		  send_data_module(real_send,(unsigned char *)message_len_char);//TCB	
			next_atdata_flg = 0x03;
			send_flg   = 0x03;//��ʾ��ƽ̨�ϱ���dataָ������ִ��
			Start_timerEx(SEND_PLATFORM_EVT,CMD_WAIT_TIME);		 
			break;
		case 0x03:
			module_send_flg=0x04;
			send_flg=0x00;
		  send_message_type=0x00;
		  real_send[4]=0x25;//��Ϣ����37��18+ʵ���ֽ���ֵ+3=18+real_send[22]+3=18+16+3=37=0x25
		  real_send[20]=0x00;//ƽ̨��ַ���ֽ�
		  real_send[21]=0x21;//ƽ̨��ַ���ֽ�
			real_send[22]=0x10;//ʵ�ʷ����ֽ������ֽ�,2*2*4(���Ƶ�)��1��������Ϊһ���ϱ�
		  real_send[23]=0x00;//ʵ�ʷ����ֽ������ֽ�
		  sendnum_mflg=0x00;
			Start_timerEx(SEND_PLATFORM_EVT,INTER_SEND);
			break;
		
		//����������״̬�����ϱ���ʼ
		case 0x04:  //module_send_flg
	 //����ϵͳ��ʼ����ɺ󣬿�������վ������ڣ�ͬʱ��������վIDС��32����0~31��������ķ�����վ��������;
      while((ZZ_Wireles_flag[32+sendnum_mflg]==0)&&(ZZ_Wired_flag[32+sendnum_mflg]==0)&&(sendnum_mflg<32))//CSH_Wired_finish==0|
       {
				sendnum_mflg++;
			 }
			if(sendnum_mflg<32)
			{           						
    	 if(send_flg==0x00)
				{			  
        send_message_len1=6+18+real_send[22]+3;//6+18+ʵ���ֽ���ֵ+3
				mqtt_publish(real_send,send_message_len1);//MQTT ʵ�ʷ���75���ֽ�(��������������)��
        bytelen_to_asc((unsigned char *)message_len_char1,mqtt_len);//send_message_lenȫ�ֱ���
     		if(factory_gateway_set[0]==1)send_at_cdma((unsigned char *)message_len_char1,21);//TCB,CDMA	����Ϊ31=6+��Ϣ����=6+25=31��CDMA��ATָ���Ϊ21��GPRS��ATָ���Ϊ15
		    if(factory_gateway_set[0]==2)send_at_gprs((unsigned char *)message_len_char1,15);//TCB,GPRS  TCP ����Ϊ43=6+��Ϣ����=6+37=43��CDMA��ATָ���Ϊ21��GPRS��ATָ���Ϊ15				                                                     
      	send_flg=0x02;
        next_atdata_flg = 0x04;
        Start_timerEx(SEND_PLATFORM_EVT,CMD_WAIT_TIME);		 
			break;					
			  }
			if(send_flg==0x02)
			{
			real_send[20]=0x00;//ƽ̨��ַ���ֽ�
		  real_send[21]=0x21;//ƽ̨��ַ���ֽ�
			alter_send(0x10,sendnum_mflg);//����ÿ��������ϱ�״̬���ݵĿ�ʼ��ַ,�޸���real_send�����ݵ�ַ(2100H,2130H,2160H,...)			
      send_message_len1=6+18+real_send[22]+3;//6+18+ʵ���ֽ���ֵ+3      
      mqtt_publish(real_send,send_message_len1);//MQTT ʵ�ʷ���75���ֽ�(��������������)��
			bytelen_to_asc((unsigned char *)message_len_char1,mqtt_len);//send_message_lenȫ�ֱ���		
			send_data_module(ReportData2,(unsigned char *)message_len_char1);				
//	    send_data_module(real_send,(unsigned char *)message_len_char);//TCB����Ϊ43=6+��Ϣ����=6+37=43��CDMA��ATָ���Ϊ21��GPRS��ATָ���Ϊ15	
			next_atdata_flg=0x4;
			send_flg=0x03;					
			Start_timerEx(SEND_PLATFORM_EVT,CMD_WAIT_TIME);
			break;
			}
			if(send_flg==0x03)
			{
			module_send_flg=0x4;//1�������ϱ���ɣ��ȴ�5S������1�������ϱ�
			send_flg=0x00;
			send_message_type=0x00;
			sendnum_mflg++;      		
			Start_timerEx(SEND_PLATFORM_EVT,INTER_SEND);
			break;
			}
		}
			else
		{
			module_send_flg=0x5;
      next_atdata_flg=0x05;			
			send_flg=0x00;
			send_message_type=0x00;
			real_send[4]=0x35;//��Ϣ����45��18+ʵ���ֽ���ֵ+3=18+real_send[22]+3=18+24+3=45=0x2D
			real_send[20]=0x00;//ƽ̨��ַ���ֽ�
		  real_send[21]=0x01;//ƽ̨��ַ���ֽ�
			real_send[22]=0x20;//ʵ�ʷ����ֽ������ֽ�2*2*(6+2)(��������)=32=0x20
			real_send[23]=0x00;//ʵ�ʷ����ֽ������ֽ�
			sendnum_mflg=0x00;		
			Start_timerEx(SEND_PLATFORM_EVT,INTER_MESS);
		  break;
		 }		
		
  /*����������״̬�����ϱ�����*/

	//�ɼ������ݡ����������ݼ����زɼ������ϱ���ʼ
		case 0x05:  //module_send_flg
			//����ϵͳ��ʼ����ɺ󣬲ɼ����Ϳ�������վ������ڣ�ͬʱ��������վIDС�ڵ���64����0~65��������ķ�����վ��������;
			  
     while((ZZ_Wireles_flag[sendnum_mflg]==0)&&(ZZ_Wired_flag[sendnum_mflg]==0)&&(sendnum_mflg<=64))//CSH_Wired_finish==0|
      {
			 sendnum_mflg++;
			}
		if(sendnum_mflg<=64) //���������Ƿ��ѷ�����ɣ�û�з����꣬��������.��sendnum_mflg�Ǽ�¼�ڼ�����վ�ϱ�
		{
      if(send_flg==0x00)
      {	
      send_message_len1=6+18+real_send[22]+3;//6+18+ʵ���ֽ���ֵ+3
			mqtt_publish(real_send,send_message_len1);//MQTT ʵ�ʷ���75���ֽ�(��������������)��
      bytelen_to_asc((unsigned char *)message_len_char1,mqtt_len);//send_message_lenȫ�ֱ���      
		  if(factory_gateway_set[0]==1)send_at_cdma((unsigned char *)message_len_char1,21);//TCB,CDMA	����Ϊ31=6+��Ϣ����=6+25=31��CDMA��ATָ���Ϊ21��GPRS��ATָ���Ϊ15
		  if(factory_gateway_set[0]==2)send_at_gprs((unsigned char *)message_len_char1,15);//TCB,GPRS  TCP ����Ϊ43=6+��Ϣ����=6+37=43��CDMA��ATָ���Ϊ21��GPRS��ATָ���Ϊ15	
			send_flg=0x02;
      next_atdata_flg = 0x05;
      Start_timerEx(SEND_PLATFORM_EVT,CMD_WAIT_TIME);		 
			break;					
			}
			if(send_flg==0x02)
			{
			real_send[20]=0x00;//ƽ̨��ַ���ֽ�
		  real_send[21]=0x01;//ƽ̨��ַ���ֽ�
		  chge_coltsnd(sendnum_mflg);//����ÿ��ɼ����ϱ����ݵĿ�ʼ��ַ,�޸���real_send�����ݵ�ַ(0100H,0130H,0160H,...)
			send_message_len1=6+18+real_send[22]+3;//6+18+ʵ���ֽ���ֵ+3				
      mqtt_publish(real_send,send_message_len1);//MQTT ʵ�ʷ���75���ֽ�(��������������)��
			bytelen_to_asc((unsigned char *)message_len_char1,mqtt_len);//send_message_lenȫ�ֱ���		
			send_data_module(ReportData2,(unsigned char *)message_len_char1);	
//	    send_data_module(real_send,(unsigned char *)message_len_char);//TCB����=6+��Ϣ����=6+2*4+45=51+8��
			next_atdata_flg=0x5;
			send_flg=0x03;					
			Start_timerEx(SEND_PLATFORM_EVT,CMD_WAIT_TIME);
			break;
			}
			if(send_flg==0x03)
			{
			module_send_flg=0x5;//1�������ϱ���ɣ��ȴ�5S������1�������ϱ�
			send_flg=0x00;
			send_message_type=0x00;
			sendnum_mflg++;				
			Start_timerEx(SEND_PLATFORM_EVT,INTER_SEND);
			break;
			}      		
		}
		else
		{
			module_send_flg=0x06;
			send_flg=0x00;
			send_message_type=0x00;
		  real_send[4]=0x19;//��Ϣ����25��18+ʵ���ֽ���ֵ+3=18+real_send[22]+3=18+4+3=25=0x19
		  real_send[20]=0x00;//ƽ̨��ַ���ֽ�
		  real_send[21]=0x25;//ƽ̨��ַ���ֽ�
			real_send[22]=0x04;//ʵ�ʷ����ֽ������ֽ�,2*2*4(���Ƶ�)��1��������Ϊһ���ϱ�
		  real_send[23]=0x00;//ʵ�ʷ����ֽ������ֽ�
		  sendnum_mflg=0x00;
			Start_timerEx(SEND_PLATFORM_EVT,INTER_SEND);
			break;
		 }	  
		//���زɼ������ϱ�����
		//�Կػ�·�ֶ�-�Զ�״̬�ϱ���ʼ
		case 0x06:  //module_send_flg
	 //����ϵͳ��ʼ����ɺ󣬿�������վ������ڣ�ͬʱ��������վIDС��32����0~31��������ķ�����վ��������;
      while((hand_auto_count[sendnum_mflg]==0)&&(sendnum_mflg<73))//CSH_Wired_finish==0|
       {
				sendnum_mflg++;
			 }
			if(sendnum_mflg<73)
			{           						
    	 if(send_flg==0x00)
				{			  
        send_message_len1=6+18+real_send[22]+3;//6+18+ʵ���ֽ���ֵ+3
				mqtt_publish(real_send,send_message_len1);//MQTT ʵ�ʷ���75���ֽ�(��������������)��
        bytelen_to_asc((unsigned char *)message_len_char1,mqtt_len);//send_message_lenȫ�ֱ���
     		if(factory_gateway_set[0]==1)send_at_cdma((unsigned char *)message_len_char1,21);//TCB,CDMA	����Ϊ31=6+��Ϣ����=6+25=31��CDMA��ATָ���Ϊ21��GPRS��ATָ���Ϊ15
		    if(factory_gateway_set[0]==2)send_at_gprs((unsigned char *)message_len_char1,15);//TCB,GPRS  TCP ����Ϊ43=6+��Ϣ����=6+37=43��CDMA��ATָ���Ϊ21��GPRS��ATָ���Ϊ15		                                                     
      	send_flg=0x02;
        next_atdata_flg = 0x06;
        Start_timerEx(SEND_PLATFORM_EVT,CMD_WAIT_TIME);		 
			break;					
			  }
			if(send_flg==0x02)
			{
			real_send[20]=0x00;//ƽ̨��ַ���ֽ�
		  real_send[21]=0x25;//ƽ̨��ַ���ֽ�
			alter_hand_auto(0x04,sendnum_mflg);//����ÿ��������ϱ�״̬���ݵĿ�ʼ��ַ,�޸���real_send�����ݵ�ַ(2100H,2130H,2160H,...)			
      send_message_len1=6+18+real_send[22]+3;//6+18+ʵ���ֽ���ֵ+3      
      mqtt_publish(real_send,send_message_len1);//MQTT ʵ�ʷ���75���ֽ�(��������������)��
			bytelen_to_asc((unsigned char *)message_len_char1,mqtt_len);//send_message_lenȫ�ֱ���		
			send_data_module(ReportData2,(unsigned char *)message_len_char1);				
//	    send_data_module(real_send,(unsigned char *)message_len_char);//TCB����Ϊ43=6+��Ϣ����=6+37=43��CDMA��ATָ���Ϊ21��GPRS��ATָ���Ϊ15	
			next_atdata_flg=0x6;
			send_flg=0x03;					
			Start_timerEx(SEND_PLATFORM_EVT,CMD_WAIT_TIME);
			break;
			}
			if(send_flg==0x03)
			{
			module_send_flg=0x6;//1�������ϱ���ɣ��ȴ�5S������1�������ϱ�
			send_flg=0x00;
			send_message_type=0x00;
			if(hand_auto_count[sendnum_mflg]>=1){hand_auto_count[sendnum_mflg]--;}	
			sendnum_mflg++;				
			Start_timerEx(SEND_PLATFORM_EVT,INTER_MESS);
			break;
			}
		}
			else
		{
			module_send_flg=0x01;//ȡ����ʱʱ�䣬6�뷢һ�����ݣ������Ҫ��ʱʱ��Ϊ0xFF
      send_flg=0x00;			
			sendnum_mflg=0x00;			
			Start_timerEx(SEND_PLATFORM_EVT,INTER_MESS);
		  break;
		 }		
  /*�Կػ�·�ֶ�-�Զ�״̬�ϱ�����*/ 
	default:
		Start_timerEx(SEND_PLATFORM_EVT, DATA_WAIT_TIME );//����CDMA��ƽ̨��������
  break;
	} 
}


static void cdma_sdk_send(void) //����sdk�������ݣ�����ƽ̨��
{
//	u16 reportCrcValue = 0;    		
	if(send_mess)		//���ڷ��Ͷ����ϱ�ʱ���ȴ�2s������ִ��
	{
		Start_timerEx(SEND_PLATFORM_EVT,2000);
		return;
	}	
	
//	if(send_flg ==0x01||send_flg ==0x04||send_flg ==0x05) //����վ��ʼ���������ZZ_Wireles_flag[65]=1��||ZZ_Wired_flag[65]==0
// 	{		
//		Start_timerEx(SEND_PLATFORM_EVT,500);
//		return;
//	}	
  if(send_message_type !=0x03&&send_message_type !=0x00) //����վ��ʼ���������ZZ_Wired_flag[65]=1��||ZZ_Wireles_flag[65]==1
 	{		
		Start_timerEx(SEND_PLATFORM_EVT,500);
		return;
	}	 	
	send_message_type=0x03;	//���յ�GSM��ERROR����ʱʹ�ã�0x03Ϊ��ƽ̨�ϱ�����
	
	/*send_message_type��¼��cdma������������ͣ�0x01Ϊ��ʼ������0x02Ϊ��ƽ̨���ؿ������0x03Ϊ��ƽ̨�ϱ����ݡ�send_flg��¼
	��cdma���������С���ͣ�0x01Ϊ��ʼ�������atָ���dataָ�0x02Ϊƽ̨�ϱ����ݵ�atָ�0x03Ϊƽ̨�ϱ����ݵ�dataָ��
	0x04Ϊƽ̨���ؿ��Ƶ�atָ�0x05Ϊƽ̨���ؿ��Ƶ�dataָ���Ҫ��ƽ̨��ָ���һ���лظ�,
	�ڽ��ճ�������next_atdata_flg���Ƴ���ִ��˳�򣬲���ƽ̨��ָ��ģ�����module_send_flg���Ƴ���ִ�С�	
	*/

	switch(module_send_flg)//0x0100~0x04FF�ɼ���������;0x500~0x08FF������������;0x0900~0x091F���ؼ�����;0x0920�ϵ�ָʾ
	{ 
		u8 len,temp_date[2];
		case 0x01:
      temp_date[0]=0x01;
      temp_date[1]=0x00;		
      len=made_keyX_value(0x0920,temp_date,1,real_send);//0x0920�ϵ�ָʾ��ַ��ֵ0x0001,1һ������������real_send[]={30393230:30303031};len=11���ֽ�
		  if(factory_gateway_set[1]==4)len=made_keyX_value4(0x0920,temp_date,1,real_send);
      mqtt_publish(real_send,len);
      bytelen_to_asc((unsigned char *)message_len_char1,mqtt_len);//mqtt_lenΪȫ�ֱ�������mqtt_publish11�����и�ֵ		
		  if(factory_gateway_set[0]==1)send_at_cdma((unsigned char *)message_len_char1,21);//TCB,CDMA	����Ϊ31=6+��Ϣ����=6+25=31��CDMA��ATָ���Ϊ21��GPRS��ATָ���Ϊ15
		  if(factory_gateway_set[0]==2)send_at_gprs((unsigned char *)message_len_char1,15);//TCB,GPRS  TCP ����Ϊ43=6+��Ϣ����=6+37=43��CDMA��ATָ���Ϊ21��GPRS��ATָ���Ϊ15	
			next_atdata_flg = 0x02;
			send_flg   = 0x02; //��ʾ��ƽ̨�ϱ���atָ������ִ�У�����ֻ��ִ��dataָ��
			Start_timerEx(SEND_PLATFORM_EVT,CMD_WAIT_TIME);		 
			break;
		case 0x02:			
			send_data_module(ReportData2,(unsigned char *)message_len_char1);//ReportData2Ϊʵ�ʷ������ݣ���mqtt_publish�����а�װ
			next_atdata_flg = 0x03;
			send_flg   = 0x03;//��ʾ��ƽ̨�ϱ���dataָ������ִ��
			Start_timerEx(SEND_PLATFORM_EVT,CMD_WAIT_TIME);		 
			break;
		case 0x03:
			module_send_flg=0x04;
			send_flg=0x00;
      send_message_type=0x00;		
		  sendnum_mflg=0x00;
			Start_timerEx(SEND_PLATFORM_EVT,INTER_SEND);
			break;
		
		//����������״̬�����ϱ���ʼ
		case 0x04:  //module_send_flg
	 //����ϵͳ��ʼ����ɺ󣬿�������վ������ڣ�ͬʱ��������վIDС��32����0~31��������ķ�����վ��������;
      while((ZZ_Wireles_flag[32+sendnum_mflg]==0)&&(ZZ_Wired_flag[32+sendnum_mflg]==0)&&(sendnum_mflg<32))//CSH_Wired_finish==0|
       {
				sendnum_mflg++;
			 }
			if(sendnum_mflg<32)
			{           						
    	 if(send_flg==0x00)
				{
				len=made_keyX_value(0x2100+sendnum_mflg*4*4,Controllers[sendnum_mflg],4,real_send);//0x2100����������״̬��ַ��һ���ϱ�1��������4��״̬����
        if(factory_gateway_set[1]==4)len=made_keyX_value4(0x2100+sendnum_mflg*4*4,Controllers[sendnum_mflg],4,real_send);					
				mqtt_publish(real_send,len);//�����ͺ���ΪSDK�ر�ָ���ģ���Ϊ�涨�˿�����״̬�ϱ�����������
        bytelen_to_asc((unsigned char *)message_len_char1,mqtt_len);//mqtt_lenΪȫ�ֱ�������mqtt_publish11�����и�ֵ		
			  if(factory_gateway_set[0]==1)send_at_cdma((unsigned char *)message_len_char1,21);//TCB,CDMA	����Ϊ31=6+��Ϣ����=6+25=31��CDMA��ATָ���Ϊ21��GPRS��ATָ���Ϊ15
		    if(factory_gateway_set[0]==2)send_at_gprs((unsigned char *)message_len_char1,15);//TCB,GPRS  TCP ����Ϊ43=6+��Ϣ����=6+37=43��CDMA��ATָ���Ϊ21��GPRS��ATָ���Ϊ15	
      	send_flg=0x02;
        next_atdata_flg = 0x04;
        Start_timerEx(SEND_PLATFORM_EVT,CMD_WAIT_TIME);		 
			break;					
			  }
			if(send_flg==0x02)
			{
			len=made_keyX_value(0x2100+sendnum_mflg*4*4,Controllers[sendnum_mflg],4,real_send);//0x2100����������״̬��ַ���ϱ�1��������4��״̬����
      if(factory_gateway_set[1]==4)len=made_keyX_value4(0x2100+sendnum_mflg*4*4,Controllers[sendnum_mflg],4,real_send);				
			mqtt_publish(real_send,len);//�����ͺ���ΪSDK�ر�ָ���ģ���Ϊ�涨�˿�����״̬�ϱ�����������
      bytelen_to_asc((unsigned char *)message_len_char1,mqtt_len);//mqtt_lenΪȫ�ֱ�������mqtt_publish11�����и�ֵ		
			send_data_module(ReportData2,(unsigned char *)message_len_char1);//ReportData2Ϊʵ�ʷ������ݣ���mqtt_publish�����а�װ
			next_atdata_flg=0x4;
			send_flg=0x03;					
			Start_timerEx(SEND_PLATFORM_EVT,CMD_WAIT_TIME);
			break;
			}
			if(send_flg==0x03)
			{
			module_send_flg=0x4;//1�������ϱ���ɣ��ȴ�5S������1�������ϱ�
			send_flg=0x00;
			send_message_type=0x00;
			sendnum_mflg++;				
			Start_timerEx(SEND_PLATFORM_EVT,INTER_SEND);
			break;
			}
		}
			else
		{
			module_send_flg=0x5;
      next_atdata_flg=0x05;			
			send_flg=0x00;
			send_message_type=0x00;
			sendnum_mflg=0x00;		
			Start_timerEx(SEND_PLATFORM_EVT,INTER_MESS);
		  break;
		 }		
		
  /*����������״̬�����ϱ�����*/

	//�ɼ������ݡ����������ݼ����زɼ������ϱ���ʼ
		case 0x05:  //module_send_flg
			//����ϵͳ��ʼ����ɺ󣬲ɼ����Ϳ�������վ������ڣ�ͬʱ��������վIDС�ڵ���64����0~65��������ķ�����վ��������;
			  
     while((ZZ_Wireles_flag[sendnum_mflg]==0)&&(ZZ_Wired_flag[sendnum_mflg]==0)&&(sendnum_mflg<=64))//CSH_Wired_finish==0|
      {
			 sendnum_mflg++;
			}
		if(sendnum_mflg<=64) //���������Ƿ��ѷ�����ɣ�û�з����꣬��������.��sendnum_mflg�Ǽ�¼�ڼ�����վ�ϱ�
		{
      if(send_flg==0x00)
      {	
			 memset(real_send,0,sizeof(real_send));
       len=made_keyX_valueF(0x0100+sendnum_mflg*4*8,Collectors[sendnum_mflg],zero_rang.k_b_float[sendnum_mflg],4*send_count,real_send);
				//0x0100Ϊ�ɼ����Ϳ������ɼ����ݵ�ַ���ϱ�1��������8���ɼ���������2���ϱ���4*send_countΪ��2���ϱ��Ŀ�ʼ������ַ
			 if(factory_gateway_set[1]==4)len=made_keyX_valueF4(0x0100+sendnum_mflg*4*8,Collectors[sendnum_mflg],zero_rang.k_b_float[sendnum_mflg],4*send_count,real_send);
			 mqtt_publish(real_send,len);//�����ͺ���Ϊͨ�����ͺ���
       bytelen_to_asc((unsigned char *)message_len_char1,mqtt_len);//mqtt_lenΪȫ�ֱ�������mqtt_publish11�����и�ֵ
       send_flg=0x02;				
			 if(factory_gateway_set[0]==1)send_at_cdma((unsigned char *)message_len_char1,21);//TCB,CDMA	����Ϊ31=6+��Ϣ����=6+25=31��CDMA��ATָ���Ϊ21��GPRS��ATָ���Ϊ15
		   if(factory_gateway_set[0]==2)send_at_gprs((unsigned char *)message_len_char1,15);//TCB,GPRS  TCP ����Ϊ43=6+��Ϣ����=6+37=43��CDMA��ATָ���Ϊ21��GPRS��ATָ���Ϊ15				  
       next_atdata_flg = 0x05;
       Start_timerEx(SEND_PLATFORM_EVT,CMD_WAIT_TIME);		 
			 break;					
			}
			if(send_flg==0x02)
			{
			 len=made_keyX_valueF(0x0100+sendnum_mflg*4*8,Collectors[sendnum_mflg],zero_rang.k_b_float[sendnum_mflg],4*send_count,real_send);
				//0x0100Ϊ�ɼ����Ϳ������ɼ����ݵ�ַ���ϱ�1��������8���ɼ�����
       if(factory_gateway_set[1]==4)len=made_keyX_valueF4(0x0100+sendnum_mflg*4*8,Collectors[sendnum_mflg],zero_rang.k_b_float[sendnum_mflg],4*send_count,real_send);				
			 mqtt_publish(real_send,len);//�����ͺ���Ϊͨ�����ͺ���
       bytelen_to_asc((unsigned char *)message_len_char1,mqtt_len);//mqtt_lenΪȫ�ֱ�������mqtt_publish11�����и�ֵ		
		   send_data_module(ReportData2,(unsigned char *)message_len_char1);//ReportData2Ϊʵ�ʷ������ݣ���mqtt_publish�����а�װ
			if(send_count==0)
			{
				send_count=1;
				module_send_flg=0x5;
				next_atdata_flg=0x5;
			  send_flg=0x00;
				send_message_type=0x00;
				Start_timerEx(SEND_PLATFORM_EVT,CMD_WAIT_TIME);//8��������2�������ϱ���300ms
				break;
			}
			else
			send_count=0;
			module_send_flg=0x5;
			next_atdata_flg=0x5;
			send_flg=0x03;					
			Start_timerEx(SEND_PLATFORM_EVT,CMD_WAIT_TIME);
			break;
			}
			
			if(send_flg==0x03)
			{
			module_send_flg=0x5;//1�������ϱ���ɣ��ȴ�5S������1�������ϱ�
			send_flg=0x00;
			send_message_type=0x00;
			send_count=0;
			sendnum_mflg++;				
			Start_timerEx(SEND_PLATFORM_EVT,INTER_SEND);
			break;
			}      		
		}
		else
		{
			module_send_flg=0x06;
			send_flg=0x00;
      send_message_type=0x00;		  
		  sendnum_mflg=0x00;
			Start_timerEx(SEND_PLATFORM_EVT,INTER_SEND);
			break;
		 }	  
		//���زɼ������ϱ�����
		//�Կػ�·�ֶ�-�Զ�״̬�����ϱ���ʼ
		case 0x06:  //module_send_flg
	 //����ϵͳ��ʼ����ɺ󣬿�������վ������ڣ�ͬʱ��������վIDС��32����0~31��������ķ�����վ��������;
      while((hand_auto_count[sendnum_mflg]==0)&&(sendnum_mflg<73))//CSH_Wired_finish==0|
       {
				sendnum_mflg++;
			 }
			if(sendnum_mflg<73)
			{           						
    	 if(send_flg==0x00)
				{
				len=made_keyX_value(0x2500+sendnum_mflg*1*4,hand_auto_flg[sendnum_mflg],1,real_send);//0x2500�Կػ�·���Զ�״̬��ַ��1���ϱ�1���ֶ�-�Զ�״̬
        if(factory_gateway_set[1]==4)len=made_keyX_value4(0x2500+sendnum_mflg*1*4,hand_auto_flg[sendnum_mflg],1,real_send);					
				mqtt_publish(real_send,len);//�����ͺ���ΪSDK�ر�ָ���ģ���Ϊ�涨�˿�����״̬�ϱ�����������
        bytelen_to_asc((unsigned char *)message_len_char1,mqtt_len);//mqtt_lenΪȫ�ֱ�������mqtt_publish11�����и�ֵ		
			  if(factory_gateway_set[0]==1)send_at_cdma((unsigned char *)message_len_char1,21);//TCB,CDMA	����Ϊ31=6+��Ϣ����=6+25=31��CDMA��ATָ���Ϊ21��GPRS��ATָ���Ϊ15
		    if(factory_gateway_set[0]==2)send_at_gprs((unsigned char *)message_len_char1,15);//TCB,GPRS  TCP ����Ϊ43=6+��Ϣ����=6+37=43��CDMA��ATָ���Ϊ21��GPRS��ATָ���Ϊ15	
      	send_flg=0x02;
        next_atdata_flg = 0x04;
        Start_timerEx(SEND_PLATFORM_EVT,CMD_WAIT_TIME);		 
			break;					
			  }
			if(send_flg==0x02)
			{
			len=made_keyX_value(0x2500+sendnum_mflg*1*4,hand_auto_flg[sendnum_mflg],1,real_send);//0x2500�Կػ�·���Զ�״̬��ַ��1���ϱ�1���ֶ�-�Զ�״̬ 
			if(factory_gateway_set[1]==4)len=made_keyX_value4(0x2500+sendnum_mflg*1*4,hand_auto_flg[sendnum_mflg],1,real_send);	
			mqtt_publish(real_send,len);//�����ͺ���ΪSDK�ر�ָ���ģ���Ϊ�涨�˿�����״̬�ϱ�����������
      bytelen_to_asc((unsigned char *)message_len_char1,mqtt_len);//mqtt_lenΪȫ�ֱ�������mqtt_publish11�����и�ֵ		
			send_data_module(ReportData2,(unsigned char *)message_len_char1);//ReportData2Ϊʵ�ʷ������ݣ���mqtt_publish�����а�װ
			next_atdata_flg=0x4;
			send_flg=0x03;					
			Start_timerEx(SEND_PLATFORM_EVT,CMD_WAIT_TIME);
			break;
			}
			if(send_flg==0x03)
			{
			module_send_flg=0x4;//1�������ϱ���ɣ��ȴ�5S������1�������ϱ�
			send_flg=0x00;
			send_message_type=0x00;	
			sendnum_mflg++;				
			Start_timerEx(SEND_PLATFORM_EVT,INTER_SEND);
			break;
			}
		}
			else
		{
			module_send_flg=0x01;//ȡ����ʱʱ�䣬6�뷢һ�����ݣ������Ҫ��ʱʱ��Ϊ0xFF
      send_flg=0x00;
      send_message_type=0x00;				
			sendnum_mflg=0x00;			
			Start_timerEx(SEND_PLATFORM_EVT,INTER_MESS);
		  break;
		 }		
		
  /*����������״̬�����ϱ�����*/ 		 
	default:
		Start_timerEx(SEND_PLATFORM_EVT, DATA_WAIT_TIME );//����CDMA��ƽ̨��������
  break;
	}
}

//static void cdma_other_send(void) //����other��������
//{
//}

static void gprs_other_send(void) //�ƶ�other��������
{
	Start_timerEx(SEND_PLATFORM_EVT, DATA_WAIT_TIME );//����CDMA��ƽ̨��������
}

static void Ethernet_tcp_send(void) //����TCP/IP��������
{
u16 reportCrcValue = 0;    		
	if(send_mess)		//���ڷ��Ͷ����ϱ�ʱ���ȴ�2s������ִ��
	{
		Start_timerEx(SEND_PLATFORM_EVT,2000);
		return;
	}	
	
//	if(send_flg ==0x01||send_flg ==0x04||send_flg ==0x05) //����վ��ʼ���������ZZ_Wired_flag[65]=1��||ZZ_Wireles_flag[65]==1
// 	{		
//		Start_timerEx(SEND_PLATFORM_EVT,500);
//		return;
//	}	
	if(send_message_type !=0x03&&send_message_type !=0x00) //����վ��ʼ���������ZZ_Wired_flag[65]=1��||ZZ_Wireles_flag[65]==1
 	{		
		Start_timerEx(SEND_PLATFORM_EVT,500);
		return;
	}	 	
	send_message_type=0x03;	//���յ�GSM��ERROR����ʱʹ�ã�0x03Ϊ��ƽ̨�ϱ�����
	
	/*send_message_type��¼��cdma������������ͣ�0x01Ϊ��ʼ������0x02Ϊ��ƽ̨���ؿ������0x03Ϊ��ƽ̨�ϱ����ݣ�0x04����Ϊ���ٻظ�����ƽ̨�Ŀ���ָ�send_flg��¼
	��cdma���������С���ͣ�0x01Ϊ��ʼ�������atָ���dataָ�0x02Ϊƽ̨�ϱ����ݵ�atָ�0x03Ϊƽ̨�ϱ����ݵ�dataָ��
	0x04Ϊƽ̨���ؿ��Ƶ�atָ�0x05Ϊƽ̨���ؿ��Ƶ�dataָ���Ҫ��ƽ̨��ָ���һ���лظ�,
	�ڽ��ճ�������next_atdata_flg���Ƴ���ִ��˳�򣬲���ƽ̨��ָ��ģ�����module_send_flg���Ƴ���ִ�С�	
	*/

	switch(module_send_flg)//0x0100~0x04FF�ɼ���������;0x500~0x08FF������������;0x0900~0x091F���ؼ�����;0x0920�ϵ�ָʾ
	{
		case 0x01:      
			module_send_flg = 0x02;
			send_flg   = 0x02; //��ʾ��ƽ̨�ϱ���atָ������ִ�У�����ֻ��ִ��dataָ��
			Start_timerEx(SEND_PLATFORM_EVT,Ethernet_WAIT_TIME);		 
			break;
		case 0x02:
			real_send[4] =0x19;//��Ϣ����25,18+ʵ���ֽ���ֵ+3=18+real_send[22]+3=18+4+3=25
			real_send[20]=0x20;//0x0920�ϵ�ָʾƽ̨��ַ���ֽ�;0x0100~0x04FF�ɼ���������
			real_send[21]=0x09;//0x0920�ϵ�ָʾƽ̨��ַ���ֽ�
			
			real_send[22]=0x04;//ʵ�ʷ����ֽ������ֽ�
			real_send[23]=0x00;//ʵ�ʷ����ֽ������ֽ�
			
			real_send[24]=0x00;
			real_send[25]=0x00;
			real_send[26]=0x01;
			real_send[27]=0x00;//���͵��ֽڣ�ÿ������4�ֽڣ�ǰ2���ֽھ�Ϊ0����2���ֽ�Ϊ��Чֵ�����ֽ���ǰ

			reportCrcValue = GetCRC16(real_send+11, 17);//8(�ɿ���ID)+1(����)+2(��ʼ��ַ)+2(�ֽ�������)=13+4(�ֽ�ֵ)=17

			real_send[28] = reportCrcValue&0x00FF;
			real_send[29] = (reportCrcValue&0xFF00)>>8;
			real_send[30] = 0xDD;
		  send_message_len1=6+18+real_send[22]+3;//6+18+ʵ���ֽ���ֵ+3
      bytelen_to_asc((unsigned char *)message_len_char1,send_message_len1);//send_message_lenȫ�ֱ���	
		  send_Ethernet_module(real_send,(unsigned char *)message_len_char1);//TCB	
			module_send_flg = 0x03;
			send_flg   = 0x03;//��ʾ��ƽ̨�ϱ���dataָ������ִ��
			Start_timerEx(SEND_PLATFORM_EVT,Ethernet_WAIT_TIME);		 
			break;
		case 0x03:
			module_send_flg=0x04;
			send_flg=0x00;
		  send_message_type=0x00;
		  real_send[4]=0x25;//��Ϣ����37��18+ʵ���ֽ���ֵ+3=18+real_send[22]+3=18+16+3=37=0x25
		  real_send[20]=0x00;//ƽ̨��ַ���ֽ�
		  real_send[21]=0x21;//ƽ̨��ַ���ֽ�
			real_send[22]=0x10;//ʵ�ʷ����ֽ������ֽ�,2*2*4(���Ƶ�)��1��������Ϊһ���ϱ�
		  real_send[23]=0x00;//ʵ�ʷ����ֽ������ֽ�
		  sendnum_mflg=0x00;
			Start_timerEx(SEND_PLATFORM_EVT,INTER_SEND);
			break;
		
		//����������״̬�ϱ���ʼ
		case 0x04:  //module_send_flg
	 //����ϵͳ��ʼ����ɺ󣬿�������վ������ڣ�ͬʱ��������վIDС��32����0~31��������ķ�����վ��������;
      while((ZZ_Wireles_flag[32+sendnum_mflg]==0)&&(ZZ_Wired_flag[32+sendnum_mflg]==0)&&(sendnum_mflg<32))//CSH_Wired_finish==0|
       {
				sendnum_mflg++;
			 }
			if(sendnum_mflg<32)
			{           						
    	 if(send_flg==0x00)
				{			   
      	 send_flg=0x02;
         module_send_flg = 0x04;
         Start_timerEx(SEND_PLATFORM_EVT,Ethernet_WAIT_TIME);		 
			break;					
			  }
			if(send_flg==0x02)
			{
			real_send[20]=0x00;//ƽ̨��ַ���ֽ�
		  real_send[21]=0x21;//ƽ̨��ַ���ֽ�
			alter_send(0x10,sendnum_mflg);//����ÿ��������ϱ�״̬���ݵĿ�ʼ��ַ,�޸���real_send�����ݵ�ַ(2100H,2130H,2160H,...)
			//mqtt_publish(real_send,75);//MQTT ʵ�ʷ���75���ֽ�,�����75����Ϊ�˳���ɶ���û���á�	
			//send_data_module(mqtt_real_send,"094");
      send_message_len1=6+18+real_send[22]+3;//6+18+ʵ���ֽ���ֵ+3
      bytelen_to_asc((unsigned char *)message_len_char1,send_message_len1);//send_message_lenȫ�ֱ���					
	    send_Ethernet_module(real_send,(unsigned char *)message_len_char1);//TCB����Ϊ43=6+��Ϣ����=6+37=43��CDMA��ATָ���Ϊ21��GPRS��ATָ���Ϊ15	
			module_send_flg=0x4;
			send_flg=0x03;					
			Start_timerEx(SEND_PLATFORM_EVT,Ethernet_WAIT_TIME);
			break;
			}
			if(send_flg==0x03)
			{
			module_send_flg=0x4;//1�������ϱ���ɣ��ȴ�5S������1�������ϱ�
			send_flg=0x00;
			send_message_type=0x00;
			sendnum_mflg++;				
			Start_timerEx(SEND_PLATFORM_EVT,INTER_SEND);
			break;
			}
		}
			else
		{
			module_send_flg=0x5;     	
			send_flg=0x00;
			send_message_type=0x00;
			real_send[4]=0x35;//��Ϣ����45��18+ʵ���ֽ���ֵ+3=18+real_send[22]+3=18+24+3=45=0x2D
			real_send[20]=0x00;//ƽ̨��ַ���ֽ�
		  real_send[21]=0x01;//ƽ̨��ַ���ֽ�
			real_send[22]=0x20;//ʵ�ʷ����ֽ������ֽ�2*2*(6+2)(��������)=32=0x20
			real_send[23]=0x00;//ʵ�ʷ����ֽ������ֽ�
			sendnum_mflg=0x00;		
			Start_timerEx(SEND_PLATFORM_EVT,INTER_MESS);
		  break;
		 }		
		
  /*����������״̬�����ϱ�����*/

	//�ɼ������ݡ����������ݼ����زɼ������ϱ���ʼ
		case 0x05:  //module_send_flg
			//����ϵͳ��ʼ����ɺ󣬲ɼ����Ϳ�������վ������ڣ�ͬʱ��������վIDС�ڵ���64����0~65��������ķ�����վ��������;
			  
     while((ZZ_Wireles_flag[sendnum_mflg]==0)&&(ZZ_Wired_flag[sendnum_mflg]==0)&&(sendnum_mflg<=64))//CSH_Wired_finish==0|
      {
			 sendnum_mflg++;
			}
		if(sendnum_mflg<=64) //���������Ƿ��ѷ�����ɣ�û�з����꣬��������.��sendnum_mflg�Ǽ�¼�ڼ�����վ�ϱ�
		{
      if(send_flg==0x00)
      {	       
				send_flg=0x02;
				module_send_flg = 0x05;
				Start_timerEx(SEND_PLATFORM_EVT,Ethernet_WAIT_TIME);		 
				break;					
			}
			if(send_flg==0x02)
			{
				real_send[20]=0x00;//ƽ̨��ַ���ֽ�
				real_send[21]=0x01;//ƽ̨��ַ���ֽ�	
				chge_coltsnd(sendnum_mflg);//����ÿ��ɼ����ϱ����ݵĿ�ʼ��ַ,�޸���real_send�����ݵ�ַ(0100H,0130H,0160H,...)
				send_message_len1=6+18+real_send[22]+3;//6+18+ʵ���ֽ���ֵ+3
				bytelen_to_asc((unsigned char *)message_len_char1,send_message_len1);//send_message_lenȫ�ֱ���	 		
				//mqtt_publish(real_send,75);//MQTT ʵ�ʷ���75���ֽ�,�����75����Ϊ�˳���ɶ���û����.		
				//send_data_module(mqtt_real_send,"094");	 
				send_Ethernet_module(real_send,(unsigned char *)message_len_char1);//TCB����=6+��Ϣ����=6+2*4+45=51+8��
				module_send_flg=0x5;
				send_flg=0x03;					
				Start_timerEx(SEND_PLATFORM_EVT,Ethernet_WAIT_TIME);
				break;
			}
			if(send_flg==0x03)
			{
				module_send_flg=0x5;//1�������ϱ���ɣ��ȴ�5S������1�������ϱ�
				send_flg=0x00;
				send_message_type=0x00;
				sendnum_mflg++;				
				Start_timerEx(SEND_PLATFORM_EVT,INTER_SEND);
				break;
			}      		
		}
		else
		{
			module_send_flg=0x06;
			send_flg=0x00;
			send_message_type=0x00;
		  real_send[4]=0x19;//��Ϣ����25��18+ʵ���ֽ���ֵ+3=18+real_send[22]+3=18+4+3=25=0x19
		  real_send[20]=0x00;//ƽ̨��ַ���ֽ�
		  real_send[21]=0x25;//ƽ̨��ַ���ֽ�
			real_send[22]=0x04;//ʵ�ʷ����ֽ������ֽ�,2*2*1(�Կػ�·)��1���Կػ�·Ϊһ���ϱ�
		  real_send[23]=0x00;//ʵ�ʷ����ֽ������ֽ�
		  sendnum_mflg=0x00;
			Start_timerEx(SEND_PLATFORM_EVT,INTER_SEND);
			break;
		 }	  
		//���زɼ������ϱ�����
		 
		 //�Կ�״̬�ϱ���ʼ
		case 0x06:  //module_send_flg
	 //����ϵͳ��ʼ����ɺ󣬿�������վ������ڣ�ͬʱ��������վIDС��32����0~31��������ķ�����վ��������;
      while((hand_auto_count[sendnum_mflg]==0)&&(sendnum_mflg<73))//CSH_Wired_finish==0|
       {
				sendnum_mflg++;
			 }			 
			if(sendnum_mflg<73)
			{           						
    	 if(send_flg==0x00)
				{			   
					 send_flg=0x02;
					 module_send_flg = 0x06;
					 Start_timerEx(SEND_PLATFORM_EVT,Ethernet_WAIT_TIME);		 
			     break;					
			  }
			if(send_flg==0x02)
			{
				real_send[20]=0x00;//ƽ̨��ַ���ֽ�
				real_send[21]=0x25;//ƽ̨��ַ���ֽ�
				alter_hand_auto(0x04,sendnum_mflg);//����ÿ���Կػ�·�ϱ�״̬���ݵĿ�ʼ��ַ,�޸���real_send�����ݵ�ַ(2500H,2504H,2508H,...)
				//mqtt_publish(real_send,75);//MQTT ʵ�ʷ���75���ֽ�,�����75����Ϊ�˳���ɶ���û���á�	
				//send_data_module(mqtt_real_send,"094");
				send_message_len1=6+18+real_send[22]+3;//6+18+ʵ���ֽ���ֵ+3
				bytelen_to_asc((unsigned char *)message_len_char1,send_message_len1);//send_message_lenȫ�ֱ���					
				send_Ethernet_module(real_send,(unsigned char *)message_len_char1);//TCB����Ϊ43=6+��Ϣ����=6+37=43��CDMA��ATָ���Ϊ21��GPRS��ATָ���Ϊ15	
				module_send_flg=0x6;
				send_flg=0x03;					
				Start_timerEx(SEND_PLATFORM_EVT,Ethernet_WAIT_TIME);
				break;
			}
			if(send_flg==0x03)
			{
				module_send_flg=0x6;//1�������ϱ���ɣ��ȴ�5S������1�������ϱ�
				send_flg=0x00;
				send_message_type=0x00;
				if(hand_auto_count[sendnum_mflg]>=1){hand_auto_count[sendnum_mflg]--;}
				sendnum_mflg++;				
				Start_timerEx(SEND_PLATFORM_EVT,INTER_MESS);
				break;
			}
		}
			else
		{
			module_send_flg=0x01;//ȡ����ʱʱ�䣬6�뷢һ�����ݣ������Ҫ��ʱʱ��Ϊ0xFF
      send_flg=0x00;
      send_message_type=0x00;			
			sendnum_mflg=0x00;			
			Start_timerEx(SEND_PLATFORM_EVT,INTER_MESS);
		  break;		 
		 }		
		
  /*�Կ�״̬�����ϱ�����*/
	default:
		Start_timerEx(SEND_PLATFORM_EVT, DATA_WAIT_TIME );//����CDMA��ƽ̨��������
  break;
	}
}
static void Ethernet_mqtt_send(void) //����mqtt��������			
{
	Start_timerEx(SEND_PLATFORM_EVT, DATA_WAIT_TIME );//����CDMA��ƽ̨��������
}
static void Ethernet_sdk_send(void)  //����sdk��������	
{
	Start_timerEx(SEND_PLATFORM_EVT, DATA_WAIT_TIME );//����CDMA��ƽ̨��������
}
static void Ethernet_other_send(void) //����other��������
{
	Start_timerEx(SEND_PLATFORM_EVT, DATA_WAIT_TIME );//����CDMA��ƽ̨��������
}

/*Ethernet��WiFiƽ̨���պ����ࣨ����1��-----------------------------------------------------------------------------------------------*/
static void RxReport1(u8 len,u8 *pData) //��ƽ̨��CDMA��GPRS���������ݴ���
{
  switch(factory_gateway_set[0]) //��������=3 ��̫����4 WiFi
	{
	 //��̫����ʼ---------------------------------
  case 0x03:	//��̫��	
		switch(factory_gateway_set[1]) //Э������=1 TCP/IP;2 MQTT; 3 SDK; 4 ������
		{			
			case 0x01:   //��̫��TCP/IP				
				Ethernet_tcp_receive(len,pData);			  
			break;
			
			case 0x02:  //��̫��MQTT				
				Ethernet_mqtt_receive(len,pData);			  
			break;
			
			case 0x03:   //��̫��SDK
				Ethernet_sdk_receive(len,pData);			  
			break;
			
			case 0x04:  //��̫��other
				Ethernet_other_receive(len,pData);
			break;
			default:
      			break;
		}
				
	break;
//��̫�� ����------------------------------------
		
//WiFi	��ʼ====================================
	case 0x04:
		switch(factory_gateway_set[1]) //Э������=1 TCP/IP;2 MQTT; 3 SDK; 4 ������
		{
			//WiFi TCP/IP
			case 0x01:
//				gprs_tcp_receive(len,pData);			
			break;
			//WiFi MQTT
			case 0x02:
//				gprs_mqtt_receive(len,pData);
			break;
			//WiFi SDK
			case 0x03:
//				gprs_sdk_receive(len,pData);
			break;
			//WiFi other
			case 0x04:
//				gprs_other_receive(len,pData);
			break;
			default:
      			break;
		}
	 break;
//WiFi	����====================================
	default:
      			break;	
	}
}

/*CDMA��GPRSƽ̨���պ����ࣨ����2��-----------------------------------------------------------------------------------------------*/
static void RxReport2(u8 len,u8 *pData) //��ƽ̨��CDMA��GPRS���������ݴ���
{
  switch(factory_gateway_set[0]) //��������=1 ���ţ�2 �ƶ�
	{
	 //���� cdma ��ʼ---------------------------------
  case 0x01:	//���� cdma	
		switch(factory_gateway_set[1]) //Э������=1 TCP/IP;2 MQTT; 3 SDK; 4 ������
		{			
			case 0x01:   //����TCP/IP
				cdma_cmd_receive(len,pData);//RxReport2_step_flg��Ϊ�˼���CPU���и���
				if(RxReport2_step_flg==0){cdma_tcp_receive(len,pData);}//RxReport2_step_flg=1��ʾ��·���������cdma_cmd_receive
			  if(RxReport2_step_flg==0){cdma_SMS_receive(len,pData);}//RxReport2_step_flg=2��ʾЭ�麯�������cdma_tcp_receive��cdma_sdk_receive��				
			break;
			
			case 0x02:  //����MQTT
				cdma_cmd_receive(len,pData);
				if(RxReport2_step_flg==0){cdma_mqtt_receive(len,pData);}
			  if(RxReport2_step_flg==0){cdma_SMS_receive(len,pData);}			
			break;
			
			case 0x03:   //����SDK
				cdma_cmd_receive(len,pData);
				if(RxReport2_step_flg==0){cdma_sdk_receive(len,pData);}
			  if(RxReport2_step_flg==0){cdma_SMS_receive(len,pData);}				
			break;
			
			case 0x04:  //����other
				cdma_cmd_receive(len,pData);
				if(RxReport2_step_flg==0){cdma_sdk_receive(len,pData);}
			  if(RxReport2_step_flg==0){cdma_SMS_receive(len,pData);}		
			break;
			default:
      			break;
		}
				
	break;
//���� cdma ����------------------------------------
		
//�ƶ� gprs	��ʼ====================================
	case 0x02:
		switch(factory_gateway_set[1]) //Э������=1 TCP/IP;2 MQTT; 3 SDK; 4 ������
		{
			//�ƶ�TCP/IP
			case 0x01:
				gprs_cmd_receive(len,pData);//RxReport2_step_flg��Ϊ�˼���CPU���и���
				if(RxReport2_step_flg==0){cdma_tcp_receive(len,pData);}//RxReport2_step_flg=1��ʾ��·���������cdma_cmd_receive�������� �ƶ�TCP/IP����ƽ̨���� gprs_tcp_receive
			  if(RxReport2_step_flg==0){cdma_SMS_receive(len,pData);}//RxReport2_step_flg=2��ʾЭ�麯�������cdma_tcp_receive��cdma_sdk_receive��	�������ƶ����ն��Ŵ�����gprs_SMS_receive				
			break;
			//�ƶ�MQTT
			case 0x02:
				gprs_cmd_receive(len,pData);//RxReport2_step_flg��Ϊ�˼���CPU���и���
			  if(RxReport2_step_flg==0){cdma_mqtt_receive(len,pData);}//������ �ƶ�mqtt����ƽ̨���� gprs_mqtt_receive
			  if(RxReport2_step_flg==0){cdma_SMS_receive(len,pData);} //�������ƶ����ն��Ŵ�����gprs_SMS_receive				
			break;
			//�ƶ�SDK
			case 0x03:
				gprs_cmd_receive(len,pData);//RxReport2_step_flg��Ϊ�˼���CPU���и���
			  if(RxReport2_step_flg==0){cdma_sdk_receive(len,pData);}//������ �ƶ�sdk����ƽ̨���� gprs_sdk_receive
			  if(RxReport2_step_flg==0){cdma_SMS_receive(len,pData);}//�������ƶ����ն��Ŵ�����gprs_SMS_receive				
			break;
			//�ƶ�other
			case 0x04:
				gprs_other_receive(len,pData);
			break;
			default:
      			break;
		}
	 break;
//�ƶ� gprs	����====================================
	default:
      			break;	
	}
}
/*CDMA��GPRS���պ���-----------------------------------------------------------------------------------*/
static void cdma_cmd_receive(u8 len,u8 *pBuf)//����TCP/IP����CDMA����ش�����
{ 
//   memcpy(test,pBuf,100);	
  if((mqtt_bcmd=strstr((char *)pBuf,"+CSQ:"))!=NULL)//˵��CDMA��������,wakeup_flg==0x01��ʾ�����Ѱ��ڼ�	
	{
//		gsm_halt_test=0x00;
//		 Start_timerEx(GSM_AT_READBUF,10000);//10S���һ�Σ�����û�������ظ�����ΪCDMA�����⣬��cycle_cmd()�ж�	
    RxReport2_step_flg=1;		
		return;
	}
	

	if(strstr((char *)pBuf,"ERROR")!= NULL)//����ϵͳ�������������Ժ�ָ�
  { 
   if(send_message_type==0x01||send_message_type==0x02||send_message_type==0x03||send_message_type==0x04) //0x02Ϊ�ظ�ƽ̨����ָ�0x03Ϊƽ̨�ϱ�����
	 {		
			if(error_num<MAX_ERROR_NUM) //����ATָ������ظ�ִ�и�ָ�������MAX_ERROR_NUM����
			 {
				if(send_message_type==0x01&&send_flg==0x01)
				{			 
				 Start_timerEx(NET_INIT_EVT,1000);	//����ִ�и�ָ�reprot_flgû�иı�		
				}
				if(send_message_type==0x02&&send_flg==0x04)
				{
				 send_flg=0x00;
         send_message_type=0x00;					
				 Start_timerEx(WG_REPLY_EVT,100);			
				}
				if(send_message_type==0x02&&send_flg==0x05)
				{
				 send_flg=0x04;			
				 Start_timerEx(WG_REPLY_EVT,100);			
				}
				
				if(send_message_type==0x03&&send_flg==0x02)
				{		
				send_flg=0x00;
        send_message_type=0x00;					
				Start_timerEx(SEND_PLATFORM_EVT,100);
				}
				if(send_message_type==0x03&&send_flg==0x03)
				{
				send_flg=0x02;	    
				Start_timerEx(SEND_PLATFORM_EVT,100);
				}
				if(send_message_type==0x04&&send_flg==0x04)
				{
				 send_flg=0x00;
         send_message_type=0x00;				
				 Start_timerEx(JM_PLATFORM_REPLY_EVT,100);			
				}
				if(send_message_type==0x04&&send_flg==0x05)
				{
				 send_flg=0x04;			
				 Start_timerEx(JM_PLATFORM_REPLY_EVT,100);			
				}
				send_message_type=0x00;
				error_num++;
				RxReport2_step_flg=1;
				return;		
			 }		
			else
			 {
				 cmd_flg   = 0x01;//LINK_SERVER=0xE9
				 send_flg  = 0x00;
				 error_num=0x00;
				 next_atdata_flg=0;
				 Stop_timerEx(WG_REPLY_EVT);		 
				 Stop_timerEx(SEND_PLATFORM_EVT);     
				 Start_timerEx(NET_INIT_EVT,500);
				 RxReport2_step_flg=1;
				 return;			 
			 }			
  }	
 }

	if(strstr((char *)pBuf,"\r\nOK\r\n")!= NULL)	
	{  
		//0x01Ϊcdma��ʼ����0x02Ϊ�ظ�ƽ̨����ָ�0x03Ϊƽ̨�ϱ�����
		error_num=0x00;	
		if(send_message_type==0x01&&send_flg==0x01)
     { 
		 cmd_flg=next_atdata_flg;
		 Start_timerEx(NET_INIT_EVT,500);
		 RxReport2_step_flg=1;
		 return; 
		 }
		if(send_message_type==0x03&&send_flg==0x02)
     { 
		 module_send_flg=next_atdata_flg;
		 Start_timerEx(SEND_PLATFORM_EVT,100);//GSMΪ10��CDMAΪ100,����ATָ��
		 RxReport2_step_flg=1;
		 return; 
		 }
		 if(send_message_type==0x03&&send_flg==0x03)
     { 
		 module_send_flg=next_atdata_flg;
		 Start_timerEx(SEND_PLATFORM_EVT,150);//GSMΪ10ms,CDMAΪ50ms;������ָ��
		 RxReport2_step_flg=1;
		 return; 
		 }
		if(send_message_type==0x02&&send_flg==0x04)
     { 
		 Start_timerEx(WG_REPLY_EVT,100);//GSMΪ10��CDMAΪ100;��ATָ��
		 RxReport2_step_flg=1;
		 return; 
		 }
		 if(send_message_type==0x02&&send_flg==0x05)
     { 
		 Start_timerEx(WG_REPLY_EVT,150);//GSMΪ10ms��CDMAΪ50ms����DATAָ��
		 RxReport2_step_flg=1;
		 return; 
		 }
		 if(send_message_type==0x04&&send_flg==0x04)
     { 
		 Start_timerEx(JM_PLATFORM_REPLY_EVT,100);//GSMΪ10��CDMAΪ100;��ATָ��
		 RxReport2_step_flg=1;
		 return; 
		 }
		 if(send_message_type==0x04&&send_flg==0x05)
     { 
		 Start_timerEx(JM_PLATFORM_REPLY_EVT,150);//GSMΪ10ms��CDMAΪ50ms����DATAָ��
		 RxReport2_step_flg=1;
		 return; 
		 }
     RxReport2_step_flg=1;
		 return;		
		}
	
	
	 if((mqtt_bcmd=strstr((char *)pBuf,"^IPSTATE: 1,0,remote close"))!=NULL) //Զ�̷������ر�����·,CDMAģ��ָ��	
	 {
		cmd_flg   = 0x01;
		send_flg  = 0x01;
	  Stop_timerEx(SEND_PLATFORM_EVT);		 
		Start_timerEx(NET_INIT_EVT,500);		
    return;			 
	 }	
	 if((mqtt_bcmd=strstr((char *)pBuf,"\r\n^DSDORMANT: 1\r\n"))!=NULL) //Զ�̷�������CDMAģ�鷢���������״̬	
	 {
		cmd_flg   = 0x01;
		send_flg  = 0x01;
	  Stop_timerEx(SEND_PLATFORM_EVT);		 
		Start_timerEx(NET_INIT_EVT,500);		
    return;			 
	 }
  	 
}

static void cdma_tcp_receive(u8 len,u8 *pBuf)//����TCP/IP����ƽ̨����
{	
	u8 k = 0, first_k=0;
	 
	if(strstr((char *)pBuf,"^IPDATA:1,")!=NULL&&factory_gateway_set[0]==1)first_k=13; //CDMA��^IPDATA:1 ;GSM:^SISR: 0,ע��ո�
	if(strstr((char *)pBuf,"^SISR: 0,")!=NULL&&factory_gateway_set[0]==2)first_k=13; //CDMA��^IPDATA:1 ;GSM:^SISR: 0,ע��ո�
	if(first_k!=0)
	{
		/*ƽ̨����ָ��*/
		for(k=13;k<len;k++) 
		          //mqtt-CDMA��0xCCǰ����43���ֽڣ�k=43���������ⳤ��Ϊ15���ֽڲ���,���ӻ���ٶ��������ֽ���������Ӧ�޸�kֵ��
		          //TCP-CDMA ��0xCCǰ����ATָ��11���ֽ�+2���ֽڣ����ȣ���k=13 ;�������Ա�֤ǰ����ַ�������0xCCҲû�й�ϵ��
		         //mqtt-GSM��0xCCǰ����43���ֽڣ�k=43���������ⳤ��Ϊ15���ֽڲ���,���ӻ���ٶ��������ֽ���������Ӧ�޸�kֵ��
		          //TCP-GSM ��0xCCǰ��ATָ����13���ֽڣ�����2���ֽ��ǳ��ȣ���k=13 ;�������Ա�֤ǰ����ַ�������0xCCҲû�й�ϵ��
		{
		
			if(pBuf[k]==0xCC) //�ϱ�ƽ̨Э��涨��֡ͷ
			{
				
				if(pBuf[k-3]==real_send[7] && pBuf[k-2]==real_send[8] && pBuf[k-1]==real_send[9]) //�ɿ���ID
				{					
					handlecmd(pBuf+k-10,(((u16)pBuf[k-5])<<8)+pBuf[k-6]+6);
					//����ƽ̨�·��Ŀ���ָ���0xCCΪ��׼�õ�������ƽָ̨�0x01,0x00,...,0xDD��,����Ϊ����ָ���
					RxReport2_step_flg=2;
					return;
				}				
			}
		}		
	}  
}

static void cdma_mqtt_receive(u8 len,u8 *pBuf)//����mqtt����ƽ̨���ݴ�����
{
	if((mqtt_bcmd=strstr((char *)pBuf,"^IPDATA:1,4,"))!=NULL||(mqtt_bcmd=strstr((char *)pBuf,"^SISR: 0,4\r\n"))!=NULL)
	//mqtt-CDMA ^IPDATA:1,4, Ϊ12���ֽڣ�mqtt�����в��ܳ��֡�OK���͡�remote close��
	 
	//if((mqtt_bcmd=strstr((char *)pBuf,"^SISR: 0,4\r\n"))!=NULL) return; //GSM
		//mqtt-GSM ^SISR: 0,4\r\n Ϊ12���ֽڣ�mqtt�����в��ܳ��֡�OK���͡�remote close��
		//5E 53 49 53 52 3A 20 30 2C 34 0D 0A;ע��ո�
	{ 
		mqtt_bcmdxb=(u8 *)mqtt_bcmd-pBuf;//�����±�
		
		if(pBuf[mqtt_bcmdxb+12]==0x20&&pBuf[mqtt_bcmdxb+13]==0x02&&pBuf[mqtt_bcmdxb+14]==0x00&&pBuf[mqtt_bcmdxb+15]==0x00)			
		//CDMA����·1�յ�4���ֽڵ�mqtt-connect�ظ�ֵ������Ϊ0x20,0x02,0x00,0x00(0x01~0x04������)��publishû�лظ�ֵ
	  {	   		
		RxReport2_step_flg=2;
	  return;
	  }
   }
	
	if((mqtt_bcmd=strstr((char *)pBuf,"^IPDATA:1,5,"))!=NULL||(mqtt_bcmd=strstr((char *)pBuf,"^SISR: 0,5\r\n"))!=NULL)   //MQTT-CDMA;AT�����12���ֽ�
	//if((mqtt_bcmd=strstr((char *)pBuf,"^SISR: 0,5\r\n"))!=NULL) return; //GSM����  
		//MQTT-GSM;AT�����12���ֽڣ�ע��ո�
  	{
	   mqtt_bcmdxb=(u8 *)mqtt_bcmd-pBuf;
	
	  if(pBuf[mqtt_bcmdxb+12]==0x90&&pBuf[mqtt_bcmdxb+13]==0x03&&pBuf[mqtt_bcmdxb+14]==0x00&&pBuf[mqtt_bcmdxb+15]==0x01)
		   //CDMA����·1�յ�5���ֽڵ�mqtt-subscribe�ظ�ֵ��0x90,0x03(�����ֽڳ���)��0x00,0x01(msgid���ֽ�)��0x00(״̬λ)		
	  {
		RxReport2_step_flg=2;
		return;
	  }
	}
		if(strstr((char *)pBuf,"^IPDATA:1,")!=NULL||strstr((char *)pBuf,"^SISR: 0,")!=NULL) //CDMA��^IPDATA:1 ;GSM:^SISR: 0,ע��ո�
	{		/*ƽ̨����ָ��*/
		u8 k;
		k=28+factory_gateway_set[83];//�������ⳤ��
		for(;k<factory_gateway_set[83]+len;k++) 
		          //mqtt-CDMA��0xCCǰ����43���ֽڣ�k=43���������ⳤ��Ϊ15���ֽڲ���,���ӻ���ٶ��������ֽ���������Ӧ�޸�kֵ��
		          //TCP-CDMA ��0xCCǰ����ATָ��11���ֽ�+2���ֽڣ����ȣ���k=13 ;�������Ա�֤ǰ����ַ�������0xCCҲû�й�ϵ��
		         //mqtt-GSM��0xCCǰ����43���ֽڣ�k=43���������ⳤ��Ϊ15���ֽڲ���,���ӻ���ٶ��������ֽ���������Ӧ�޸�kֵ��
		          //TCP-GSM ��0xCCǰ��ATָ����13���ֽڣ�����2���ֽ��ǳ��ȣ���k=13 ;�������Ա�֤ǰ����ַ�������0xCCҲû�й�ϵ��
		{
		
			if(pBuf[k]==0xCC) //�ϱ�ƽ̨Э��涨��֡ͷ
			{
				
				if(pBuf[k-3]==real_send[7] && pBuf[k-2]==real_send[8] && pBuf[k-1]==real_send[9]) //�ɿ���ID
				{	         			
					handlecmd(pBuf+k-10,(((u16)pBuf[k-5])<<8)+pBuf[k-6]+6);//pBuf+k-10Ϊ0x01�ĵ�ַ��pBuf[k-5])<<8)+pBuf[k-6]+6Ϊ����ָ��ĳ���
					//����ƽ̨�·��Ŀ���ָ���0xCCΪ��׼�õ�������ƽָ̨�0x01,0x00,...,0xDD��,����Ϊ����ָ���
					RxReport2_step_flg=2;
					return;
				}				
			}
		}
	}
}	
static void cdma_sdk_receive(u8 len,u8 *pBuf)//����sdk����ƽ̨����	
{			
// ����˵����MQTTָ��ظ�����	
	
	if((mqtt_bcmd=strstr((char *)pBuf,"^IPDATA:1,4,"))!=NULL||(mqtt_bcmd=strstr((char *)pBuf,"^SISR: 0,4\r\n"))!=NULL)
	//mqtt-CDMA ^IPDATA:1,4, Ϊ12���ֽڣ�mqtt�����в��ܳ��֡�OK���͡�remote close��
	 
	//if((mqtt_bcmd=strstr(pBuf,"^SISR: 0,4\r\n"))!=NULL) return; //GSM
		//mqtt-GSM ^SISR: 0,4\r\n Ϊ12���ֽڣ�mqtt�����в��ܳ��֡�OK���͡�remote close��
		//5E 53 49 53 52 3A 20 30 2C 34 0D 0A;ע��ո�
	{ 
		mqtt_bcmdxb=(u8 *)mqtt_bcmd-pBuf;//�����±�
		
		if(pBuf[mqtt_bcmdxb+12]==0x20&&pBuf[mqtt_bcmdxb+13]==0x02&&pBuf[mqtt_bcmdxb+14]==0x00&&pBuf[mqtt_bcmdxb+15]==0x00)			
		//CDMA����·1�յ�4���ֽڵ�mqtt-connect�ظ�ֵ������Ϊ0x20,0x02,0x00,0x00(0x01~0x0������)��publishû�лظ�ֵ
	  {	   		
		RxReport2_step_flg=2;
	  return;
	  }
   }
	
	if((mqtt_bcmd=strstr((char *)pBuf,"^IPDATA:1,5,"))!=NULL||(mqtt_bcmd=strstr((char *)pBuf,"^SISR: 0,5\r\n"))!=NULL)   //MQTT-CDMA;AT�����12���ֽ�
	//if((mqtt_bcmd=strstr((char *)pBuf,"^SISR: 0,5\r\n"))!=NULL) return; //GSM����  
		//MQTT-GSM;AT�����12���ֽڣ�ע��ո�
  	{
	   mqtt_bcmdxb=(u8 *)mqtt_bcmd-pBuf;
	
	  if(pBuf[mqtt_bcmdxb+12]==0x90&&pBuf[mqtt_bcmdxb+13]==0x03&&pBuf[mqtt_bcmdxb+14]==0x00&&pBuf[mqtt_bcmdxb+15]==0x01)
		   //CDMA����·1�յ�5���ֽڵ�mqtt-subscribe�ظ�ֵ��0x90,0x03(�����ֽڳ���)��0x00,0x01(msgid���ֽ�)��0x00(״̬λ)		
	  {
		RxReport2_step_flg=2;
		return;
	  }
	}
//		memcpy(test,pBuf,100);
		mqtt_bcmdxb=match_str(pBuf,len,(unsigned char *)"{\"method\":\"",11);
	  if(factory_gateway_set[1]==4)mqtt_bcmdxb=match_str(pBuf,len,(unsigned char *)"{\"command\":\"",12);//����
 if(mqtt_bcmdxb!=0) //CDMA��^IPDATA:1 ;GSM:^SISR: 0,ע��ո�	
	{ 
		memcpy(ctrl_key,pBuf+mqtt_bcmdxb+1,4);
		memcpy(ctrl_value,pBuf+mqtt_bcmdxb+1+16,4);
//	 if(factory_gateway_set[1]==4)memcpy(ctrl_value,pBuf+mqtt_bcmdxb+16,4);//����
		/*ƽ̨����ָ�� {"method":"2100","params":"0001"}*/
		ctrl_adrr=dword_asc_hex(ctrl_key);//��4���ֽڵ�ASC��ת����ʮ��������0x2100
		ctrl_cmd=dword_asc_hex(ctrl_value);
		if(ctrl_adrr>=0x2100&&ctrl_adrr<0x2300)
		{			
			offset_addrX=(ctrl_adrr-0x2100)/16;
			offset_addrY=(ctrl_adrr&0x000F)/2;			
			Controllers[offset_addrX][offset_addrY]=ctrl_cmd&0x00FF;
			Controllers[offset_addrX][offset_addrY+1]=(ctrl_cmd>>8)&0x00FF;
			crtl_cmd_num[offset_addrX][offset_addrY/2]=50;//��վ���߷��ͼ�¼�������ظ�����3�Σ��յ���0�����ټ������͡�
			crtl_cmd_numWX[offset_addrX][offset_addrY/2]=50;//��վ���߷��ͼ�¼�������ظ�����3�Σ��յ���0�����ټ������͡�
			memset(sdk_ctrl_reply, 0, sizeof(sdk_ctrl_reply));//��һ�γ��ִ���ERROR����������ֵ�п��ܱ����������У�������Ҫ����
			sdk_ctrl_reply[0]=len;
      memcpy(sdk_ctrl_reply+1,pBuf,len);			
      Start_timerEx(WG_REPLY_EVT,150);
      RxReport2_step_flg=2;
     	return;		
		}	
   if(ctrl_adrr>=0x2300&&ctrl_adrr<0x2500)
		{			
			return;		
		}
   if(ctrl_adrr>=0x2500&&ctrl_adrr<0x2620)
		{			
			offset_addrX=(ctrl_adrr-0x2500)/4;						
			hand_auto_flg[offset_addrX][0]=ctrl_cmd&0x00FF;
			hand_auto_flg[offset_addrX][1]=(ctrl_cmd>>8)&0x00FF;
			hand_auto_count[offset_addrX]=5;//��ƽ̨�����ֶ�-�Զ�״̬��¼�������ظ�����5�Ρ�			
			memset(sdk_ctrl_reply, 0, sizeof(sdk_ctrl_reply));//��һ�γ��ִ���ERROR����������ֵ�п��ܱ����������У�������Ҫ����
			sdk_ctrl_reply[0]=len;
      memcpy(sdk_ctrl_reply+1,pBuf,len);			
      Start_timerEx(WG_REPLY_EVT,150);
      RxReport2_step_flg=2;
     	return;		
		}				
	} //����յ�������Ϣ��������	
}
//static void cdma_other_receive(u8 len,u8 *pBuf)//����other����ƽ̨����
//{
//}

static void cdma_SMS_receive(u8 len,u8 *pBuf)//����TCP/IP����CDMA����ش�����
	{ 
		u8 _cnt = 0;
	u8 is_mess = 0;
//	memcpy(test,pBuf,len);		
	if(factory_gateway_set[0]==1)is_mess=match_str(pBuf,len,(unsigned char *)"HCMT",4);//CDMA������HCMT��ͷ;GSMΪ+CMT
  if(factory_gateway_set[0]==2)is_mess=match_str(pBuf,len,(unsigned char *)"+CMT",4);//CDMA������HCMT��ͷ;GSMΪ+CMT
	/*�жϽ��յ��Ķ���ָ��*/
	if(is_mess)//�ж϶���
	{
		u8 i,j;
		u8 is_wakeup = match_str(pBuf,len,(unsigned char *)"HXCX",4);
		u8 jmwgrest = match_str(pBuf,len,(unsigned char *)"JMWGREST",8);
		u8 is_main   = match_str(pBuf,len,main_call,11);
		u8 is_voice  = match_str(pBuf,len,voice_call,11);
		u8 is_third  = match_str(pBuf,len,third_call,11);

		u8 mess_sta=0,mess_len=0,len_strt=0;

		unsigned char _cmd[]={"AT+CNMA\r\n"};//gsm��cdma��ͬ

		_cnt=0;

//		WriteDataToBuffer(2,_cmd,0,9);	//ȷ���յ�����
		memcpy(USART2SendTCB,_cmd,9);
    WriteDataToDMA_BufferTX2(9);	
		
		for(i=is_mess+1;i<len;i++) 		//^HCMT:13634171664,2012,06,19,21,23,03,0,1,6,0,0,0 PT8888 ��PT8888Ϊʵ����������Ϣ
		{
			if(pBuf[i]==0x2C)			//�жϵ����ţ����ŵ�ASCII��Ϊ0x2C
			{
				_cnt++;
				switch(_cnt)
				{
					case 1:
					//call_len=i-is_mess-2;
						break;
					case 9:
						len_strt=i+1;//������Ч���ȵ������±꣬�������6Ϊʵ����������Ϣ����
						break;
					case 10:
						mess_len=0;
						for(j=len_strt;j<i;j++)
						{
							if(j+1==i)
							{
								mess_len += pBuf[j]-0x30;
							}
							else
							{
								mess_len += (pBuf[j]-0x30)*10*(i-j-1);//mess_lenʵ����Ϣ����
							}
						}
						break;
					case 12:
						mess_sta=i+4;//���ſ�ʼ����һ���ֽڣ�mess_staΪʵ����Ϣ��ʼ���±�
						i=len;
						break;
					default:
						break;
				}
			}
		}
		
		if(is_wakeup)    //CDMA��ʼ��
		{			
				cmd_flg = 0x01;   				
				Start_timerEx(NET_INIT_EVT,150);			
		}
	 else if(is_main||is_voice||is_third)
		{
			if(jmwgrest)//�������յ�"JMWGREST"���ţ�ϵͳ������ѭ����ϵͳ��������
		  {
				__set_FAULTMASK(1);//�ر����е��ж�
				NVIC_SystemReset();//ϵͳ��λ
		    while(1);        
		  }
			if(pBuf[mess_sta]=='I'&&pBuf[mess_sta+1]=='P')//'IP'
			{
				if(len>=mess_sta+23&&mess_len==23&&is_number(pBuf+mess_sta+2,21))	  //�޸�IP
				{	
          factory_gateway_set[30]=21;					
					memcpy(factory_gateway_set+31,pBuf+mess_sta+2,21);
					Flash_Write(0x0807B000, factory_gateway_set, 255);
				}
			}
			
			else if(pBuf[mess_sta]==0x44&&pBuf[mess_sta+1]==0x42)//'D','B'
			{
				if(is_main)	   			//�޸�voice_call,�ҵ��ƶ�����
				{
					if(len>=mess_sta+13&&mess_len==13&&is_number(pBuf+mess_sta+2,11))
					{
						memcpy(factory_gateway_set+221,pBuf+mess_sta+2,11);
						Flash_Write(0x0807B000, factory_gateway_set, 255);
					}
				}
				else if(is_voice)//�ҵĵ��ź���
				{ 
					if(len>=mess_sta+13&&mess_len==13&&is_number(pBuf+mess_sta+2,11))
					{
						memcpy(factory_gateway_set+232,pBuf+mess_sta+2,11);
						Flash_Write(0x0807B000, factory_gateway_set, 255);
					}
				}
			}
			else if(pBuf[mess_sta]==0x44&&pBuf[mess_sta+1]==0x43)//'D','C'
			{	  
				if(is_third)
				{
					for(i=0;i<11;i++)
					{
						third_call[i]=0;
					}
					memcpy(factory_gateway_set+243,third_call,11);
					Flash_Write(0x0807B000, factory_gateway_set, 255);
				 }
				} 			
			
			else if(pBuf[mess_sta]=='X'&&pBuf[mess_sta+1]=='Y')	//'XY'�޸�Э�����͡�
			{	
				if(len>=mess_sta+5 && mess_len==5 && is_number(pBuf+mess_sta+2,3))
				{
					
					factory_gateway_set[1]=(pBuf[mess_sta+2]-0x30)*100+(pBuf[mess_sta+3]-0x30)*10+(pBuf[mess_sta+4]-0x30);
					Flash_Write(0x0807B000, factory_gateway_set, 255);
				}
			}
			
			else if(pBuf[mess_sta]=='W'&&pBuf[mess_sta+1]=='G'&&pBuf[mess_sta+2]<=0x32&&pBuf[mess_sta+2]>=0x30)//'WG'�޸�����ID 002002004254
			{
				u8 _tmp=0;
				if(len>=mess_sta+14&&mess_len==14&&is_number(pBuf+mess_sta+2,12))
				{
					for(i=mess_sta+2;i<mess_sta+12;i=i+3)
					{
						_tmp=(pBuf[i]-0x30)*100+(pBuf[i+1]-0x30)*10+(pBuf[i+2]-0x30);
						factory_gateway_set[2+(i-mess_sta-2)/3]=_tmp;
					}
					Flash_Write(0x0807B000, factory_gateway_set, 255);
				}
			}
			else if(pBuf[mess_sta]=='C'&&pBuf[mess_sta+1]=='K'&&pBuf[mess_sta+2]<=0x32&&pBuf[mess_sta+2]>=0x30)//'CK'�޸Ĳɿ���ID 004005006009
			{
				u8 _tmp=0;
				if(len>=mess_sta+14&&mess_len==14&&is_number(pBuf+mess_sta+2,12))
				{
					for(i=mess_sta+2;i<mess_sta+12;i=i+3)
					{
						_tmp=(pBuf[i]-0x30)*100+(pBuf[i+1]-0x30)*10+(pBuf[i+2]-0x30);
						factory_gateway_set[6+(i-mess_sta-2)/3]=_tmp;
					}
					Flash_Write(0x0807B000, factory_gateway_set, 255);
				}
			}
			else if(pBuf[mess_sta]=='K'&&pBuf[mess_sta+1]=='H'&&pBuf[mess_sta+2]=='J'&&pBuf[mess_sta+3]=='M'&&pBuf[mess_sta+4]=='I'&&pBuf[mess_sta+5]=='D')
				//'KHJMID'  �޸Ŀͻ���ID
			{				
				if(len>=mess_sta+mess_len)
				{	
          factory_gateway_set[52]=mess_len-6;					
					memcpy(factory_gateway_set+53,pBuf+mess_sta+6,mess_len-6);		
					Flash_Write(0x0807B000, factory_gateway_set, 255);
				}
			}
			else if(pBuf[mess_sta]=='D'&&pBuf[mess_sta+1]=='Y'&&pBuf[mess_sta+2]=='J'&&pBuf[mess_sta+3]=='M'&&pBuf[mess_sta+4]=='Z'&&pBuf[mess_sta+5]=='T')
				//'DYJMZT'�޸Ķ�������
			{
				if(len>=mess_sta+mess_len)
				{	
          factory_gateway_set[83]=mess_len-6;							
					memcpy(factory_gateway_set+84,pBuf+mess_sta+6,mess_len-6);		
					Flash_Write(0x0807B000, factory_gateway_set, 255);
				}
			}
			else if(pBuf[mess_sta]=='T'&&pBuf[mess_sta+1]=='S'&&pBuf[mess_sta+2]=='J'&&pBuf[mess_sta+3]=='M'&&pBuf[mess_sta+4]=='Z'&&pBuf[mess_sta+5]=='T')
				//'TSJMZT'�޸���������
			{
				if(len>=mess_sta+mess_len)
				{	
          factory_gateway_set[124]=mess_len-6;						
					memcpy(factory_gateway_set+125,pBuf+mess_sta+6,mess_len-6);		
					Flash_Write(0x0807B000, factory_gateway_set, 255);
				}
			}
			else if(pBuf[mess_sta]=='Y'&&pBuf[mess_sta+1]=='H'&&pBuf[mess_sta+2]=='J'&&pBuf[mess_sta+3]=='M'&&pBuf[mess_sta+4]=='M'&&pBuf[mess_sta+5]=='Z')
				//'YHJMMZ'�޸��û���
			{
				if(len>=mess_sta+mess_len)
				{
          factory_gateway_set[155]=mess_len-6;							
					memcpy(factory_gateway_set+156,pBuf+mess_sta+6,mess_len-6);		
					Flash_Write(0x0807B000, factory_gateway_set, 255);
				}
			}
			else if(pBuf[mess_sta]=='M'&&pBuf[mess_sta+1]=='M'&&pBuf[mess_sta+2]=='J'&&pBuf[mess_sta+3]=='M'&&pBuf[mess_sta+4]=='Y'&&pBuf[mess_sta+5]=='H')
				//'MMJMYH'�޸��û�����
			{
				if(len>=mess_sta+mess_len)
				{	
					factory_gateway_set[188]=mess_len-6;		
					memcpy(factory_gateway_set+189,pBuf+mess_sta+6,mess_len-6);		
					Flash_Write(0x0807B000, factory_gateway_set, 255);
				}
			}
		}
	}	
    return;		
}

static void gprs_cmd_receive(u8 len,u8 *pBuf)//�ƶ�TCP/IP����CDMA����ش�����
{	
	if((mqtt_bcmd=strstr((char *)pBuf,"^SISR: 0,0\r\n"))!=NULL)
	//"^SISR: 0,0"��ʾgsm�������Ѷ��գ�ֹͣ��ȡ������
	{		 
		RxReport2_step_flg=1;			
		return;
	}
//	if((mqtt_bcmd=strstr((const char *)pBuf,"^SISR: 0,1\r\n"))!=NULL) //���ƽ̨�Ƿ�ָ��,ע��һ��Ҫ�пո�;\r\nΪ0x0D 0x0A;"^SISR: 0,1\r\n"��ʾ�����ݿɶ�
//   {  
//     if(readbuf_fast_flg==0x00)
//		 {
//			 readbuf_fast_flg=0x01; 
//			 //����gsmģ�黺������ȡ�ٶȣ������ȡ����ִ�л��ҡ���300ms�ٶ���������3ms�ǵ�һ�ζ�ȡ�ٶȡ�ֻ�ж���gsm ����������readbuf_fast_flg=0x00 
//			 Start_timerEx(GSM_AT_READBUF,3);//��GSMģ���ȡƽ̨���͵�����
//		 }			 
//    return;		 
//   }
	 if(strstr((char *)pBuf,"^SHUTDOWN")!= NULL)while(1);  //GSM:"^SHUTDOWN"
	 if(strstr((char *)pBuf,"^SIS: 0,0,")!= NULL||strstr((char *)pBuf,"^SIS: 0,2,")!= NULL)//GSM:internet����ʱ����������
		 {		      
			cmd_flg   = 0x01;//REST
			send_flg  = 0x01;				
			Stop_timerEx(WG_REPLY_EVT);
			Stop_timerEx(SEND_PLATFORM_EVT);			
			Start_timerEx(NET_INIT_EVT,500);
      RxReport2_step_flg=1;		 
			return;
	   }
	if(strstr((char *)pBuf,"ERROR")!= NULL)//����ϵͳ�������������Ժ�ָ�
  { 
   if(send_message_type==0x01||send_message_type==0x02||send_message_type==0x03||send_message_type==0x04) //0x02Ϊ�ظ�ƽ̨����ָ�0x03Ϊƽ̨�ϱ�����
	 {		
		if(error_num<MAX_ERROR_NUM) //����ATָ������ظ�ִ�и�ָ�������MAX_ERROR_NUM����
	 {
		if(send_message_type==0x01&&send_flg==0x01)
		{			 
		 Start_timerEx(NET_INIT_EVT,1000);	//����ִ�и�ָ�reprot_flgû�иı�		
		}
		if(send_message_type==0x02&&send_flg==0x04)
		{
     send_flg=0x00;
     send_message_type=0x00;			
		 Start_timerEx(WG_REPLY_EVT,100);			
		}
		if(send_message_type==0x02&&send_flg==0x05)
		{
     send_flg=0x04;			
		 Start_timerEx(WG_REPLY_EVT,100);			
		}
		
		if(send_message_type==0x03&&send_flg==0x02)
		{		
    send_flg=0x00;
    send_message_type=0x00;	    
		Start_timerEx(SEND_PLATFORM_EVT,100);
		}
		if(send_message_type==0x03&&send_flg==0x03)
		{
    send_flg=0x02;	    
		Start_timerEx(SEND_PLATFORM_EVT,100);
		}
		if(send_message_type==0x04&&send_flg==0x04)
		{
     send_flg=0x00;
     send_message_type=0x00;		
		 Start_timerEx(JM_PLATFORM_REPLY_EVT,100);			
		}
		if(send_message_type==0x04&&send_flg==0x05)
		{
     send_flg=0x04;			
		 Start_timerEx(JM_PLATFORM_REPLY_EVT,100);			
		}
		send_message_type=0x00;
		error_num++;
		RxReport2_step_flg=1;
    return;		
	 }		
		else
	 {
		 cmd_flg   = 0x01;//LINK_SERVER=0xE9
		 send_flg  = 0x00;
     error_num=0x00;
     next_atdata_flg=0;
		 Stop_timerEx(WG_REPLY_EVT);		 
	   Stop_timerEx(SEND_PLATFORM_EVT);     
  	 Start_timerEx(NET_INIT_EVT,500);
		 RxReport2_step_flg=1;
     return;			 
   }			
  }	
 } 
	if(strstr((char *)pBuf,"^SISW: 0,1")!= NULL) //��ֹһ��ָ���ظ�ִ�ж��飬����AT^SISOָ����OK��^SISW: 0,1,1460
   {		 
		RxReport2_step_flg=1;			
		return;
	 }
  if(strstr((char *)pBuf,"\r\nOK\r\n")!= NULL||strstr((char *)pBuf,"^SISW: 0,"))	//"^SISW: 0,1"��ʾGSMģ��ɹ�����
	 {  
		//0x01Ϊcdma��ʼ����0x02Ϊ�ظ�ƽ̨����ָ�0x03Ϊƽ̨�ϱ�����
		error_num=0x00;	
		if(send_message_type==0x01&&send_flg==0x01)
     { 
		 cmd_flg=next_atdata_flg;
		 Start_timerEx(NET_INIT_EVT,500);
		 RxReport2_step_flg=1;
		 return; 
		 }
		if(send_message_type==0x03&&send_flg==0x02)
     { 
		 module_send_flg=next_atdata_flg;
		 Start_timerEx(SEND_PLATFORM_EVT,10);//GSMΪ10��CDMAΪ100,����ATָ��
		 RxReport2_step_flg=1;
		 return; 
		 }
		 if(send_message_type==0x03&&send_flg==0x03)
     { 
		 module_send_flg=next_atdata_flg;
		 Start_timerEx(SEND_PLATFORM_EVT,30);//GSMΪ10ms,CDMAΪ50ms;������ָ��
		 RxReport2_step_flg=1;
		 return; 
		 }
		if(send_message_type==0x02&&send_flg==0x04)
     { 
		 Start_timerEx(WG_REPLY_EVT,10);//GSMΪ10��CDMAΪ100;��ATָ��
		 RxReport2_step_flg=1;
		 return; 
		 }
		if(send_message_type==0x02&&send_flg==0x05)
     { 
		 Start_timerEx(WG_REPLY_EVT,30);//GSMΪ10ms��CDMAΪ50ms����DATAָ��
		 RxReport2_step_flg=1;
		 return; 
		 }
		 if(send_message_type==0x04&&send_flg==0x04)
     { 
		 Start_timerEx(JM_PLATFORM_REPLY_EVT,10);//GSMΪ10��CDMAΪ100;��ATָ��
		 RxReport2_step_flg=1;
		 return; 
		 }
		if(send_message_type==0x04&&send_flg==0x05)
     { 
		 Start_timerEx(JM_PLATFORM_REPLY_EVT,30);//GSMΪ10ms��CDMAΪ50ms����DATAָ��
		 RxReport2_step_flg=1;
		 return; 
		 }
     RxReport2_step_flg=1;
		 return;		
		}
}

static void gprs_other_receive(u8 len,u8 *pBuf)//�ƶ�other����ƽ̨����
{
}

static void Ethernet_tcp_receive(u8 len,u8 *pBuf)//�ƶ�TCP/IP����ƽ̨����
{
	u8 k = 0;

		/*ƽ̨����ָ��*/
		for(k=13;k<len;k++) 
		          //mqtt-CDMA��0xCCǰ����43���ֽڣ�k=43���������ⳤ��Ϊ15���ֽڲ���,���ӻ���ٶ��������ֽ���������Ӧ�޸�kֵ��
		          //TCP-CDMA ��0xCCǰ����ATָ��11���ֽ�+2���ֽڣ����ȣ���k=13 ;�������Ա�֤ǰ����ַ�������0xCCҲû�й�ϵ��
		         //mqtt-GSM��0xCCǰ����43���ֽڣ�k=43���������ⳤ��Ϊ15���ֽڲ���,���ӻ���ٶ��������ֽ���������Ӧ�޸�kֵ��
		          //TCP-GSM ��0xCCǰ��ATָ����13���ֽڣ�����2���ֽ��ǳ��ȣ���k=13 ;�������Ա�֤ǰ����ַ�������0xCCҲû�й�ϵ��
		{
		
			if(pBuf[k]==0xCC) //�ϱ�ƽ̨Э��涨��֡ͷ
			{
				
				if(pBuf[k-3]==real_send[7] && pBuf[k-2]==real_send[8] && pBuf[k-1]==real_send[9]) //�ɿ���ID
				{					
					handlecmd(pBuf+k-10,(((u16)pBuf[k-5])<<8)+pBuf[k-6]+6);
					//����ƽ̨�·��Ŀ���ָ���0xCCΪ��׼�õ�������ƽָ̨�0x01,0x00,...,0xDD��,����Ϊ����ָ���
					RxReport2_step_flg=2;
					return;
				}				
			}
		}	
}
static void Ethernet_mqtt_receive(u8 len,u8 *pBuf)//�ƶ�mqtt����ƽ̨����	
{
}
static void Ethernet_sdk_receive(u8 len,u8 *pBuf)//�ƶ�sdk����ƽ̨����
{
}
static void Ethernet_other_receive(u8 len,u8 *pBuf)//�ƶ�other����ƽ̨����
{
}


//�ظ�ƽ̨���������ѯ
static void wg_reply_cmd(void)//�ظ�ƽ̨���������ѯ
{
	switch(factory_gateway_set[0]) //��������=1 ���ţ�2 �ƶ���3 ��̫����4 WiFi��5 USB��6 �����ӣ�
 {
//���� cdma ��ʼ---------------------------------
  case 0x01:		
		switch(factory_gateway_set[1]) //Э������=1 TCP/IP;2 MQTT; 3 SDK; 4 ������
		{
			//����TCP/IP
			case 0x01:
				cdma_tcp_reply();
			break;
			//����MQTT
			case 0x02:
				cdma_mqtt_reply();
			break;
			//����SDK
			case 0x03:
				cdma_sdk_reply();
			break;
			//����other
			case 0x04:
				cdma_other_reply();
			break;
		  default:
      			break;
		}			
	break;
//���� cdma ����------------------------------------
		
//�ƶ� gprs	��ʼ====================================
	case 0x02:
		switch(factory_gateway_set[1]) //Э������=1 TCP/IP;2 MQTT; 3 SDK; 4 ������
		{
			//�ƶ�TCP/IP
			case 0x01:
				cdma_tcp_reply();//����gprs_tcp_reply
			break;
			//�ƶ�MQTT
			case 0x02:
				cdma_mqtt_reply();//����gprs_mqtt_reply
			break;
			//�ƶ�SDK
			case 0x03:
				cdma_sdk_reply();//����gprs_sdk_reply
			break;
			//�ƶ�other
			case 0x04:
				gprs_other_reply();
			break;
			default:
      			break;
		}
	 break;
//�ƶ� gprs	����====================================
	
//Ethernet ��ʼ---------------------------------
  case 0x03:		
		switch(factory_gateway_set[1]) //Э������=1 TCP/IP;2 MQTT; 3 SDK; 4 ������
		{
			//Ethernet TCP/IP
			case 0x01:
				Ethernet_tcp_reply();
			break;
			//Ethernet MQTT
			case 0x02:
				Ethernet_mqtt_reply();
			break;
			//Ethernet SDK
			case 0x03:
				Ethernet_sdk_reply();
			break;
			//Ethernet other
			case 0x04:
				Ethernet_other_reply();
			break;
		  default:
      			break;
		}			
	break;
//Ethernet ����------------------------------------
		
//WiFi ��ʼ&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
	case 0x04:
	          break;

//WiFi ����&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
	
//USB  ��ʼ########################################
	case 0x05:
		
	          break;
//USB ����#########################################
	default:
  break;
 }
}

static void cdma_tcp_reply(void)//����TCP/IP�ظ�ƽ̨���������ѯ
{
	if(send_mess)			//���ڷ��Ͷ����ϱ����ȴ�50ms��ִ��
	{
		Start_timerEx(WG_REPLY_EVT,50);
		return;
	}
//	if(send_flg ==0x01||send_flg ==0x02||send_flg ==0x03)
// 	{		
//		Start_timerEx(WG_REPLY_EVT,500);
//		return;
//	}
  if(send_message_type !=0x02&&send_message_type !=0x00) //����վ��ʼ���������ZZ_Wired_flag[65]=1��||ZZ_Wireles_flag[65]==1
 	{		
		Start_timerEx(WG_REPLY_EVT,500);
		return;
	}	 		
	send_message_type=0x02;	//���յ�GSM��ERROR����ʱʹ�ã�0x02����Ϊ�ظ�ƽ̨�Ŀ���ָ��
	if(send_flg ==0x00)
	{		
	 //send_at_cdma("046");//�������ⳤ�Ȳ��ܱ䣬������ӣ������Ӧ���ֽ���(�Ϊ99);publish�������������Ϊmqtt_real_send
	 if(factory_gateway_set[0]==1)send_at_cdma((unsigned char *)"027",21);//TCB,CDMA	����Ϊ31=6+��Ϣ����=6+25=31��CDMA��ATָ���Ϊ21��GPRS��ATָ���Ϊ15
	 if(factory_gateway_set[0]==2)send_at_gprs((unsigned char *)"027",15);//TCB,GPRS  TCP ����Ϊ43=6+��Ϣ����=6+37=43��CDMA��ATָ���Ϊ21��GPRS��ATָ���Ϊ15	
	 //TCB	����Ϊ31=6+��Ϣ����=6+21=27���ر�˵������������Ϊ29����CDMA��ATָ���Ϊ21��GPRS��ATָ���Ϊ15���ر�˵������������Ϊ29��
	 send_flg =0x04;
	 Start_timerEx(WG_REPLY_EVT,CMD_WAIT_TIME);
	 return;
	}
	if(send_flg ==0x04)
	{
		
	 //mqtt_publish(reply_xiafa_data,27);//mqtt���ݰ�����Ϊ46��MQTTͷ��19���ֽڣ����Ʒ�������(�·��Ŀ�������)27���ֽڣ��ر�˵������������Ϊ29��
	 //send_data_module(mqtt_real_send,"046");//���ر�˵������������Ϊ29��      
	  send_data_module(reply_xiafa_data,(unsigned char *)"027");//TCB���ر�˵������������Ϊ29��
	 send_flg=0x05;
   Start_timerEx(WG_REPLY_EVT,CMD_WAIT_TIME);
	 return;		
	}
  if(send_flg ==0x05)		
	{
	send_flg=0x00;
	send_message_type=0x00;
	return;	
	}
	send_flg=0x00;
	send_message_type=0x00;
	return;	
}

static void cdma_mqtt_reply(void)//����mqtt�ظ�ƽ̨���������ѯ
{
	if(send_mess)			//���ڷ��Ͷ����ϱ����ȴ�50ms��ִ��
	{
		Start_timerEx(WG_REPLY_EVT,50);
		return;
	}
//	if(send_flg ==0x01||send_flg ==0x02||send_flg ==0x03)
// 	{		
//		Start_timerEx(WG_REPLY_EVT,500);
//		return;
//	}
  if(send_message_type !=0x02&&send_message_type !=0x00) //����վ��ʼ���������ZZ_Wired_flag[65]=1��||ZZ_Wireles_flag[65]==1
 	{		
		Start_timerEx(WG_REPLY_EVT,500);
		return;
	}	 			
	send_message_type=0x02;	//���յ�GSM��ERROR����ʱʹ�ã�0x02����Ϊ�ظ�ƽ̨�Ŀ���ָ��
	if(send_flg ==0x00)
	{
	 send_message_len2=6+18+3;//6+18+3
	 mqtt_publish(reply_xiafa_data,send_message_len2);//MQTT ʵ�ʷ���75���ֽ�(��������������)��
   bytelen_to_asc((unsigned char *)message_len_char2,mqtt_len);//mqtt_lenȫ�ֱ�������mqtt_publish�����и�ֵ
	 if(factory_gateway_set[0]==1)send_at_cdma((unsigned char *)message_len_char2,21);//TCB,CDMA	����Ϊ31=6+��Ϣ����=6+25=31��CDMA��ATָ���Ϊ21��GPRS��ATָ���Ϊ15
	 if(factory_gateway_set[0]==2)send_at_gprs((unsigned char *)message_len_char2,15);//TCB,GPRS  TCP ����Ϊ43=6+��Ϣ����=6+37=43��CDMA��ATָ���Ϊ21��GPRS��ATָ���Ϊ15	
//send_at_cdma("046");//�������ⳤ�Ȳ��ܱ䣬������ӣ������Ӧ���ֽ���(�Ϊ99);publish�������������Ϊmqtt_real_send
//	 send_at_cdma((unsigned char *)"027",21);//TCB	����Ϊ31=6+��Ϣ����=6+21=27���ر�˵������������Ϊ29����CDMA��ATָ���Ϊ21��GPRS��ATָ���Ϊ15���ر�˵������������Ϊ29��
	 send_flg =0x04;
	 Start_timerEx(WG_REPLY_EVT,CMD_WAIT_TIME);
	 return;
	}
	if(send_flg ==0x04)
	{
	 send_message_len2=6+18+3;//6+18+3=27 �յ��Ŀ��������
	 mqtt_publish(reply_xiafa_data,send_message_len2);//MQTT ʵ�ʷ���75���ֽ�(��������������)��
   bytelen_to_asc((unsigned char *)message_len_char2,mqtt_len);//mqtt_lenȫ�ֱ�������mqtt_publish�����и�ֵ
	 //mqtt���ݰ�����Ϊ46��MQTTͷ��19���ֽڣ����Ʒ�������(�·��Ŀ�������)27���ֽڣ��ر�˵������������Ϊ29��
	 send_data_module(ReportData2,(unsigned char *)message_len_char2);//���ر�˵������������Ϊ29��      
// send_data_module(reply_xiafa_data,(unsigned char *)"027");//TCB���ر�˵������������Ϊ29��
	 send_flg=0x05;
   Start_timerEx(WG_REPLY_EVT,CMD_WAIT_TIME);
	 return;		
	}
  if(send_flg ==0x05)		
	{
	send_flg=0x00;
	send_message_type=0x00;
	return;	
	}
	send_flg=0x00;
	send_message_type=0x00;
	return;	
	
}

static void cdma_sdk_reply(void)//����sdk�ظ�ƽ̨���������ѯ
{
 if(send_mess)			//���ڷ��Ͷ����ϱ����ȴ�50ms��ִ��
	{
		Start_timerEx(WG_REPLY_EVT,50);
		return;
	}
//	if(send_flg ==0x01||send_flg ==0x02||send_flg ==0x03)
// 	{		
//		Start_timerEx(WG_REPLY_EVT,500);
//		return;
//	}
  if(send_message_type !=0x02&&send_message_type !=0x00) //����վ��ʼ���������ZZ_Wired_flag[65]=1��||ZZ_Wireles_flag[65]==1
 	{		
		Start_timerEx(WG_REPLY_EVT,500);
		return;
	}	 				
	send_message_type=0x02;	//���յ�GSM��ERROR����ʱʹ�ã�0x02����Ϊ�ظ�ƽ̨�Ŀ���ָ��
	if(send_flg ==0x00)
	{ 	
     u8 sdk_bcmdxb1;		
//		sdk_len=made_keyX_value(0x2100+offset_addrX*4*4,Controllers[offset_addrX],4,real_send);
		//0x2100����������״̬��ַ���ϱ�1��������4��״̬���� ;offset_addrX��cdma_sdk_receive����������
		sdk_bcmdxb=match_str(sdk_ctrl_reply,sdk_ctrl_reply[0],(unsigned char *)"{\"method\":",10);//ƥ�䷵�صĵ�ַ�ǡ������ĵ�ַ����������Դ��ַ��2
		memcpy(jm_reply_cmd+1,sdk_ctrl_reply+sdk_bcmdxb+1,5);//{"method":"2100","params":"0009"},Ϊ����ƽ̨����ظ�����
	  memcpy(jm_reply_cmd+7,sdk_ctrl_reply+sdk_bcmdxb+18,4);//�ظ�����"{H2100:0009}"��Ϊ����ƽ̨����ظ�����
    sdk_bcmdxb1=match_str(sdk_ctrl_reply+sdk_bcmdxb,sdk_ctrl_reply[0]-sdk_bcmdxb,(unsigned char *)"}",1);
		sdk_len=sdk_bcmdxb1+10;//sdk_bcmdxb1Ϊ������Ч�ֽ�����10Ϊ{"method":���ֽ���
    memset(real_send, 0,sizeof(real_send));//�ϴη���ֵ�п��ܱ����������У�������Ҫ����		
		memcpy(real_send,sdk_ctrl_reply+sdk_bcmdxb-10,sdk_len);		
		
		/*����Ϊpublish��Ӧ�����������ȡ���޸�*/			
		sdk_bcmdxb=match_str(sdk_ctrl_reply,sdk_ctrl_reply[0],(unsigned char *)"v1/devices/me/rpc/request/",26);
		//sdk_ctrl_reply[0]��������ȣ�sdk_ctrl_reply[1]����������֡
		memset(SDK_topicString_pub1+27, 0, 128-27);//�ϴη���ֵ�п��ܱ����������У�������Ҫ����
		memcpy(SDK_topicString_pub1+27,sdk_ctrl_reply+sdk_bcmdxb,sdk_ctrl_reply[sdk_bcmdxb-26-1]-26);//sdk_ctrl_reply[sdk_bcmdxb-1]�������ⳤ��
    //27Ϊ v1/devices/me/rpc/response/���ȣ�26Ϊv1/devices/me/rpc/request/���ȣ�����Ϊv1/devices/me/rpc/request/1061 ����1061ΪrequestID�����ȿɱ�			
		mqtt_publish1(real_send,sdk_len);//�����ͺ���ΪSDK�ر�ָ���ģ���Ϊ�涨�˿�����״̬�ϱ�����������
    bytelen_to_asc((unsigned char *)message_len_char2,mqtt_len);//mqtt_lenΪȫ�ֱ�������mqtt_publish1�����и�ֵ		
		if(factory_gateway_set[0]==1)send_at_cdma((unsigned char *)message_len_char2,21);//TCB,CDMA	����Ϊ31=6+��Ϣ����=6+25=31��CDMA��ATָ���Ϊ21��GPRS��ATָ���Ϊ15
	  if(factory_gateway_set[0]==2)send_at_gprs((unsigned char *)message_len_char2,15);//TCB,GPRS  TCP ����Ϊ43=6+��Ϣ����=6+37=43��CDMA��ATָ���Ϊ21��GPRS��ATָ���Ϊ15
    send_flg=0x04;    
    Start_timerEx(WG_REPLY_EVT,CMD_WAIT_TIME);	 
	  return;
	}
	if(send_flg ==0x04)
	{		
//		sdk_len=made_keyX_value(0x2100+offset_addrX*4*4,Controllers[offset_addrX],4,real_send);
		//0x2100����������״̬��ַ���ϱ�1��������4��״̬���� ;offset_addrX��cdma_sdk_receive����������
		mqtt_publish1(real_send,sdk_len);//�����ͺ���ΪSDK�ر�ָ���ģ���Ϊ�涨�˿�����״̬�ϱ�����������
    bytelen_to_asc((unsigned char *)message_len_char2,mqtt_len);//mqtt_lenΪȫ�ֱ�������mqtt_publish1�����и�ֵ		
		send_data_module(ReportData2,(unsigned char *)message_len_char2);//ReportData2Ϊʵ�ʷ������ݣ���mqtt_publish�����а�װ		
		send_flg=0x05;					
		Start_timerEx(WG_REPLY_EVT,CMD_WAIT_TIME);    
	 return;		
	}
  if(send_flg ==0x05)		
	{
	  send_flg=0x00;
		send_message_type=0x00;
		Start_timerEx(JM_PLATFORM_REPLY_EVT,300);		
	return;	
	}
	send_flg=0x00;
	send_message_type=0x00;
	return;		
}	

static void jm_platform_reply(void)//����ƽ̨���ٷ��ؿ�������
{
 if(send_mess)			//���ڷ��Ͷ����ϱ����ȴ�50ms��ִ��
	{
		Start_timerEx(JM_PLATFORM_REPLY_EVT,50);
		return;
	}
//	if(send_flg ==0x01||send_flg ==0x02||send_flg ==0x03)
// 	{		
//		Start_timerEx(JM_PLATFORM_REPLY_EVT,500);
//		return;
//	}
  if(send_message_type !=0x04&&send_message_type !=0x00) //����վ��ʼ���������ZZ_Wired_flag[65]=1��||ZZ_Wireles_flag[65]==1
 	{		
		Start_timerEx(JM_PLATFORM_REPLY_EVT,500);
		return;
	}	 					
	send_message_type=0x04;	//���յ�GSM��ERROR����ʱʹ�ã�0x04����Ϊ���ٻظ�����ƽ̨�Ŀ���ָ��
	if(send_flg ==0x00)
	{ 	
//		  memset(real_send, 0,sizeof(real_send));//�ϴη���ֵ�п��ܱ����������У�������Ҫ����		
//		  memcpy(real_send,jm_reply_cmd,12);//�ظ�����"{H2100:0009}"
      mqtt_publish(jm_reply_cmd,12);
      bytelen_to_asc((unsigned char *)message_len_char1,mqtt_len);//mqtt_lenΪȫ�ֱ�������mqtt_publish11�����и�ֵ		
		  if(factory_gateway_set[0]==1)send_at_cdma((unsigned char *)message_len_char1,21);//TCB,CDMA	����Ϊ31=6+��Ϣ����=6+25=31��CDMA��ATָ���Ϊ21��GPRS��ATָ���Ϊ15
		  if(factory_gateway_set[0]==2)send_at_gprs((unsigned char *)message_len_char1,15);//TCB,GPRS  TCP ����Ϊ43=6+��Ϣ����=6+37=43��CDMA��ATָ���Ϊ21��GPRS��ATָ���Ϊ15			
			send_flg   = 0x04; //��ʾ��ƽ̨�ϱ���atָ������ִ�У�����ֻ��ִ��dataָ��    
     Start_timerEx(JM_PLATFORM_REPLY_EVT,CMD_WAIT_TIME);	 
	  return;
	}	
	if(send_flg ==0x04)
	{
		mqtt_publish(jm_reply_cmd,12);//�����ͺ���ΪSDK�ر�ָ���ģ���Ϊ�涨�˿�����״̬�ϱ�����������
    bytelen_to_asc((unsigned char *)message_len_char2,mqtt_len);//mqtt_lenΪȫ�ֱ�������mqtt_publish�����и�ֵ		
		send_data_module(ReportData2,(unsigned char *)message_len_char2);//ReportData2Ϊʵ�ʷ������ݣ���mqtt_publish�����а�װ		
		send_flg=0x05;		
    Start_timerEx(JM_PLATFORM_REPLY_EVT,CMD_WAIT_TIME);		
	 return;		
	}
  if(send_flg ==0x05)		
	{
	send_flg=0x00;
	send_message_type=0x00;
	return;	
	}
	send_flg=0x00;
	send_message_type=0x00;
	return;		
}

static void cdma_other_reply(void)//����other�ظ�ƽ̨���������ѯ
{
}

static void gprs_other_reply(void)//�ƶ�other�ظ�ƽ̨���������ѯ
{
}

static void Ethernet_tcp_reply(void)//Ethernet TCP/IP�ظ�ƽ̨���������ѯ 
{
	if(send_mess)			//���ڷ��Ͷ����ϱ����ȴ�50ms��ִ��
	{
		Start_timerEx(WG_REPLY_EVT,50);
		return;
	}
//	if(send_flg ==0x01||send_flg ==0x02||send_flg ==0x03)
// 	{		
//		Start_timerEx(WG_REPLY_EVT,500);
//		return;
//	}
  if(send_message_type !=0x02&&send_message_type !=0x00) //����վ��ʼ���������ZZ_Wired_flag[65]=1��||ZZ_Wireles_flag[65]==1
 	{		
		Start_timerEx(WG_REPLY_EVT,500);
		return;
	}	 					
	send_message_type=0x02;	//���յ�GSM��ERROR����ʱʹ�ã�0x02����Ϊ�ظ�ƽ̨�Ŀ���ָ��
	if(send_flg ==0x00)
	{	 
	 send_flg =0x04;
	 Start_timerEx(WG_REPLY_EVT,Ethernet_WAIT_TIME);
	 return;
	}
	if(send_flg ==0x04)
	{		
	 //mqtt_publish(reply_xiafa_data,27);//mqtt���ݰ�����Ϊ46��MQTTͷ��19���ֽڣ����Ʒ�������(�·��Ŀ�������)27���ֽڣ��ر�˵������������Ϊ29��
	 //send_data_module(mqtt_real_send,"046");//���ر�˵������������Ϊ29��      
	  send_Ethernet_module(reply_xiafa_data,(unsigned char *)"027");//TCB���ر�˵������������Ϊ29��
	 send_flg=0x05;
   Start_timerEx(WG_REPLY_EVT,Ethernet_WAIT_TIME);
	 return;		
	}
  if(send_flg ==0x05)		
	{
	send_flg=0x00;
	send_message_type=0x00;
	return;	
	}
	send_flg=0x00;
	send_message_type=0x00;
	return;	
}
static void Ethernet_mqtt_reply(void)//Ethernet mqtt�ظ�ƽ̨���������ѯ	
{
}
static void Ethernet_sdk_reply(void)//Ethernet sdk�ظ�ƽ̨���������ѯ		
{
}
static void Ethernet_other_reply(void)//Ethernet other�ظ�ƽ̨���������ѯ
{
}

/*��ƽ̨�������ݺ�������*/
static void send_at_cdma(unsigned char send_data_len[],u8 at_cmd_len)		//�������ݵ�atָ�CDMA;CDMAΪat_cmd_len=21;GSMΪat_cmd_len=15
    {   			  
	       at_send_cdma[16]=send_data_len[0];//CDMAΪat_send[16]~[18]�g;GSMΪat_send[10]~[12]
         at_send_cdma[17]=send_data_len[1];
			   at_send_cdma[18]=send_data_len[2];			
//	       WriteDataToBuffer(2,at_send,0,at_cmd_len); //����ATָ��;CDMAΪat_cmd_len=21;GSMΪat_cmd_len=15
			   memcpy(USART2SendTCB,at_send_cdma,at_cmd_len);
         WriteDataToDMA_BufferTX2(at_cmd_len); 
		 }
static void send_at_gprs(unsigned char send_data_len[],u8 at_cmd_len)		//�������ݵ�atָ�ͨѶģ�飬CDMA��at_cmd_len=21��GPRS:at_cmd_len=15 
{
         at_send_gprs[10]=send_data_len[0];//CDMAΪat_send[16]~[18]��
         at_send_gprs[11]=send_data_len[1];
			   at_send_gprs[12]=send_data_len[2];			  
			   memcpy(USART2SendTCB,at_send_gprs,at_cmd_len);
         WriteDataToDMA_BufferTX2(at_cmd_len); 
}
		
static void send_data_module(unsigned char send_data[],unsigned char send_data_len[])		//�������ݵ�CDMA
    {    
		   u8 temp_len;			   
	       temp_len=(send_data_len[0]-0x30)*100+(send_data_len[1]-0x30)*10+(send_data_len[2]-0x30);//�����ͳ���ASC��ת����16������			
//	       WriteDataToBuffer(2,send_data,0,temp_len); //�������� 
			memcpy(USART2SendTCB,send_data,temp_len);//ReportData2
      WriteDataToDMA_BufferTX2(temp_len);  
    }
		
static void send_Ethernet_module(unsigned char send_data[],unsigned char send_data_len[] )		//�������ݵ�Ethernetģ��;
    {    
		  u8 temp_len;			   
	    temp_len=(send_data_len[0]-0x30)*100+(send_data_len[1]-0x30)*10+(send_data_len[2]-0x30);//�����ͳ���ASC��ת����16������	
			memcpy(USART1SendTCB,send_data,temp_len);//ReportData2
      WriteDataToDMA_BufferTX1(temp_len);  
    }
		
static void alter_send(u8 n,u8 flg)
{
	u16 CRC_Value;
	u8 i=0;
	
		u16 _lastaddr = real_send[20]+(((u16)real_send[21])<<8);
		_lastaddr     = _lastaddr+(n*flg);//�ϱ�ʱ��2*2*4=16��ÿ����������������Ϊ4���ֽڣ�ǰ2���ֽ�Ϊ0
		real_send[20] = _lastaddr&0xFF;
		real_send[21] = _lastaddr>>8;
		
	for(i=0;i<n/2;i=i+2)	//01 00 00 00 65 00 02 02 02 03 cc 01 02 03 04 05 06 07 64 04 00 21 50 00 ........cr cr dd
	{		
			real_send[24+i*2]  =0x00;
			real_send[25+i*2]=0x00;
			real_send[26+i*2]=Controllers[flg][i];//8(�ɿ���ID)+1(����)+2(��ʼ��ַ)+2(�ֽ�������)=13+16(�ֽ���)=29
			real_send[27+i*2]=Controllers[flg][i+1];
	}

	CRC_Value=GetCRC16(real_send+11, 13+n);//8(�ɿ���ID)+1(����)+2(��ʼ��ַ)+2(�ֽ�������)=13+16(ֵ���ֽ���)=29

	real_send[24+n] = CRC_Value&0x00FF;//24+16(ֵ���ֽ���)=40
	real_send[25+n] = (CRC_Value>>8);
	real_send[26+n] = 0xDD;
	real_send[27+n] = flg; 
}

static void alter_hand_auto(u8 n,u8 flg)
{
	u16 CRC_Value;
	u8 i=0;
	
		u16 _lastaddr = real_send[20]+(((u16)real_send[21])<<8);
		_lastaddr     = _lastaddr+(n*flg);//�ϱ�ʱ��2*2*4=16��ÿ����������������Ϊ4���ֽڣ�ǰ2���ֽ�Ϊ0
		real_send[20] = _lastaddr&0xFF;
		real_send[21] = _lastaddr>>8;
		
	for(i=0;i<n/2;i=i+2)	//01 00 00 00 65 00 02 02 02 03 cc 01 02 03 04 05 06 07 64 04 00 21 50 00 ........cr cr dd
	{		
			real_send[24+i*2]  =0x00;
			real_send[25+i*2]=0x00;
			real_send[26+i*2]=hand_auto_flg[flg][i];//8(�ɿ���ID)+1(����)+2(��ʼ��ַ)+2(�ֽ�������)=13+16(�ֽ���)=29
			real_send[27+i*2]=hand_auto_flg[flg][i+1];
	}

	CRC_Value=GetCRC16(real_send+11, 13+n);//8(�ɿ���ID)+1(����)+2(��ʼ��ַ)+2(�ֽ�������)=13+16(ֵ���ֽ���)=29

	real_send[24+n] = CRC_Value&0x00FF;//24+16(ֵ���ֽ���)=40
	real_send[25+n] = (CRC_Value>>8);
	real_send[26+n] = 0xDD;
	real_send[27+n] = flg; 
}

/*�ر�˵��
ÿ���ɼ�����6���������12���ֽڣ�����4���ֽڱ�������������8���ɼ���������16���ֽ�
������8��������1�ž�Ĥ��������1�ž�Ĥ����������2�ž�Ĥ��������2�ž�Ĥ����������������
*/

static void chge_coltsnd(u8 flg)	   //75�ֽ�
{
	u16 i=0;
	u16 CRC_Value;
	
  u16 _lastaddr = real_send[20]+(((u16)real_send[21])<<8);
		_lastaddr     = _lastaddr+(0x0020*flg);//�ϱ�ʱ��2*2*8=32��ÿ���ɼ�����Ϊ4���ֽڣ�ǰ2���ֽ�Ϊ0
		real_send[20] = _lastaddr&0xFF;
		real_send[21] = _lastaddr>>8;
		
  for(i=0;i<16;i=i+2) //1���ɼ���8������16���ֽ�ת��Ϊ�ϱ���32���ֽ�
	{
		real_send[24+2*i] = 0x00;
		real_send[25+2*i] = 0x00;
		real_send[26+2*i] = Collectors[flg][i];
		real_send[27+2*i] = Collectors[flg][i+1];
	}
	
	CRC_Value=GetCRC16(real_send+11,45);//8(�ɿ���ID)+1(����)+2(��ʼ��ַ)+2(�ֽ�������)=13+24+8(�ֽ�ֵ)=45
	real_send[56]=CRC_Value&0x00FF;//24+24(ֵ���ֽ���)=48
	real_send[57]=((CRC_Value&0xFF00)>>8);
	real_send[58]=0xDD;
	real_send[59]=flg; 
}

static void handlecmd(u8 *reply_xiafa_cmd,u16 len)
	//����ƽ̨�·��Ŀ���ָ�reply_xiafa_cmd��һ��������ƽָ̨�0x01,0x00,...,0xDD��,len����Ϊ����ָ���
{	
	u16 CRC_Value=0x0000;
	
	if(reply_xiafa_cmd[len-1]!=0xDD) 		return;	
	switch(reply_xiafa_cmd[19])//ƽ̨��������0x06
	{
		case 0x06://ƽ̨�·�����ָ��
			CRC_Value = GetCRC16(reply_xiafa_cmd+11, len-14);
			if(reply_xiafa_cmd[len-3]==((CRC_Value&0x00FF)) && reply_xiafa_cmd[len-2]==((CRC_Value&0xFF00)>>8))
			{				
				set_ctrl_data(reply_xiafa_cmd+20,reply_xiafa_cmd+22,2);//���������������ݵ������������        
				memcpy(reply_xiafa_data,reply_xiafa_cmd,len );
				Start_timerEx(WG_REPLY_EVT,150);				
			}
			break;
		default:
			break;
	}	
	return;
}	

/*set_ctrl_data(ƽ̨�·�����ָ���ַ��ָ�����ݣ�2)�����磺��ַΪ2100H(00,21);����Ϊ0001H(01,00)
ÿ����������4��ͨ����4�����������趨��ÿ��ͨ��ռ4���ֽ�,��:��1���������Ŀ��������ַΪ:2100H,2104H,2108H,210CH;
��2������������Ϊ:2110H,2114H,2118H,211CH��
���������Ϊ0~31��ÿ������������ʼ��ַΪ��2100H+10H*���������
2100H~22FCHƽ̨��������
2300H-24FCHΪ4��ͨ����������������ʱ�����ã�������޴˹��ܣ�
2500H-25FCHΪ64�����Կػ�·��2600H-2620HΪ9�������Կػ�·��
*/
static void set_ctrl_data(u8 *_addr,u8 *_data,u8 len)		//len==2 set_ctrl_data(reply_xiafa_cmd+20,reply_xiafa_cmd+22,2);���������������ݵ������������       
{
	u16 PT_address,dis=0, rnd=0,rem=0;//2100H~22FCHƽ̨��������
  PT_address=_addr[0]+((u16)_addr[1]<<8);
  if(PT_address>=0x2100&&PT_address<0x2300)
	{		
		dis=((PT_address-0x2100)>>2);//dis=�Կ�������ʼ��ַ2100h��ʼ����ƫ��ֵ/4
		
		rnd=dis>>2;						//rnd=dis/2;rndΪ��������ͳһ���,0~31
		rem=(dis%4)<<1;		//rem=(dis%8)*2; /remΪrnd��ŵľ�Ĥ�������ڲ��������(0~3)*2,(0~3Ϊ��������)
		
		if(rnd<32&&rem<=8)
		{
			Controllers[rnd][rem]   = _data[0];//����ָ�����λ��������ָ����룺00 01,��������Ϊ����� 01 00 ��ߣ�
			Controllers[rnd][rem+1] = _data[1];
			crtl_cmd_num[rnd][rem/2]=50;//��վ���߷��ͼ�¼�������ظ�����3�Σ��յ���0��������������͡�
			crtl_cmd_numWX[rnd][rem/2]=50;//��վ���߷��ͼ�¼�������ظ�����3�Σ��յ���0��������������͡�		
		}
		return;
  }
 if(PT_address>=0x2300&&PT_address<0x2500)
 {
  return;
  }
 if(PT_address>=0x2500&&PT_address<0x2620)
 {
  dis=((PT_address-0x2500)>>2);//dis=���Կػ�·��ʼ��ַ2500h��ʼ����ƫ��ֵ/4
	 hand_auto_flg[dis][0]=_data[0];
	 hand_auto_flg[dis][1]=_data[1];
	 hand_auto_count[dis]=5;//���Զ�״̬�����仯��5�λظ�ƽ̨	 
  return;
  } 
}

//���ź�������
/*���������ȫΪ���֣�������*/
static bool is_number(u8 *sorce,u8 len)
{
	u8 i,temp_flase_true;
//  memcpy(test,sorce,len);		
	for(i=0;i<len;i++)
	{
		temp_flase_true=(sorce[i]!=0x2E&&sorce[i]!=0x3A&&sorce[i]!=0x2C);
		if((sorce[i] < 0x30 || sorce[i] > 0x39)&&temp_flase_true)
		{
			return false;
		}		
	}
	return true;
}

/*���ƥ�䷵����ͬ�ε����һλ�ĵ�ַ�����򷵻�0*/
static u8 match_str(u8 *dst,u8 dst_len,u8 *sor,u8 sor_len)
{
	u8 _lastSame=0;
	u8 i=0,j=0;
	bool _same=false;
	for(i=0;i<dst_len;i++)
	{
		if(dst[i]==sor[0])
		{
			_same=true;
			for(j=1;j<sor_len;j++)
			{
				if(dst[i+j]!=sor[j])
				{
					_same=false;
					j=sor_len;
				}
			}
			if(_same)
			{
				_lastSame=i+sor_len;
				i=dst_len;
			}
		}
	}
	return _lastSame;
}

//mqtt����
   void  mqtt_connect(void)   
  {
		char char_string_clientID[40];
		char char_string_username[40];
		char char_string_password[40];
		char char_string_topicName[50];
	 	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    memset(char_string_clientID,0,sizeof(char_string_clientID)); 		
		memcpy(char_string_clientID,factory_gateway_set+53,factory_gateway_set[52]); //�ͻ���ID    
		data.clientID.cstring = char_string_clientID;
		data.keepAliveInterval = 900;
		data.cleansession = 1;
	  data.willFlag=0;
		memset(char_string_username,0,sizeof(char_string_username)); 
		memcpy(char_string_username,factory_gateway_set+156,factory_gateway_set[155]); //�û���	
  	data.username.cstring =char_string_username;
    if(factory_gateway_set[188]!=0)			
    {
		memset(char_string_password,0,sizeof(char_string_password)); 
		memcpy(char_string_password,factory_gateway_set+189,factory_gateway_set[188]);	//ʹ������
		data.password.cstring = char_string_password; //���� 
		}
		if(factory_gateway_set[1]==4)
		{
//    	memset(char_string_username,0,sizeof(char_string_username));// ����
//			memcpy(char_string_username,"testCode1-DTU-20170925-000068",29);	//ʹ���û���������		
//		  data.username.cstring =char_string_username;//����
		memset(char_string_password,0,sizeof(char_string_password)); 
		memcpy(char_string_password,md5_password_ascii,32);	//ʹ������
		data.password.cstring = char_string_password; //����	
	  data.willFlag=1;			
		memset(char_string_topicName,0,sizeof(char_string_topicName));
//			memcpy(char_string_topicName,"/testCode1-DTU-20170925-000068/lastwill",39);	//ʹ��topicName;�û���������
		memcpy(char_string_topicName,"/",1);	//ʹ��topicName	
    memcpy(char_string_topicName+1,factory_gateway_set+156,factory_gateway_set[155]);	//ʹ��topicName���û���			
		memcpy(char_string_topicName+1+factory_gateway_set[155],"/lastwill",9);	//ʹ��topicName="/${deviceId}/lastwill"
		data.will.topicName.cstring=char_string_topicName;
//		memset(char_string,0,sizeof(char_string)); 
//		memcpy(char_string,"\"devid\":\"111123132dsa\"",22);	//ʹ��will.message
//		data.will.message.cstring=char_string;
		}
		mqtt_len = MQTTSerialize_connect(ReportData2, sizeof(ReportData2), &data);          
 }
	
	void mqtt_subscribe(void)
{       // subscribe                ���ý���״̬  --�������� 
	  char char_string[40];
		MQTTString topicString = MQTTString_initializer;
		volatile int len;
		int msgid = 1;
		int req_qos = 0;
	  memset(char_string,0,sizeof(char_string)); 
    memcpy(char_string,factory_gateway_set+84,factory_gateway_set[83]); //factory_gateway_set[83]Ϊ�������ⳤ�ȣ�[84]~[113]���	30���ַ�	
	if(factory_gateway_set[1]==4)
	{
	  memset(char_string,0,sizeof(char_string));
//		memcpy(char_string,"/testCode1-DTU-20170925-000068/control",38);	//�������⣻����
		memcpy(char_string,"/",1);	//ʹ��topicName	
    memcpy(char_string+1,factory_gateway_set+156,factory_gateway_set[155]);	//ʹ��topicName���û���			
		memcpy(char_string+1+factory_gateway_set[155],"/control",8);	//ʹ��topicName="/${deviceId}/lastwill"
	}
		topicString.cstring = char_string;
		mqtt_len = MQTTSerialize_subscribe(ReportData2, sizeof(ReportData2), 0, msgid, 1, &topicString, &req_qos);	
}

void mqtt_publish(unsigned char *real,int pub_len)//��������
{   	  
	//volatile int len;
	  char char_string[40];
    MQTTString topicString = MQTTString_initializer;
	  memset(char_string,0,sizeof(char_string)); 
    memcpy(char_string,factory_gateway_set+125,factory_gateway_set[124]); //factory_gateway_set[124]Ϊ�������ⳤ�ȣ�[125]~[154]���	30���ַ�
	
   if(factory_gateway_set[1]==4)
		{		
		  memset(char_string,0,sizeof(char_string));
//			memcpy(char_string,"/testCode1-DTU-20170925-000068/status",37);	//ʹ��topicName;�û���������
			memcpy(char_string,"/",1);	//ʹ��topicName	
      memcpy(char_string+1,factory_gateway_set+156,factory_gateway_set[155]);	//ʹ��topicName���û���			
		  memcpy(char_string+1+factory_gateway_set[155],"/status",7);	//ʹ��topicName="/${deviceId}/lastwill"
		}	
    topicString.cstring = char_string;
	mqtt_len=MQTTSerialize_publish(ReportData2, sizeof(ReportData2), 0, 0, 0, 0, topicString, (unsigned char*)real,pub_len);	 	
 }

/*�����������⺯����ΪSDKЭ�����Ϳ�����״̬ʹ��*/
void mqtt_publish1(unsigned char *real,int pub_len)//��������1
	{ 	
    MQTTString topicString = MQTTString_initializer;
    topicString.cstring = SDK_topicString_pub1;
	mqtt_len=MQTTSerialize_publish(ReportData2, sizeof(ReportData2), 0, 0, 0, 0, topicString, (unsigned char*)real,pub_len);	 	
 }

//SDK����
 /*�ú�����Ҫ����ƽ̨��2�ֽ���ʼ��ַ(keyX_addr)��2�ֽ���������(value)����������nת��Ϊ��key1:value1��key2:value2��...
    keyn:valuen ��ASC��,����װ��{key1:value1��key2:value2��... keyn:valuen}�͵�key_data_send������*/
 static u8 made_keyX_value(u16 keyX_addr,u8 *value,u8 n,u8 *key_data_send)
 {
	 u8 i,keyX_asc[5],value_asc[4];//u8 8λ�޷�����
   s8 j;            //s8 8λ�з�����
	 u16 temp_addr,temp_value;
	 
	 for(i=0;i<n;i++)
	 {
		 temp_addr= keyX_addr+i*4;
     temp_value=(value[2*i+1]<<8)|value[2*i];	 
		for(j=4;j>=1;j--)
		 {		  
	    keyX_asc[j]=temp_addr%16;//��2�ֽ�16λ�ĵ�ַת����ASC�룻��keyֵ
       if(keyX_asc[j]<=9)
			 {
			   keyX_asc[j]=keyX_asc[j]+'0';
			 }
       else
       {
			   keyX_asc[j]=keyX_asc[j]+0x37;
			 }
			keyX_asc[0]='H';
			 
      value_asc[j-1]=temp_value%16;//��2�ֽ�16λ������ת����ASC�룻���ϱ�ƽ̨������
       if(value_asc[j-1]<=9)
			 {
			   value_asc[j-1]=value_asc[j-1]+'0';
			 }
       else
       {
			   value_asc[j-1]=value_asc[j-1]+0x37;
			 }
			 
		 temp_addr=temp_addr/16;
		 temp_value=temp_value/16;
		 }
		 memcpy(key_data_send+i*(6+5)+1,keyX_asc,5);//H+4�ֽ�+��:��;��6�ֽ�
		 key_data_send[i*(6+5)+1+5]=':'; 
    memcpy(key_data_send+i*(6+5)+7,value_asc,4);//4�ֽ�+������;��5�ֽ�
     key_data_send[i*(6+5)+7+4]=',';		 
	 }
	 key_data_send[0]='{';
   key_data_send[n*(6+5)]='}';
   return	 n*(6+5)+1;//���һ���ֽ� ',' ��  '}'  �滻�ˣ�����ֻ��1��ÿ��������Ҫ11���ֽ�
 }
 
/*�ú�����Ҫ����ƽ̨��2�ֽ���ʼ��ַ(keyX_addr)��2�ֽ���������(value)����������nת��Ϊ��{"key1":value1}��{"key2":value2}��...
    {"keyn":valuen} ��ASC��,����װ��[{"key1":value1}��{"key2":value2}��... {"keyn":valuen}]�͵�key_data_send������*/
 static u8 made_keyX_value4(u16 keyX_addr,u8 *value,u8 n,u8 *key_data_send)
 {
	 u8 i,keyX_asc[4],value_asc[4];//u8 8λ�޷�����
   s8 j;            //s8 8λ�з�����
	 u16 temp_addr,temp_value;
	 
	 for(i=0;i<n;i++)
	 {
		 temp_addr= keyX_addr+i*4;
     temp_value=(value[2*i+1]<<8)|value[2*i];	 
		for(j=3;j>=0;j--)
		 {		  
	    keyX_asc[j]=temp_addr%16;//��2�ֽ�16λ�ĵ�ַת����ASC�룻��keyֵ
       if(keyX_asc[j]<=9)
			 {
			   keyX_asc[j]=keyX_asc[j]+'0';
			 }
       else
       {
			   keyX_asc[j]=keyX_asc[j]+0x37;
			 }
			 
      value_asc[j]=temp_value%16;//��2�ֽ�16λ������ת����ASC�룻���ϱ�ƽ̨������
       if(value_asc[j]<=9)
			 {
			   value_asc[j]=value_asc[j]+'0';
			 }
       else
       {
			   value_asc[j]=value_asc[j]+0x37;
			 }
			 
		 temp_addr=temp_addr/16;
		 temp_value=temp_value/16;
		 }
		 key_data_send[i*14+1+0]='{';
		 key_data_send[i*14+1+1]='"';
	  memcpy(key_data_send+i*14+1+2,keyX_asc,4);//{"4�ֽ�":4�ֽ�},  ��14�ֽ�(����,)
		 key_data_send[i*14+1+6]='"';
     key_data_send[i*14+1+7]=':';	 
    memcpy(key_data_send+i*14+1+8,value_asc,4);//{"4�ֽ�":4�ֽ�},  ��14�ֽ�(����,)
     key_data_send[i*14+1+8+4]='}';
     key_data_send[i*14+1+8+5]=',';		 
	 }
	 key_data_send[0]='[';
   key_data_send[n*14]=']';
   return	 n*14+1;//���һ���ֽ� ',' ��  '}'  �滻�ˣ�����ֻ��1��ÿ��������Ҫ14���ֽ�
 }
  
 
 /*�ú�����Ҫ����ƽ̨��2�ֽ���ʼ��ַ(keyX_addr)��2�ֽڵ�����int������(valueN)ת����4�ֽ���������(valueF)����������nת��Ϊ��key1:valueF1��key2:valueF2��...
    keyn:valueFn ��ASC��,����װ��{key1:valueF1��key2:valueF2��... keyn:valueFn}�͵�key_data_send������;valueFN��ֵΪXXXX.XX*/
 static u8 made_keyX_valueF(u16 keyX_addr,u8 *value,float *valueF,u8 n,u8 *key_data_send)
 {
	 u8 i,keyX_asc[5],value_asc[10];//u8 8λ�޷�����
	 u8 valueF_asc_len=0,valueF_asc_len1=0;
   s8 j;                                 //s8 8λ�з�����
	 float unit_valueF[8];
	 u16 temp_addr,temp_value;
	 
	 for(i=n;i<n+4;i++)
	 {
		 temp_addr= keyX_addr+i*4;
		 
     temp_value=(value[2*i+1]<<8)|value[2*i];
		 if(valueF[2*i]<=0.1&&valueF[2*i]>=-0.1)
		 {
			 valueF[2*i]=655.35;
			 valueF[2*i+1]=0;
		 }
		 if(valueF[2*i+1]>99999.99||valueF[2*i+1]<-99999.99)
		 {
			 valueF[2*i]=655.35;
			 valueF[2*i+1]=0;
		 }
     unit_valueF[i]=temp_value/valueF[2*i]+valueF[2*i+1];//float_to_string(float data, u8 *str)
		 valueF_asc_len=float_to_string(unit_valueF[i],value_asc);
		 
		for(j=4;j>=1;j--)
		 {		  
	    keyX_asc[j]=temp_addr%16;//��2�ֽ�16λ�ĵ�ַת����ASC�룻��keyֵ
       if(keyX_asc[j]<=9)
			 {
			   keyX_asc[j]=keyX_asc[j]+'0';
			 }
       else
       {
			   keyX_asc[j]=keyX_asc[j]+0x37;
			 }			 
		 temp_addr=temp_addr/16;		 
		 }
		 keyX_asc[0]='H';
		 memcpy(key_data_send+(i-n)*6+valueF_asc_len1+1,keyX_asc,5);//H+4�ֽ�+��:��;��6�ֽ�
		 key_data_send[(i-n)*6+valueF_asc_len1+1+5]=':'; 
    memcpy(key_data_send+(i-n)*6+valueF_asc_len1+6+1,value_asc,valueF_asc_len);//5�ֽ�+������;��6�ֽ�
     
		 key_data_send[(i-n)*6+7+valueF_asc_len1+valueF_asc_len]=',';
     valueF_asc_len1=valueF_asc_len1+valueF_asc_len+1;		 
	 }
	 key_data_send[0]='{';
   key_data_send[4*6+valueF_asc_len1]='}';
   return	 4*6+valueF_asc_len1+1;//���һ���ֽ� ',' ��  '}'  �滻�ˣ�����ֻ��1��ÿ��������Ҫ10���ֽ�
 } 
 
 /*�ú�����Ҫ����ƽ̨��2�ֽ���ʼ��ַ(keyX_addr)��2�ֽڵ�����int������(valueN)ת����4�ֽ���������(valueF)����������nת��Ϊ��{"key1":valueF1}��{"key2":valueF2}��...
    {"keyn":valueFn" ��ASC��,����װ��{{"key1":valueF1}��{"key2":valueF2}��...{"keyn":valueFn}}�͵�key_data_send������;valueFN��ֵΪXXXX.XX*/
 static u8 made_keyX_valueF4(u16 keyX_addr,u8 *value,float *valueF,u8 n,u8 *key_data_send)
 {
	 u8 i,keyX_asc[4],value_asc[10];//u8 8λ�޷�����
	 u8 valueF_asc_len=0,valueF_asc_len1=0;
   s8 j;                                 //s8 8λ�з�����
	 float unit_valueF[8];
	 u16 temp_addr,temp_value;
	 
	 for(i=n;i<n+4;i++)
	 {
		 temp_addr= keyX_addr+i*4;
		 
     temp_value=(value[2*i+1]<<8)|value[2*i];
		 if(valueF[2*i]<=0.1&&valueF[2*i]>=-0.1)
		 {
			 valueF[2*i]=655.35;
			 valueF[2*i+1]=0;
		 }
		 if(valueF[2*i+1]>99999.99||valueF[2*i+1]<-99999.99)
		 {
			 valueF[2*i]=655.35;
			 valueF[2*i+1]=0;
		 }
     unit_valueF[i]=temp_value/valueF[2*i]+valueF[2*i+1];//float_to_string(float data, u8 *str)
		 valueF_asc_len=float_to_string(unit_valueF[i],value_asc);
		 
		for(j=3;j>=0;j--)
		 {		  
	    keyX_asc[j]=temp_addr%16;//��2�ֽ�16λ�ĵ�ַת����ASC�룻��keyֵ
       if(keyX_asc[j]<=9)
			 {
			   keyX_asc[j]=keyX_asc[j]+'0';
			 }
       else
       {
			   keyX_asc[j]=keyX_asc[j]+0x37;
			 }			 
		 temp_addr=temp_addr/16;		 
		 }		 
		 memcpy(key_data_send+(i-n)*8+valueF_asc_len1+1+2,keyX_asc,4);//{"4�ֽ�":��8�ֽ�;˵���� {"2100":
		 key_data_send[(i-n)*8+valueF_asc_len1+1]='{';
		 key_data_send[(i-n)*8+valueF_asc_len1+1+1]='"';
		 key_data_send[(i-n)*8+valueF_asc_len1+1+6]='"';
		 key_data_send[(i-n)*8+valueF_asc_len1+1+7]=':'; 
     memcpy(key_data_send+(i-n)*8+valueF_asc_len1+1+8,value_asc,valueF_asc_len);//4�ֽ�+������;��5�ֽ�
     key_data_send[(i-n)*8+9+valueF_asc_len1+valueF_asc_len]='}';
		 key_data_send[(i-n)*8+9+valueF_asc_len1+valueF_asc_len+1]=',';
     valueF_asc_len1=valueF_asc_len1+valueF_asc_len+2;		 
	 }
	 key_data_send[0]='[';
   key_data_send[4*8+valueF_asc_len1]=']';
   return	 4*8+valueF_asc_len1+1;//���һ���ֽ� ',' ��  '}'  �滻�ˣ�����ֻ��1��ÿ��������Ҫ10���ֽ�
 } 
  
 
//��������
 static void bytelen_to_asc(u8 *zfc,u8 len) //��1���ֽ�ʮ��������ת��Ϊʮ������ֵ��asc��
 {
	 s8 i;	
	 for(i=2;i>=0;i--)
	 {
	  zfc[i]=len%10+'0';
    len=len/10;		 
	 }	
 }
 
static void byte_to_asc(u8 byte_hex,u8 *byte_asc)//��1���ֽ�ʮ��������ת��Ϊ��Ӧasc��
{
	u8 temp_hex=0;
	temp_hex=(byte_hex&0xF0)>>4;	
  if(temp_hex<=9)
	 {
		 *byte_asc=temp_hex+'0';
	 }
	 else
	 {
		 *byte_asc=temp_hex+0x57;
	 }
  temp_hex=(byte_hex&0x0F);	
  if(temp_hex<=9)
	 {
		 *(byte_asc+1)=temp_hex+'0';
	 }
	 else
	 {
		 *(byte_asc+1)=temp_hex+0x57;
	 }	 
}
	
static u16 dword_asc_hex(u8 *dword_asc)//��4���ֽڵ�ASC��("210C")ת����һ��16λʮ��������0x210C	
{
	u8 i;
	u16 len=0;
	for(i=0;i<=3;i++)
	{ 
	 if(dword_asc[i]<=0x39)
	 {
		 len=len*16+(dword_asc[i]-0x30);
	 }
   else if(dword_asc[i]>=0x41&&dword_asc[i]<=0x46)
   {
	   len=len*16+(dword_asc[i]-0x37);
	 }
   else
   {
	   len=len*16+(dword_asc[i]-0x57);
	 }		 
	}
	return len;
}
 
 static void cycle_cmd(void)//���Ը����г����¼��Ƿ�����
 {
	 //��ʼ���������ʼ���¼��Ƿ���������
 if(halt_module<0x10&&cmd_flg!=0xFF) //���4Gͨ��ģ�鹤���Ƿ�������������������REST4Gͨ��ģ��
	{      
   halt_module++;//GSMģ���ʼ����ɲŽ��м��;		if(cmd_flg==0xFF)ȡ��
	 Start_timerEx(CYCLE_CMD_EVT,10000);//�����¼����������Ƿ�������
	}
	else if(halt_module>=0x10&&cmd_flg!=0xFF) 
	{
      while(1); 		
//    halt_module=0x00;		
//	  cmd_flg   = 0x01;//CDMA����REST
//    send_flg  = 0x01;
//	  Start_timerEx(NET_INIT_EVT,500);
//		Start_timerEx(CYCLE_CMD_EVT,65000);		
	}
  else
	{	
   	Start_timerEx(CYCLE_CMD_EVT,10000);//�����¼����������Ƿ�������	
		return;
	} 
 //�������������ʼ���¼��Ƿ���������
 }
 static void set_slave_param(void)//���������趨��վ����
 {
	 u8 i;
	 if(slave_set_flg==1)//�����������趨
   {
	  while((ctrlslave_param_flg[set_slave_Index]<=0)&&(set_slave_Index<4))//CSH_Wired_finish==0|
              {
							  set_slave_Index++;
							}
					if(set_slave_Index<4)
					{
						YX_Slave_ID=set_slave_Index+first_xiabiao_I+33;           						
						bytelen3=WriteMultipleRegister(YX_Slave_ID,4,12,ctrlslave_param_set[set_slave_Index],ReportData3);//MODBUSд����Ĵ���,�������鳤��
						memcpy(USART3SendTCB,ReportData3,bytelen3);
						WriteDataToDMA_BufferTX3(bytelen3);
						if(ctrlslave_param_flg[set_slave_Index]>0) ctrlslave_param_flg[set_slave_Index]--;//��վ���ͼ�¼��ÿ����һ�μ�1���յ��ظ���0�������ظ�����3��
						set_slave_Index++;
					}
      			if(set_slave_Index>=4)
  				{  
    				set_slave_Index=0;
    						
  				}
					slave_set_finish=0;
      		for(i=0;i<4;i++){slave_set_finish=slave_set_finish|ctrlslave_param_flg[i];}
          if(slave_set_finish!=0)
					{
	  			Start_timerEx(SET_SLAVEPARAM_EVT,280); 
					}
					else
					{
						set_slave_Index=0;
						slave_set_flg=0;
						set_finish_flg=0;						
						Stop_timerEx(SET_SLAVEPARAM_EVT);
            Start_timerEx(WG_SENDZZ_EVT,200);						
					}
	 }
   if(slave_set_flg==2)//�ɼ��������趨
   {
	  while((cjqslave_param_flg[set_slave_Index]<=0)&&(set_slave_Index<8))//CSH_Wired_finish==0|
              {
							  set_slave_Index++;
							}
					if(set_slave_Index<8)
					{ 
						YX_Slave_ID=set_slave_Index+first_xiabiao_I+1;						//������վID
						bytelen3=WriteMultipleRegister(YX_Slave_ID,16,18,cjqslave_param_set[set_slave_Index],ReportData3);
						memcpy(USART3SendTCB,ReportData3,bytelen3);
						WriteDataToDMA_BufferTX3(bytelen3);
						if(cjqslave_param_flg[set_slave_Index]>0) cjqslave_param_flg[set_slave_Index]--;//��վ���ͼ�¼��ÿ����һ�μ�1���յ��ظ���0�������ظ�����3��
						set_slave_Index++;
					}
      			if(set_slave_Index>=8)
  				{  
    				set_slave_Index=0;    						
  				}
					slave_set_finish=0;
      		for(i=0;i<8;i++){slave_set_finish=slave_set_finish||cjqslave_param_flg[i];}
          if(slave_set_finish!=0)
					{
	  			Start_timerEx(SET_SLAVEPARAM_EVT,280); 
					}
					else
					{
						set_slave_Index=0;
						slave_set_flg=0;
						set_finish_flg=0;						
						Stop_timerEx(SET_SLAVEPARAM_EVT);
						Start_timerEx(WG_SENDZZ_EVT,200);
					}
	 }
	 if(slave_set_flg==3)//�趨��վ��ַ���ŵ�
   {
		 bytelen3=WriteMultipleRegister(0xF0,34,8,set_slaveID_channel,ReportData3);
		 memcpy(USART3SendTCB,ReportData3,bytelen3);
		 WriteDataToDMA_BufferTX3(bytelen3);		 
     if(slaveID_channel_flg>0) {slaveID_channel_flg--;}//��վ���ͼ�¼��ÿ����һ�μ�1���յ��ظ���0�������ظ�����5��
     if(slaveID_channel_flg!=0)
			{
			Start_timerEx(SET_SLAVEPARAM_EVT,280); 
			}
			else
			{				
				slave_set_flg=0;
				set_finish_flg=0;
				if(set_slaveID_channel[0]==65&&set_slaveID_channel[2]==255 )
				{
				  SI4463_Channel=set_slaveID_channel[3];
					Flash_Write(0x0807D000, set_slaveID_channel+3,1) ;
					if(close_433MHZ!=0)
					{
					 SI4463_Init();
					 Stop_timerEx(SET_SLAVEPARAM_EVT);
					 Start_timerEx(WX_SENDZZ_EVT,3000);					 
					}//��������������Ӧ�������
				}
				Stop_timerEx(SET_SLAVEPARAM_EVT);
				Start_timerEx(WG_SENDZZ_EVT,200);
			}	 
	 }
 }
 
 //433MHZ���ߺ���
 
// static void	Get_WX_Channel(void)
//{
//	if(!GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2))
//		SI4463_Channel += 1;
//	if(!GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3))
//		SI4463_Channel += 2;	
//	if(!GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4))
//		SI4463_Channel += 4;	
//	if(!GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_5))
//		SI4463_Channel += 8;	
//	if(!GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_6))
//		SI4463_Channel += 16;		
//}

static void Clear_Buf(u8 buf[], u8 length, u8 data)
{
	u8 i = 0;
	for(i = 0; i < length; i++)
		buf[i] = data;	
}

static void SI4463Receive(u8 len,u8 *pData) //����վͨ�ţ�������վ�ظ�
{

	if(GetCRC16(pData,len)==0)
	{
		 online_slaveID_WX[pData[0]-1]=pData[0];//��ʾ����վ���ߣ���¼����
		 ZZ_Wireles_flag[pData[0]-1]=0x01;//�յ���վ����ͨ�ţ����ñ�־��0x01��ʾ���ڣ�0��ʾ����վ������	 
		 init_cmd_numWX[pData[0]-1]=0;
//		 Start_timerEx(WX_SENDZZ_EVT,50);
		 if(pData[1]==0x03)
		{	    			    
			memcpy(Collectors[pData[0]-1],pData+3,pData[2]);
			Start_timerEx(WX_SENDZZ_EVT,250);
			return;			
		}
    if(pData[1]==0x06)
		{
			if((pData[2]==0x00)&&(pData[3]<=0x03)&&(pData[0]>=33)&&(pData[0]<=64))
					//slave_set_flg��Send_slave_cmd��RxReport5�������趨��slave_set_flg==0����������վ�ظ�
				{					
					crtl_cmd_numWX[pData[0]-33][pData[3]]=0;//��ʾ��վ�յ�����ָ������ٷ���д����
					Start_timerEx(WX_SENDZZ_EVT,250);
					return;	
				 }		
    }			
		
		if(pData[1]==0x10)
		{	     		
        if((pData[0]>=33)&&(slave_set_flgWX==1)&&(pData[3]>=4)&&(pData[3]<=15))//slave_set_flgWX==1�����趨����������վ���߻ظ�
				{
				  ctrlslave_param_flgWX[pData[0]-firstwx_xiabiao_i-32]=0;
					Start_timerEx(WX_SET_SLAVEPARAM_EVT,500);
					return;	
				}
				if((pData[3]>=16)&&(pData[3]<=33)&&(slave_set_flgWX==2))//slave_set_flgWX==2�����趨�����ɼ����Ϳ�������վ���߻ظ�
				{
				  cjqslave_param_flgWX[pData[0]-firstwx_xiabiao_i-1]=0;
					Start_timerEx(WX_SET_SLAVEPARAM_EVT,500);
					return;	
				}
				if((pData[3]>=34)&&(pData[3]<=35)&&(slave_set_flgWX==3))//slave_set_flgWX==2�����趨�����ɼ����Ϳ�������վ���߻ظ�
				{
				  slaveID_channel_flgWX=0;
					Start_timerEx(WX_SET_SLAVEPARAM_EVT,500);
					return;	
				}
			}
	}
 else if(slave_set_flgWX!=0) {Start_timerEx(WX_SET_SLAVEPARAM_EVT,500);}		
 else {Start_timerEx(WX_SENDZZ_EVT,500);}
}

static void SI4463_SENDZZ(void)
{		  
  		switch(WX_Query_Flag) //WX_Query_Flag
  		{
    		case CONTROLLERS_CMD:
 //��ʼ��δ��� �� ������վ����ɹ��յ��ظ����跢�ͣ�ͬʱ��������վIDС�ڵ���32��������ķ�����վ��������;
          while((crtl_cmd_numWX[Query_Index_ControllerWX][ctrl_wx_j]<=0)&&(Query_Index_ControllerWX<32))
              {
								ctrl_wx_j++;								
								if(ctrl_wx_j>=4)
								{								 
							   Query_Index_ControllerWX++;
								 ctrl_wx_j=0;
								}							  
							}
					if(Query_Index_ControllerWX<32&&ctrl_wx_j<=3)
					{
           						
    			  WX_len=WriteSingleRegister(Query_Index_ControllerWX+33,ctrl_wx_j,Controllers[Query_Index_ControllerWX]+ctrl_wx_j*2,SI4463_TxBUFF);
					  SI4463_SEND_PACKET(SI4463_TxBUFF,WX_len, SI4463_Channel, 0);
						if(crtl_cmd_numWX[Query_Index_ControllerWX][ctrl_wx_j]>0)crtl_cmd_numWX[Query_Index_ControllerWX][ctrl_wx_j]--;//��վ���ͼ�¼��ÿ����һ�μ�1���յ��ظ���0�������ظ�����3��
        		ctrl_wx_j++;								
						if(ctrl_wx_j>=4)
						{								 
						 Query_Index_ControllerWX++;
						 ctrl_wx_j=0;
						}						
					}
      			if(Query_Index_ControllerWX>=32)
  				{  
    				Query_Index_ControllerWX=0;
    				WX_Query_Flag=COLLECTORS_CMD;						
  				}      			
	  			Start_timerEx(WX_SENDZZ_EVT,250);					
      			break;
      
    		case COLLECTORS_CMD:
					while((online_slaveID_WX[Query_Index_CollectorWX]==0)&&(Query_Index_CollectorWX<64))
              {											 
							   Query_Index_CollectorWX++;											  
							 }
					if(Query_Index_CollectorWX<64)
					{
						WX_len=ReadData(Query_Index_CollectorWX+1,READ_HOLDING_REGISTER,0x0000,0x0008,SI4463_TxBUFF);
						SI4463_SEND_PACKET(SI4463_TxBUFF,WX_len, SI4463_Channel, 0);   
						Query_Index_CollectorWX++;
				   }
      	  if(Query_Index_CollectorWX >63)//�ɼ�����ѯ��ϣ���ʼ��ѯ������
				  {
        			Query_Index_CollectorWX=0;
						  WX_Query_Flag=ZZ_QUERY_COLLECTOR;     		
							Start_timerEx(WX_SENDZZ_EVT,350);
						  break;					    

				   }
          else
					{						
							WX_Query_Flag=CONTROLLERS_CMD;     		
							Start_timerEx(WX_SENDZZ_EVT,350);
						break;
           }
		   case ZZ_QUERY_COLLECTOR://�ɼ�����ѯ
				 while((online_slaveID_WX[Query_IndexZZ_C_WX]!=0)&&(Query_IndexZZ_C_WX<32))
              {											 
							   Query_IndexZZ_C_WX++;											  
							 }
					if(Query_IndexZZ_C_WX<32)
					{
						WX_len=ReadData(Query_IndexZZ_C_WX+1,READ_HOLDING_REGISTER,0x0000,0x0008,SI4463_TxBUFF);
						SI4463_SEND_PACKET(SI4463_TxBUFF,WX_len, SI4463_Channel, 0);   
						if(init_cmd_numWX[Query_IndexZZ_C_WX]>0){init_cmd_numWX[Query_IndexZZ_C_WX]--;}
						else {Query_IndexZZ_C_WX++;}
				   }
      	  if(Query_IndexZZ_C_WX >31){Query_IndexZZ_C_WX=0;}//�ɼ�����ѯ��ϣ���ʼ��ѯ������				  
						  WX_Query_Flag=ZZ_QUERY_CONTROLLER;     		
							Start_timerEx(WX_SENDZZ_EVT,350);
              break;

case ZZ_QUERY_CONTROLLER://��������ѯ
				 while((online_slaveID_WX[Query_IndexZZ_K_WX+32]!=0)&&(Query_IndexZZ_K_WX<32))
              {											 
							   Query_IndexZZ_K_WX++;											  
							 }
					if(Query_IndexZZ_K_WX<32)
					{
						WX_len=ReadData(Query_IndexZZ_K_WX+33,READ_HOLDING_REGISTER,0x0000,0x0008,SI4463_TxBUFF);
						SI4463_SEND_PACKET(SI4463_TxBUFF,WX_len, SI4463_Channel, 0);   
						if(init_cmd_numWX[Query_IndexZZ_K_WX+32]>0){init_cmd_numWX[Query_IndexZZ_K_WX+32]--;}
						else {Query_IndexZZ_K_WX++;}
				   }
      	  if(Query_IndexZZ_K_WX >31)//�ɼ�����ѯ��ϣ���ʼ��ѯ������
				   {
        			Query_IndexZZ_K_WX=0;
						 
						if(ZZ_Wireles_flag[64]==0)
						 {
							 u8 i;
							 CSH_countWX=0;
							 for (i=0;i<64;i++)
								{
								 CSH_countWX=CSH_countWX|init_cmd_numWX[i];
								}
							 if((CSH_countYX==0)&&(CSH_countWX==0)){ZZ_Wireles_flag[64]=1;}	
						 }
						 
							if(Query_Wired_WirelesWX>0){ZZ_Wireles_flag[Query_Wired_WirelesWX-1]=ZZ_temp_stateWX;}
              else {ZZ_Wireles_flag[63]=ZZ_temp_stateWX;}			
							ZZ_temp_stateWX=ZZ_Wireles_flag[Query_Wired_WirelesWX];						  
							ZZ_Wireles_flag[Query_Wired_WirelesWX]=0x02;//��վ����ͨ�ű�־=2,��������ʾ�Ƶ�,���ڼ��
//							online_slaveID_WX[Query_Wired_WirelesWX]=0x00;//���ڼ��							
              if(ZZ_temp_stateWX==0x02){ZZ_temp_stateWX=0;}		
							Query_Wired_WirelesWX++;
              if(Query_Wired_WirelesWX>63){ Query_Wired_WirelesWX=0;}							
					  }
						  WX_Query_Flag=CONTROLLERS_CMD;     		
							Start_timerEx(WX_SENDZZ_EVT,350);
              break;						
    		default:
					Start_timerEx(WX_SENDZZ_EVT,500);
      		break;
  		}
}

 static void set_slave_paramWX(void)//���������趨��վ����
 {
	 u8 i;
	 if(slave_set_flgWX==1)
   {
	  while((ctrlslave_param_flgWX[set_slave_IndexWX]<=0)&&(set_slave_IndexWX<4))//CSH_Wireles_finish==0|
              {
							  set_slave_IndexWX++;
							}
					if(set_slave_IndexWX<4)
					{ 
          WX_Slave_ID=set_slave_IndexWX+firstwx_xiabiao_i+33;						
    			WX_len=WriteMultipleRegister(WX_Slave_ID,4,12,ctrlslave_param_set[set_slave_IndexWX],SI4463_TxBUFF);
				  SI4463_SEND_PACKET(SI4463_TxBUFF,WX_len, SI4463_Channel, 0);
					if(ctrlslave_param_flgWX[set_slave_IndexWX]>0) {ctrlslave_param_flgWX[set_slave_IndexWX]--;}//��վ���ͼ�¼��ÿ����һ�μ�1���յ��ظ���0�������ظ�����3��
        	set_slave_IndexWX++;
					}
      			if(set_slave_IndexWX>=4)
  				{  
    				set_slave_IndexWX=0;
    						
  				}
					slave_set_finishWX=0;
      		for(i=0;i<4;i++){slave_set_finishWX=slave_set_finishWX|ctrlslave_param_flgWX[i];}
          if(slave_set_finishWX!=0)
					{
	  			 Start_timerEx(WX_SET_SLAVEPARAM_EVT,280); 
					}
					else
					{
						set_slave_IndexWX=0;
						slave_set_flgWX=0;
						set_finish_flg=0;
						Stop_timerEx(WX_SET_SLAVEPARAM_EVT);
            Start_timerEx(WX_SENDZZ_EVT,350);						
					}
	 }
   if(slave_set_flgWX==2)
   {
	  while((cjqslave_param_flgWX[set_slave_IndexWX]<=0)&&(set_slave_IndexWX<8))//CSH_Wireles_finish==0|
              {
							  set_slave_IndexWX++;
							}
					if(set_slave_IndexWX<8)
					{
          WX_Slave_ID=set_slave_IndexWX+firstwx_xiabiao_i+1;						
          WX_len=WriteMultipleRegister(WX_Slave_ID,16,18,cjqslave_param_set[set_slave_IndexWX],SI4463_TxBUFF);
				  SI4463_SEND_PACKET(SI4463_TxBUFF,WX_len, SI4463_Channel, 0);    			
					if(cjqslave_param_flgWX[set_slave_IndexWX]>0) {cjqslave_param_flgWX[set_slave_IndexWX]--;}//��վ���ͼ�¼��ÿ����һ�μ�1���յ��ظ���0�������ظ�����3��
        	set_slave_IndexWX++;
					}
      			if(set_slave_IndexWX>=8)
  				{  
    				set_slave_IndexWX=0;
    						
  				}
					slave_set_finishWX=0;
      		for(i=0;i<8;i++){slave_set_finishWX=slave_set_finishWX|cjqslave_param_flgWX[i];}
          if(slave_set_finishWX!=0)
					{
	  			Start_timerEx(WX_SET_SLAVEPARAM_EVT,500); 
					}
					else
					{
						set_slave_IndexWX=0;
						slave_set_flgWX=0;
						set_finish_flg=0;
						Stop_timerEx(WX_SET_SLAVEPARAM_EVT);
						Start_timerEx(WX_SENDZZ_EVT,350);	
					}
	 }
	 if(slave_set_flgWX==3)
   {
		 WX_len=WriteMultipleRegister(0xF0,34,8,set_slaveID_channel,SI4463_TxBUFF);//��վID,��վ��Ӧ�ı����ַ������������������
		 SI4463_SEND_PACKET(SI4463_TxBUFF,WX_len, SI4463_Channel, 0);
     if(slaveID_channel_flgWX>0) {slaveID_channel_flgWX--;}//��վ���ͼ�¼��ÿ����һ�μ�1���յ��ظ���0�������ظ�����5��
     if(slaveID_channel_flgWX!=0)
			{
			Start_timerEx(WX_SET_SLAVEPARAM_EVT,500); 
			}
			else
			{				
				slave_set_flgWX=0;
				set_finish_flg=0;
				if(set_slaveID_channel[0]==65&&set_slaveID_channel[2]==255 )
				{
				  SI4463_Channel=set_slaveID_channel[3];
					Flash_Write(0x0807D000, set_slaveID_channel+3,1) ;
					if(close_433MHZ!=0)
					{
					 SI4463_Init();
					 Stop_timerEx(WX_SET_SLAVEPARAM_EVT);
					 Start_timerEx(WX_SENDZZ_EVT,3000);
					 return;
					}//��������������Ӧ�������
				}
				Stop_timerEx(WX_SET_SLAVEPARAM_EVT);
				Start_timerEx(WX_SENDZZ_EVT,350);	
			}	 
	 }
 }
 //����Ϊ�������ɼ�����
 static void wgcollector_data(void) //�������ɼ�����
{
	//	u16 temp;	
	switch(ReadDataCNT) 
	{		
		case 0:
		 TD_param_num=0;//����ÿ��ͨ���ϱ�������˳�������ͨ����������������ͨ�����Ĳ����Զ���ǰ��
		 if(factory_gateway_set[12]==1&&factory_gateway_set[15]!=18)//���ն�;factory_gateway_set[15]=18 Ϊ������Һλ�����ȼ�⣬������ʹ��SCK
		 {
			 collector_temp =  Get_Illuminance();//���նȣ�PC0
			 if(collector_temp != 0xFFFF)
				{ 				
					wgcollector_data_buff[TD_param_num++]      =  collector_temp & 0x00ff;           		//���նȵ�λ
					wgcollector_data_buff[TD_param_num++]      =  (collector_temp & 0xff00)>>8;      		//���նȸ�λ 					
				}
			 else
			 {
			   TD_param_num++;           	                      	//���նȵ�λ
				 TD_param_num++;      		                           //���նȸ�λ 				
			 }			 
			break;
      }
		 
     if(factory_gateway_set[12]==5&&factory_gateway_set[15]!=18)//����ѹ��;factory_gateway_set[15]=18 Ϊ������Һλ�����ȼ�⣬������ʹ��SCK			 
		 {
			 collector_temp           =  GET_PRESSUE0();
			 if(collector_temp!=0xffff)
			 {
			 wgcollector_data_buff[TD_param_num++]      =  collector_temp & 0x00ff;           		//����ѹ����λ
			 wgcollector_data_buff[TD_param_num++]      =  (collector_temp & 0xff00)>>8;      		//����ѹ����λ
       }
			 else
			 {
			  TD_param_num++;
				TD_param_num++;
			 }				 
		  break;
		 }		 

     if(factory_gateway_set[12]==6)//����������			 
		 {
			collector_temp=PCin(0);//���ն������PC0
			wgcollector_data_buff[TD_param_num++]      =  collector_temp & 0x00ff;           		//���뿪����=1����collector_data_buff[0]=0x01
			wgcollector_data_buff[TD_param_num++]      =  (collector_temp & 0xff00)>>8;      		//���뿪����=1����collector_data_buff[1]=0x00
		  break;
		 }			 
     if(factory_gateway_set[12]==7)//Ƶ������;
		 {
			 u8 i;
			 collector_temp=0;
			 for(i=freq_I+1;i<=60;i++){collector_temp=collector_temp+TIM2_FrequencyPC0[i];}
			 for(i=0;i<freq_I;i++){collector_temp=collector_temp+TIM2_FrequencyPC0[i];}
			 wgcollector_data_buff[TD_param_num++]      =  collector_temp & 0x00ff;           		//����Ƶ���ֽڵ�
			 wgcollector_data_buff[TD_param_num++]      =  (collector_temp & 0xff00)>>8;      		//����Ƶ���ֽڸߣ�ÿ����������������60ΪƵ��
		  break;
		 }
     if(factory_gateway_set[12]==8)//0~1VDC���룻
		 {
			TD_param_num++;
			TD_param_num++;
		  break;
		 }
     if(factory_gateway_set[12]==10&&factory_gateway_set[15]!=18)////Һλѹ��cps20;factory_gateway_set[15]=18 Ϊ������Һλ�����ȼ�⣬������ʹ��SCK	
		 {
			 collector_temp           =  GET_level0();
			 if(collector_temp!=0xffff)
			 {
			 wgcollector_data_buff[TD_param_num++]      =  collector_temp & 0x00ff;           		//Һλѹ����λ,������k=193.16 b=30 ��λ��kpa
			 wgcollector_data_buff[TD_param_num++]      =  (collector_temp & 0xff00)>>8;      		//Һλѹ����λ	
			 absolute_pressure_lower=collector_temp;
			 }
			 else
			 {
			  TD_param_num++;
				TD_param_num++;
			 }
		  break;
		 }     
		break;
			
		case 1:
			if(factory_gateway_set[15]==2)//������ʪ������
		  {
			 collector_temp = SI7021_HumiMeasurement();//����ʪ��;PA1�����ݿڣ�PC3ΪCLK
			 if(collector_temp >= 0xD916)
			 {
				collector_temp = 0xD916;
				}	
				wgcollector_data_buff[TD_param_num++]  = collector_temp&0x00ff;           
				wgcollector_data_buff[TD_param_num++] = (collector_temp&0xff00)>>8;
				
				collector_temp = SI7021_TempMeasurement();//�����¶�
				wgcollector_data_buff[TD_param_num++]      =  collector_temp&0x00ff;           
				wgcollector_data_buff[TD_param_num++]      =  (collector_temp&0xff00)>>8;   
		  break;
		  }
			if(factory_gateway_set[15]==10)//Һλѹ��cps120
		  {
			 collector_temp = GET_level1();//����ʪ��;PA1�����ݿڣ�PC3ΪCLK
			 if(collector_temp !=0xffff)
			 {				
				wgcollector_data_buff[TD_param_num++] = collector_temp&0x00ff;//����ѹ��  ������k=182.04  b=30����λ��kpa         
				wgcollector_data_buff[TD_param_num++] = (collector_temp&0xff00)>>8;				
				wgcollector_data_buff[TD_param_num++] =  level1_temperature&0x00ff; //�����¶ȣ�������k=99.30  b=-40����λ���棩          
				wgcollector_data_buff[TD_param_num++] =  (level1_temperature&0xff00)>>8;
        absolute_pressure_upper=collector_temp; 				 
			 }
			 else
			 {
			   TD_param_num++;
				 TD_param_num++;
				 TD_param_num++;
				 TD_param_num++;				
			 }
			  break;
		  }
			
			if(factory_gateway_set[15]==12)//��ѹҺλLWP5050GD,��ʽ ���pa=(65000*collector_temp��/65536-10000
		  {
			 collector_temp = PRESSUE_level1();//����ʪ��;PA1�����ݿڣ�PC3ΪCLK	
       if(collector_temp<=0xF000)	//���collector_temp>=61440,��ʾû�нӴ�����������ѹ��-10000~50937pa
			 {	         
          collector_temp=collector_temp-(factory_gateway_set[16]*100);
          collector_temp=collector_temp+(factory_gateway_set[17]*100);				 
					wgcollector_data_buff[TD_param_num++] = collector_temp&0x00ff;//��ѹҺλ ��          
					wgcollector_data_buff[TD_param_num++] = (collector_temp&0xff00)>>8;				
					wgcollector_data_buff[TD_param_num++] =  level1_temperature&0x00ff; //�����¶ȣ���          
					wgcollector_data_buff[TD_param_num++] = (level1_temperature&0xff00)>>8;
					absolute_pressure_upper=collector_temp;				
					if(absolute_pressure_upper>=(10082-1000)&&absolute_pressure_upper<=(10082+1000))//��2500��-2.28kpa~+2.28kpa
					{
						pressure_correct++;
						absolute_pressure_zero[pressure_correct]=absolute_pressure_upper;
						if(pressure_correct>36)//5*36=180s,pressure_correct��󲻳���50�������޸�absolute_pressure_zero����
						{
							u32 sum_pressure_lower;
							u8 i;
							sum_pressure_lower=0;
							for(i=0;i<pressure_correct;i++)
							{
							 sum_pressure_lower=sum_pressure_lower+absolute_pressure_zero[i];
							}
							absolute_pressure_lower=sum_pressure_lower/pressure_correct;
							pressure_correct=0;
						}
					}
					else //if(absolute_pressure_upper>(10082+1500))
					{
					 pressure_correct=0;
					}
				}
			  break;
		  }
		 if(factory_gateway_set[15]==5)//����ѹ��			 
		 {
			 collector_temp           =  GET_PRESSUE1();
			 if(collector_temp!=0xffff)
			 {
			 wgcollector_data_buff[TD_param_num++]      =  collector_temp & 0x00ff;           		//����ѹ����λ
			 wgcollector_data_buff[TD_param_num++]      =  (collector_temp & 0xff00)>>8;      		//����ѹ����λ
//       TD_param_num++;           
//       TD_param_num++;
				}
			 else
			 {
			  TD_param_num++;
				TD_param_num++;
//				TD_param_num++;
//				TD_param_num++; 
			 }
			break;
			}
		 if(factory_gateway_set[15]==6)//����������
		 {			 
			 collector_temp           =  PAin(1);//������ʪ�����ݽ�PA0
			 wgcollector_data_buff[TD_param_num++]      =  collector_temp & 0x00ff;        //���뿪����=0����collector_data_buff[0]=0x00
			 wgcollector_data_buff[TD_param_num++]      =  (collector_temp & 0xff00)>>8;   //���뿪����=0����collector_data_buff[1]=0x00
//       TD_param_num++;
//			 TD_param_num++;
			break;
			}			 
		 if(factory_gateway_set[15]==7)//Ƶ������
		 {
			 u8 i;			 
			 collector_temp=0;
			 for(i=freq_I+1;i<=60;i++){collector_temp=collector_temp+TIM2_FrequencyPA1[i];}
			 for(i=0;i<freq_I;i++){collector_temp=collector_temp+TIM2_FrequencyPA1[i];}
			 
			 wgcollector_data_buff[TD_param_num++]      =  collector_temp & 0x00ff;        //����Ƶ���ֽڵ�
			 wgcollector_data_buff[TD_param_num++]      =  (collector_temp & 0xff00)>>8;   //����Ƶ���ֽڸߣ�ÿ����������������60ΪƵ��
//       TD_param_num++;
//			 TD_param_num++;
			break;
			}
		 if(factory_gateway_set[15]==18)//�������룻������Һλ���
		 {
			 u8 i,j;
       u16 t; 
       PA1_pulse_time=0;
			 TIM5_pulsePA1_NUM=factory_gateway_set[12+3*5+2];//����8�˲�����
			 if(TIM5_pulsePA1_NUM<12){TIM5_pulsePA1_NUM=12;}
			 if(TIM5_pulsePA1_NUM>50){TIM5_pulsePA1_NUM=50;}
			 memcpy(TIM5_sort_pulsePA1,TIM5_pulsePA1,4*TIM5_pulsePA1_NUM);//TIM2_pulsePA1_NUM=32*2�ֽ�			
       for(i=0;i<TIM5_pulsePA1_NUM;i++)//32��������
       {
				for(j=i+1;j<TIM5_pulsePA1_NUM;j++)
				 {
				  if(TIM5_sort_pulsePA1[i]>TIM5_sort_pulsePA1[j])//TIM2_sort_pulsePA1��������
					 {
						 t=TIM5_sort_pulsePA1[i];
						 TIM5_sort_pulsePA1[i]=TIM5_sort_pulsePA1[j];
						 TIM5_sort_pulsePA1[j]=t;
					 }
				  }
        }
			 for(i=5;i<TIM5_pulsePA1_NUM-5;i++)	
			 {
				 PA1_pulse_time=PA1_pulse_time+TIM5_sort_pulsePA1[i];
			 }
			 collector_temp=(PA1_pulse_time*340)/(200*(TIM5_pulsePA1_NUM-10)); //��λ��0.1mm		 
        temp_level=collector_temp;//��ͨ��5ʹ�ã�����8�����㳵��ʵ��Һλ		 
			 wgcollector_data_buff[TD_param_num++]      =  collector_temp & 0x00ff;        //����Ƶ���ֽڵ�
			 wgcollector_data_buff[TD_param_num++]      =  (collector_temp & 0xff00)>>8;   //����Ƶ���ֽڸߣ�ÿ����������������60ΪƵ��
//       TD_param_num++;
//			 TD_param_num++;
			break;
			}
	if(factory_gateway_set[15]==8)//0~1VDC����,Ӳ��ȡ��50ŷķ����
		 {
			 TD_param_num++;
			 TD_param_num++;
//			 TD_param_num++;
//			 TD_param_num++;
			break;
			}
	if(factory_gateway_set[15]==23)//��Ӧ��ѩ����������,Ӳ��ȡ��50ŷķ����
	{
		wgcollector_data_buff[TD_param_num++]      =  report_last_rain & 0x00ff;        //����Ƶ���ֽڵ�
		wgcollector_data_buff[TD_param_num++]      =  (report_last_rain & 0xff00)>>8;   //����Ƶ���ֽڸߣ�ÿ����������������60ΪƵ��	 
		break;
	}	 
		break;
			
		case 2:
     if(factory_gateway_set[18]==3||factory_gateway_set[18]==8)
		 {			
		 	 TD_number=0;
		     if(factory_gateway_set[18]==8){temp_adc=Get_Adclvbo(TD_number,0,1305);}
		     else {temp_adc=Get_Adclvbo(TD_number,262,1305);}   				
		     if(First_adc_average[TD_number]==0)
		      {  
				   temp_adc=First_Getaverage( TD_number, Maxlvbo_number, temp_adc);
				   First_adc_average[TD_number]=1;
		      }
          TDlvbo_number[TD_number]=factory_gateway_set[12+2*3+2];//ȡ���������趨���˲��������ֽ� 					
				  temp_adc=TD_Getaverage( TD_number,TDlvbo_number[TD_number], temp_adc,tdcycle_i[TD_number]);
			      First_adc_average[TD_number]=1;
			      tdcycle_i[TD_number]++;
			      if(tdcycle_i[TD_number]>=TDlvbo_number[TD_number])
			       {
			   	     tdcycle_i[TD_number]=0;
			       }
			collector_temp          =  temp_adc;//����ˮ��
			dp_temp_level=temp_adc;//�߾�ѹ��ѹ����ͨ��5ʹ�ã�����8�����㳵��ʵ��Һλ
      dr_temp_level=temp_adc;//������Ƶ����ͨ��5ʹ�ã�����8�����㳵��ʵ��Һλ						 
		  wgcollector_data_buff[TD_param_num++]     =  collector_temp&0x00ff;         		 //����ˮ�ֵ�λ
		  wgcollector_data_buff[TD_param_num++]     =  (collector_temp&0xff00)>>8;
		}						 
		break;
		
		case 3:
		 if(factory_gateway_set[21]==3||factory_gateway_set[21]==8)
		 {			 
			TD_number=1;
		     if(factory_gateway_set[21]==8){temp_adc=Get_Adclvbo(TD_number,0,1305);}
		     else {temp_adc=Get_Adclvbo(TD_number,262,1305);}   				
		     if(First_adc_average[TD_number]==0)
		      {  
				   temp_adc=First_Getaverage( TD_number, Maxlvbo_number, temp_adc);
				   First_adc_average[TD_number]=1;
		      }
		    TDlvbo_number[TD_number]=factory_gateway_set[12+3*3+2];//ȡ���������趨���˲��������ֽڣ��˲��������255�� 	
			  temp_adc=TD_Getaverage( TD_number,TDlvbo_number[TD_number], temp_adc,tdcycle_i[TD_number]);
		      First_adc_average[TD_number]=1;
		      tdcycle_i[TD_number]++;
		      if(tdcycle_i[TD_number]>=TDlvbo_number[TD_number])
		       {
		   	     tdcycle_i[TD_number]=0;
		       }
			collector_temp          =  temp_adc; //�����¶�
			wgcollector_data_buff[TD_param_num++]     =  collector_temp&0x00ff;          	//�����¶ȵ�λ
			wgcollector_data_buff[TD_param_num++]     =  (collector_temp&0xff00)>>8;    	//�����¶ȸ�λ 
	  }					 
		break;
		
		case 4:
			if(factory_gateway_set[24]==4&&factory_gateway_set[15]!=18)//������̼����
		  {
				collector_temp            =  Get_Carbon();//������̼
				if(collector_temp<60000)
				{
					wgcollector_data_buff[TD_param_num++]     =  collector_temp&0x00ff;          	//������̼��λ
					wgcollector_data_buff[TD_param_num++]     =  (collector_temp&0xff00)>>8;    	//������̼��λ
				 }
        else
				{
				  TD_param_num++;                             	//������̼��λ
					TD_param_num++;                             	//������̼��λ
				}					
				break;
		   }
      if(factory_gateway_set[24]==5&&factory_gateway_set[15]!=18)//����ѹ��
			{
				collector_temp           =  GET_PRESSUE4();
		    if(collector_temp!=0xffff)
			 {
		    wgcollector_data_buff[TD_param_num++]      =  collector_temp & 0x00ff;           		//����ѹ����λ
		    wgcollector_data_buff[TD_param_num++]      =  (collector_temp & 0xff00)>>8;      		//����ѹ����λ
        }
			 else
			 {
			  TD_param_num++;
				TD_param_num++;
			 } 				 
				break;
			 }
      if(factory_gateway_set[24]==6)//����������
		  {
			 	collector_temp=PAin(0);//������̼���ݽ�PA0
			  wgcollector_data_buff[TD_param_num++]      =  collector_temp & 0x00ff;           		//���뿪����=1����collector_data_buff[10]=0x01
			  wgcollector_data_buff[TD_param_num++]      =  (collector_temp & 0xff00)>>8;      		
		    break;
		  }			 
      if(factory_gateway_set[24]==7)//Ƶ������
		  {
			 u8 i;
			 collector_temp=0;
			 for(i=freq_I+1;i<=60;i++){collector_temp=collector_temp+TIM2_FrequencyPA0[i];}
			 for(i=0;i<freq_I;i++){collector_temp=collector_temp+TIM2_FrequencyPA0[i];}
			 wgcollector_data_buff[TD_param_num++]      =  collector_temp & 0x00ff;           //����Ƶ���ֽڵ�
			 wgcollector_data_buff[TD_param_num++]      =  (collector_temp & 0xff00)>>8;     //����Ƶ���ֽڸߣ�ÿ����������������60ΪƵ��
		   break;
		   }
      if(factory_gateway_set[24]==8)//0~1VDC����,Ӳ��ȡ��50ŷķ����
		  {
				 TD_param_num++;
				 TD_param_num++;				
				 break;
		   }
      if(factory_gateway_set[24]==10&&factory_gateway_set[15]!=18)//Һλѹ��cps120
			{
				collector_temp           =  GET_level4();
				if(collector_temp!=0xffff)
			 {
		    wgcollector_data_buff[TD_param_num++]      =  collector_temp & 0x00ff;           		//Һλѹ����λ��������k=193.16 b=30����λ��kpa
		    wgcollector_data_buff[TD_param_num++]      =  (collector_temp & 0xff00)>>8;      		//Һλѹ����λ
				
        }
			 else
			 {
			  TD_param_num++;
				TD_param_num++;
			 } 				 
				break;
			 }				 			
		break;
		 
	  case 5:
			if(factory_gateway_set[27]==11||factory_gateway_set[27]==16)//��ѹ����
		   {
				if(absolute_pressure_upper>=absolute_pressure_lower)
				{
				  temp_flow=absolute_pressure_upper-absolute_pressure_lower;
//					if(factory_gateway_set[15]==12&&factory_gateway_set[16]!=0&&factory_gateway_set[17]!=0)
//					{
//					  temp_flow=(temp_flow*factory_gateway_set[17])/factory_gateway_set[16];
//						if(temp_flow>(factory_gateway_set[17]*100+500)){temp_flow=(factory_gateway_set[17]*100+500);}
//					}
					TD_number=2;
				 if(temp_flow<300)
				 {
				   First_adc_average[TD_number]=0;//�Ƿ��һ�ν��룬��=0
					 temp_flow=0;
				 }				 
		     if(First_adc_average[TD_number]==0)
		      {  
				   temp_flow=First_Getaverage( TD_number, Maxlvbo_number, temp_flow);
				   First_adc_average[TD_number]=1;
		      }
          TDlvbo_number[TD_number]=factory_gateway_set[12+5*3+2];//ȡ���������趨�Ĳ�ѹ�˲��������ֽ� 					
				  temp_flow=TD_Getaverage(TD_number,TDlvbo_number[TD_number], temp_flow,tdcycle_i[TD_number]);
//			      First_adc_average[TD_number]=1;
			      tdcycle_i[TD_number]++;
			      if(tdcycle_i[TD_number]>=TDlvbo_number[TD_number])
			       {
			   	     tdcycle_i[TD_number]=0;
			       }			  
				 if(factory_gateway_set[13]!=0&&factory_gateway_set[14]!=0)
				 {
				   temp_flow=(factory_gateway_set[14]*temp_flow)/factory_gateway_set[13];
				 }
				 collector_temp          =  temp_flow;//
				 wgcollector_data_buff[14]      =  collector_temp & 0x00ff;           		//��ѹ��λ��������k=1893.72  b=0����λ��M
		     wgcollector_data_buff[15]      =  (collector_temp & 0xff00)>>8;      		//��ѹ��λ		
				}			
		   break;
		   }
    if(factory_gateway_set[27]==19||factory_gateway_set[27]==17)//���峬��������Һλ�߶ȼ���
		{
			
			temp_level_up=factory_gateway_set[28]*100;//��ѹ�����������Һλ������8�ı�������Ϊ���߶ȣ���λ��cm������ת��Ϊ0.1mm		
		 if(temp_level>temp_level_up){temp_level=temp_level_up;}
		 if(temp_level<3000){temp_level=0;}//�����趨
	 
	  temp_levelS=temp_level_up-temp_level;

	  wgcollector_data_buff[14]=temp_levelS & 0x00ff; 
		wgcollector_data_buff[15]=(temp_levelS & 0xff00)>>8;
    break;				 
		}
		
   if(factory_gateway_set[27]==20)//�߾�ѹ��ѹ����Һλ�߶ȼ���
		{			
			temp_level_up=factory_gateway_set[28]*100;//��ѹ�����������Һλ������8�ı�������Ϊ���߶ȣ���λ��cm������ת��Ϊ0.1mm
      temp_levelS=(dp_temp_level-262)*(20000/(1305-262));
		 if(temp_levelS<0){temp_levelS=0;}//�����趨
		 if(temp_levelS>temp_level_up){temp_levelS=temp_level_up;}
		 temp_levelS=temp_levelS+800;
		 if(temp_levelS<2000&&temp_levelS>=0)temp_levelS=0;//ҺλС��80mm������Ϊ0��Һλ�Զ�У��
			wgcollector_data_buff[14]=temp_levelS & 0x00ff; 
			wgcollector_data_buff[15]=(temp_levelS & 0xff00)>>8;
    break;				 
		}
		
   if(factory_gateway_set[27]==21)//������Ƶ����Һλ�߶ȼ���
		{
      u32 temp_js;			
			temp_level_up=factory_gateway_set[28]*100;//��ѹ�����������Һλ������8�ı�������Ϊ���߶ȣ���λ��cm������ת��Ϊ0.1mm
			temp_js=(dr_temp_level-262)*factory_gateway_set[19]*100;//��������ˮ�ֵĲ��������������ֵ
      temp_levelS=temp_js/(1305-262);
		  if(temp_levelS<0){temp_levelS=0;}//�����Ư�ƣ��Զ�����
		  temp_levelS=temp_levelS+800;
		  if(temp_levelS>temp_level_up){temp_levelS=temp_level_up;}	//���ܳ�����������ֵ
			
		 //ҺλС��factory_gateway_set[29]�趨���£�����Ϊ0��Һλ�Զ�У��;lag_value��ֹ��㷴����
		 if(temp_levelS<((factory_gateway_set[29]*100)+lag_value))
		 {
			 temp_levelS=0;
		   lag_value=1000;
		 }
		if(temp_levelS>(temp_level_up-3000)){lag_value=0;}//����װ���ӽ���������������lag_value��ֵ
	  wgcollector_data_buff[14]=temp_levelS & 0x00ff; 
		wgcollector_data_buff[15]=(temp_levelS & 0xff00)>>8;
    break;				 
		}
		
		if(factory_gateway_set[27]==22&&TD_param_num<=6)//������Ƶ����Һλ�߶ȼ���
		{
     wgcollector_data_buff[TD_param_num++]=data_buf_FXFS[1];//������ֽ�
		 wgcollector_data_buff[TD_param_num++]=data_buf_FXFS[0];//������ֽ�
		 wgcollector_data_buff[TD_param_num++]=data_buf_FXFS[3];//���ٵ��ֽ�
		 wgcollector_data_buff[TD_param_num++]=data_buf_FXFS[2];//���ٸ��ֽ�
		 
		 wgcollector_data_buff[TD_param_num++]=data_buf_PM[1];//PM1.0���ֽ�
		 wgcollector_data_buff[TD_param_num++]=data_buf_PM[0];//PM1.0���ֽ�
		 wgcollector_data_buff[TD_param_num++]=data_buf_PM[3];//PM2.5���ֽ�
		 wgcollector_data_buff[TD_param_num++]=data_buf_PM[2];//PM2.5���ֽ�
     wgcollector_data_buff[TD_param_num++]=data_buf_PM[5];//PM2.5���ֽ�
		 wgcollector_data_buff[TD_param_num++]=data_buf_PM[4];//PM2.5���ֽ�		
    break;				 
		}
		break;
	}
	memcpy(Collectors[64],wgcollector_data_buff,16);	
	if(ReadDataCNT>=5){ReadDataCNT=0;}
	else {ReadDataCNT++;}
		/*����ΪҺλ---��������*/
	if(factory_gateway_set[27]==16||factory_gateway_set[27]==17)
	{
		u8  flow_temp[12];
		bytelen_to_asc((unsigned char *)flow_temp,wgcollector_data_buff[15]);//Һλ���ֽ� pa
		bytelen_to_asc((unsigned char *)flow_temp+3,wgcollector_data_buff[14]);//Һλ���ֽ� pa
		bytelen_to_asc((unsigned char *)flow_temp+6,wgcollector_data_buff[13]);//�ۻ������ֽ� ��
		bytelen_to_asc((unsigned char *)flow_temp+9,wgcollector_data_buff[12]);//�ۻ������ֽ� ��
		
		memcpy(USART1SendTCB,flow_temp,12);
		WriteDataToDMA_BufferTX1(12);
	}		
}

static void startadc(void)
{
	DMA_Cmd(DMA1_Channel1,ENABLE);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);  //����ת����ʼ��ADCͨ��DMA��ʽ���ϵĸ���RAM���� 
}

u16 Get_Adclvbo(u8 TD_Xnumber,u16 TD_xiaxian,u16 TD_shangxian)
 {
   u8 i,j;
   u16 AdcLvbo[100],Temp_adc,t;
   u32 Temp_adcto=0;
   for(i=0;i<100;i++)
     {Temp_adc=ADC_ConvertedValue[TD_Xnumber];
      if(Temp_adc<TD_xiaxian)   //����
	   Temp_adc=TD_xiaxian;
	  if(Temp_adc>TD_shangxian)	 //����
	   Temp_adc=TD_shangxian;
      AdcLvbo[i]= Temp_adc;
     }

   for(i=0;i<100;i++)//100��������
   {
      for(j=i+1;j<100;j++)
       {
         if(AdcLvbo[i]>=AdcLvbo[j])
	     {
	       t=AdcLvbo[i];
	       AdcLvbo[i]=AdcLvbo[j];
	       AdcLvbo[j]=t;
	     }
       }
   }
   for(i=40;i<60;i++)	
   {
     Temp_adcto=Temp_adcto+AdcLvbo[i];
   }
	Temp_adcto=Temp_adcto/20; 
 
  return Temp_adcto;
}

u16 First_Getaverage(u8 td_xnumber,u8 maxlvbo_xnumber,u16 temp_adc)
{  u8 i;
   for(i=0;i<maxlvbo_xnumber;i++)
   {
   	  Adc_average[td_xnumber][i] =temp_adc;
	  
   }  return temp_adc;
}

u16 TD_Getaverage(u8 td_xnumber,u8 tdlvbo_xnumber,u16 temp_xadc,u8 tdcycle_xi)
{ u8 i;
  u32 average_adcto=0;
  Adc_average[td_xnumber][tdcycle_xi] =temp_xadc;
	if(tdlvbo_xnumber==0){tdlvbo_xnumber=1;}
  for(i=0;i<tdlvbo_xnumber;i++)
  {
  	 average_adcto=average_adcto+Adc_average[td_xnumber][i];  //���
  }
  temp_xadc=average_adcto/tdlvbo_xnumber;  //��ƽ��ֵ
  return temp_xadc;
}
//�������ɼ����������

// ������ת�����ַ���
// ������data
// ���أ�str
static u8 float_to_string(float data, u8 *str)
{
  int i,j,k;
	float data_temp;
  long temp,tempoten;
  u8 intpart[20], dotpart[20];  // �����������ֺ�С������

  //1.ȷ������λ
  if(data<0) {str[0]='-';data=-data;}
  else str[0]=' ';
  // С����3λ��5���Ƚ�λ
  data_temp=data*100.0f-(long)(data*100.0f);
  if(data_temp>=0.5){data=data+0.01f;}		
  //2.ȷ����������
  temp=(long)data;

  i=0;
  tempoten=temp/10;
  while(tempoten!=0)
  {
    intpart[i]=temp-10*tempoten+48; //to ascii code
    temp=tempoten;
    tempoten=temp/10;
    i++;
  }
  intpart[i]=temp+48;

  //3.ȷ��С������,ȡ��2λС��
  data=data-(long)data;	
  for(j=0;j<2;j++)
  {
    dotpart[j]=(int)(data*10)+48;
    data=data*10.0;
    data=data-(long)data;
  }    
  //4.������װ
  for(k=1;k<=i+1;k++) str[k]=intpart[i+1-k];
  str[i+2]='.';
  for(k=i+3;k<i+j+3;k++) str[k]=dotpart[k-i-3];
	return i+j+3 ;
//  str[i+j+3]=0x0D;

}


static void IOxh_send_cmd(void)
{
 	if(factory_gateway_set[15]==18)//��PA1-DATA4�������ȼ�⣬��TIM2����Ӧ���ж�
				{	
					level_num_err++;
					if(level_num_err>5){while(1);}//���20�η��ͼ���������û���յ�TIME5D1�ж�,��ϵͳ��������TIME5�жϳ�����level_num_err����
					TIM5->CCER&=~(1<<5);//�����ز���
					PCout(3)=1;
					Delayus(2000);
					PCout(3)=0;
				}								
 if(factory_gateway_set[27]==15)//����1������Һλ���� 
 {
	 memcpy(USART1SendTCB,uart1_cmd_csb,1);//ReportData2
   WriteDataToDMA_BufferTX1(1);   
  } 
}
//����5����
static void uart5_send_cmd(void)
{
 if(factory_gateway_set[27]==13) {WriteDataToBuffer(5,(u8 *)uart5_cmd_csb,0,8);}//������������
 if(factory_gateway_set[27]==14||factory_gateway_set[27]==16||factory_gateway_set[27]==17) {WriteDataToBuffer(5,(u8 *)uart5_cmd_dc,0,8);}//���������
 if(factory_gateway_set[27]==22)
  {
   if(switch_cmd_addr==0){WriteDataToBuffer(5,(u8 *)uart5_cmd_FSFX,0,8);}//����+����
   if(switch_cmd_addr==1){WriteDataToBuffer(5,(u8 *)uart5_cmd_PM,0,8);}//PM1.0+PM2.5+PM10
	 if(switch_cmd_addr>=1){switch_cmd_addr=0;}
	 else {switch_cmd_addr++;}
  }
 if(factory_gateway_set[27]==25) {WriteDataToBuffer(5,(u8 *)uart5_cmd_LEL,0,8);}//��ȼ���屨����	
}

static void RxReport5(u8 len,u8 *pData)
{	
	if(GetCRC16(pData,len)==0)
	{
		
	 if(factory_gateway_set[27]==13)//������������
	 {
			if(pData[0]==0x01&&pData[1]==0x03)						//�յ����������ص�����
			{  
					//�����ƻظ�˲ʱ��������ַ��0x01
					u32 temp;
					memcpy(uart5_data_buf,pData+3,4);	// ���ֽ�Q_water_data[0]~Q_water_data[3];��λ����/Сʱ
					temp=(u32)uart5_data_buf[3]<<24|(u32)uart5_data_buf[2]<<16|(u32)uart5_data_buf[1]<<8|(u32)uart5_data_buf[0];
					temp=temp>>4;//�ȳ���16��ƽ̨��ʽ�ٳ�62.5,ת���ɵ�λΪ��������/Сʱ
					uart5_data_buf[0]=(u8)(temp&0x000000FF);
					uart5_data_buf[1]=(u8)((temp>>8)&0x000000FF);
				 wgcollector_data_buff[12]=uart5_data_buf[0];
				 wgcollector_data_buff[13]=uart5_data_buf[1];
	       //			memcpy(Collectors[pData[0]-1],pData+3,12);	//  01 03 0C |-- --| -- --| -- --| -- --| -- --| CRC CRC
				//����ָ���൱�ڴ�memcpy[��վ��][0]��ʼ���12������;pData[0]=��վ��+1��pData[1]=0x03���ܺţ�pData[2]=��������(ÿ������2�ֽ�)		
	      //			Start_timerEx( WGSEND_SLAVECMD_EVT, 80 );
			}
		return;
	 }
	 
	if(factory_gateway_set[27]==14||factory_gateway_set[27]==16||factory_gateway_set[27]==17)//���������
	 {
			if(pData[0]==0x01&&pData[1]==0x03)						//�յ����������ص�����
		{        
			  u32 temp;
			  memcpy(uart5_data_buf,pData+3,4);	// ���ֽ�Q_water_data[0]~Q_water_data[3];�ۼ����� ��λ����
			  temp=(u32)uart5_data_buf[0]<<24|(u32)uart5_data_buf[1]<<16|(u32)uart5_data_buf[2]<<8|(u32)uart5_data_buf[3];
			  uart5_data_buf[0]=(u8)(temp&0x000000FF);
			  uart5_data_buf[1]=(u8)((temp>>8)&0x000000FF);
			 wgcollector_data_buff[12]=uart5_data_buf[0];
			 wgcollector_data_buff[13]=uart5_data_buf[1];
		}
		return;
	 }
	 
  if(factory_gateway_set[27]==22)//����(k=1,b=0)+����(k=100,b=0) PM1.0+PM2.5+PM10(k=1,b=0)
	 {
			if(pData[0]==0x01&&pData[1]==0x03)						//�յ����ٷ��򴫸������ص�����
		   {
        u16 temp_FS;
        u8 	temp_FS_int[4];			 
			  memcpy(uart5_data_buf,pData+3,6);	// ������ֽ� ���ֽ�+����4�ֽڸ���
			  data_buf_FXFS[0]=uart5_data_buf[0];
				data_buf_FXFS[1]=uart5_data_buf[1];
				temp_FS_int[3]=uart5_data_buf[4];
				temp_FS_int[2]=uart5_data_buf[5];
				temp_FS_int[1]=uart5_data_buf[2];
				temp_FS_int[0]=uart5_data_buf[3];
        memcpy(data_buf_FS.wind_speed_int,temp_FS_int,4);
        temp_FS=data_buf_FS.wind_speed_float*100;	
        data_buf_FXFS[2]=(temp_FS & 0xff00)>>8;	
        data_buf_FXFS[3]=temp_FS & 0x00ff;			 
		   }
			if(pData[0]==0x02&&pData[1]==0x03)						//�յ�PM1.0+PM2.5+PM10���������ص�����
		   {			  
			  memcpy(data_buf_PM,pData+3,6);	//PM1.0���ֽ� ���ֽ�+PM2.5���ֽ� ���ֽ�+PM10���ֽ� ���ֽ�	        		 
		   }
			 return;
	 }
	 if(factory_gateway_set[27]==25)//��ȼ���屨����
	 {
			if(pData[0]==0x01&&pData[1]==0x03)						//�յ����������ص�����
		  {		
			 wgcollector_data_buff[12]=pData[4];//k=100,b=0,��λ%
			 wgcollector_data_buff[13]=pData[3];
		  }
			return;
	  }
	 
	}		 			
}

static void RxReport1_csb_yw(u8 len,u8 *pData)//����������1Һλ���
{
	
	if(len!=2)return;
	temp_level=(u16)pData[0]<<8|(u16)pData[1];
//	if(temp_level>1500)temp_level=1500;	 
					TD_number=2;				 
		     if(First_adc_average[TD_number]==0)
		      {  
				   temp_level=First_Getaverage( TD_number, Maxlvbo_number, temp_level);
				   First_adc_average[TD_number]=1;
		      }
          TDlvbo_number[TD_number]=factory_gateway_set[12+5*3+2];//ȡ���������趨�Ĳ�ѹ�˲��������ֽ� 					
				  temp_level=TD_Getaverage(TD_number,TDlvbo_number[TD_number], temp_level,tdcycle_i[TD_number]);
//			      First_adc_average[TD_number]=1;
			      tdcycle_i[TD_number]++;
			      if(tdcycle_i[TD_number]>=TDlvbo_number[TD_number])
			       {
			   	     tdcycle_i[TD_number]=0;
			       }
			 temp_level_up=factory_gateway_set[28]*10;//��ѹ�����������Һλ������8�ı�������Ϊ���߶ȣ���λ��cm������ת��Ϊmm		
			 if(temp_level>temp_level_up){temp_level=temp_level_up;}
			 
//     if(temp_level>900)//Һλ�Զ�У��
//				 {					 
//					 temp_level_num++;
//           if(temp_level_num>60)
//           {						 
//				    First_adc_average[TD_number]=0;
//					  temp_level_up=temp_level;
//						temp_level_num=0; 
//					 }
//				  }
//         else {temp_level_num=0;}				 
	  temp_levelS=temp_level_up-temp_level;
//		if(temp_levelS<0)temp_levelS=0;//Һλ�Զ�У��
	  wgcollector_data_buff[14]=temp_levelS & 0x00ff; 
		wgcollector_data_buff[15]=(temp_levelS & 0xff00)>>8;	
}




















