#include "PIDCollector.h"


void pidcollector_data(void) //�������ɼ�����
{
	//	u16 temp;	
	switch(ReadPIDCNT) 
	{		
		case 0:
		 pid_param_num=0;//����ÿ��ͨ���ϱ�������˳�������ͨ����������������ͨ�����Ĳ����Զ���ǰ��
		 
     if(factory_gateway_set[12]==7)//Ƶ������;
		 {
			 u8 i;
			 pidcollector_temp=0;
			 pidcollector_temp=TIM2_FrequencyPC0[freq_I];
			
			 pidcollector_data_buff[pid_param_num++]      =  pidcollector_temp & 0x00ff;           		//����Ƶ���ֽڵ�
			pidcollector_data_buff[pid_param_num++]      =  (pidcollector_temp & 0xff00)>>8;      		//����Ƶ���ֽڸߣ�ÿ����������������60ΪƵ��
		  break;
		 }
     
		break;
			
		case 1:
			 
		 if(factory_gateway_set[15]==7)//Ƶ������
		 {
			 u8 i;			 
			 pidcollector_temp=0;
			 pidcollector_temp=TIM2_FrequencyPA1[freq_I];
			
			 
			 pidcollector_data_buff[pid_param_num++]      =  pidcollector_temp & 0x00ff;        //����Ƶ���ֽڵ�
			 pidcollector_data_buff[pid_param_num++]      =  (pidcollector_temp & 0xff00)>>8;   //����Ƶ���ֽڸߣ�ÿ����������������60ΪƵ��
//       TD_param_num++;
//			 TD_param_num++;
			break;
			}
		 
		break;
			
		case 2:
     	if(factory_gateway_set[24]==7)//Ƶ������
		  {
			 u8 i;
			 pidcollector_temp=0;
			 pidcollector_temp=TIM2_FrequencyPA0[freq_I];
			 
			 pidcollector_data_buff[pid_param_num++]      =  pidcollector_temp & 0x00ff;           //����Ƶ���ֽڵ�
			 pidcollector_data_buff[pid_param_num++]      =  pidcollector_temp & 0xff00>>8;  //����Ƶ���ֽڸߣ�ÿ����������������60ΪƵ��
		   break;
		   }
     break;
	}	 
	
	  memcpy(pidCollectors,pidcollector_data_buff,16);	


		bytelen3=WriteSingleRegister(37,0,pidCollectors,ReportData3);//ÿ����2���ֽ�
		memcpy(USART3SendTCB,ReportData3,bytelen3);
		WriteDataToDMA_BufferTX3(bytelen3);

	if(ReadPIDCNT>=2){ReadPIDCNT=0;}
	else {ReadPIDCNT++;}
		
}