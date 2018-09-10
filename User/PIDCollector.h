#include "stm32f10x.h"
#include "string.h"
#ifndef __PIDCOLLECTOR_H
#define __PIDCOLLECTOR_H


u8  ReadPIDCNT = 0;
u8 pid_param_num = 0;
extern u8 factory_gateway_set[];
__attribute__((section("NO_INIT"),zero_init))  u8  pidcollector_data_buff[16]; //缓存pid采集频率
 __attribute__((section("NO_INIT"),zero_init))  u8 pidCollectors[16]; //存储pid频率数据
u8 pidcollector_temp;
extern u16 TIM2_FrequencyPA0[];
extern u16 TIM2_FrequencyPA1[];
extern u16 TIM2_FrequencyPC0[];
extern u8 freq_I;

extern u8  WriteSingleRegister(u8 Slave_ID,u16 addr,u8 *pData,u8 *temp);
extern u8 ReadData(u8 Slave_ID,u8 function_code,u16 addr,u16 num,u8 *temp);

extern u8 bytelen3;//Send_slave_cmd函数的临时变量定义
extern u8 USART3SendTCB[128];
extern u8 ReportData3[];

extern void WriteDataToDMA_BufferTX3(uint16_t size);
extern void RxReport3(u8 len,u8 *pData);
void pidcollector_data(void);
#endif
