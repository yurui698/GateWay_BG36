#include "PIDCollector.h"


void pidcollector_data(void) //传感器采集数据
{
	//	u16 temp;	
	switch(ReadPIDCNT) 
	{		
		case 0:
		 pid_param_num=0;//建立每个通道上报参数的顺序，如果该通道不检测参数，后面通道检测的参数自动往前移
		 
     if(factory_gateway_set[12]==27)//水肥流量计频率输入;
		 {
			 u8 i;
			 pidcollector_temp=0;
//			 pidcollector_temp=TIM2_FrequencyPC0[freq_I];
			 for(i=freq_I+1;i<=60;i++){pidcollector_temp=pidcollector_temp+TIM2_FrequencyPC0[i];}
			 for(i=0;i<freq_I;i++){pidcollector_temp=pidcollector_temp+TIM2_FrequencyPC0[i];}
			
			 pidcollector_data_buff[pid_param_num++]      =  pidcollector_temp & 0x00ff;           		//输入频率字节低
			pidcollector_data_buff[pid_param_num++]      =  (pidcollector_temp & 0xff00)>>8;      		//输入频率字节高；每分钟脉冲数，除以60为频率
		  break;
		 }
     
		break;
			
		case 1:
			 
		 if(factory_gateway_set[15]==27)//水肥流量计频率输入
		 {
			 u8 i;			 
			 pidcollector_temp=0;
//			 pidcollector_temp=TIM2_FrequencyPA1[freq_I];
			for(i=freq_I+1;i<=60;i++){pidcollector_temp=pidcollector_temp+TIM2_FrequencyPA1[i];}
			 for(i=0;i<freq_I;i++){pidcollector_temp=pidcollector_temp+TIM2_FrequencyPA1[i];}
			 
			 pidcollector_data_buff[pid_param_num++]      =  pidcollector_temp & 0x00ff;        //输入频率字节低
			 pidcollector_data_buff[pid_param_num++]      =  (pidcollector_temp & 0xff00)>>8;   //输入频率字节高；每分钟脉冲数，除以60为频率
			break;
			}
		 
		break;
			
		case 2:
     	if(factory_gateway_set[24]==27)//水肥流量计频率输入
		  {
			 u8 i;
			 pidcollector_temp=0;
//			 pidcollector_temp=TIM2_FrequencyPA0[freq_I];
			 for(i=freq_I+1;i<=60;i++){pidcollector_temp=pidcollector_temp+TIM2_FrequencyPA0[i];}
			 for(i=0;i<freq_I;i++){pidcollector_temp=pidcollector_temp+TIM2_FrequencyPA0[i];}
			 
			 pidcollector_data_buff[pid_param_num++]      =  pidcollector_temp & 0x00ff;           //输入频率字节低
			 pidcollector_data_buff[pid_param_num++]      =  pidcollector_temp & 0xff00>>8;  //输入频率字节高；每分钟脉冲数，除以60为频率
		   break;
		   }
     break;
	}	 
	
	  memcpy(pidCollectors,pidcollector_data_buff,16);	


		bytelen3=WriteSingleRegister(66,0x0000,pidCollectors,ReportData3);//每个点2个字节
		memcpy(USART3SendTCB,ReportData3,bytelen3);
		WriteDataToDMA_BufferTX3(bytelen3);
	
		bytelen3=WriteSingleRegister(66,0x0001,pidCollectors+2,ReportData3);//每个点2个字节
		memcpy(USART3SendTCB,ReportData3,bytelen3);
		WriteDataToDMA_BufferTX3(bytelen3);
	
		bytelen3=WriteSingleRegister(66,0x0002,pidCollectors+4,ReportData3);//每个点2个字节
		memcpy(USART3SendTCB,ReportData3,bytelen3);
		WriteDataToDMA_BufferTX3(bytelen3);

	if(ReadPIDCNT>=2){ReadPIDCNT=0;}
	else {ReadPIDCNT++;}
		
}
