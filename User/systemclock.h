#ifndef __systemclock_H
#define __systemclock_H

#include "stm32f10x.h"

/*�¼���־λ�궨��*/
#define SYS_INIT_EVT           0x00000001
#define RX1_DELAY_EVT          0x00000002
#define RX1_TIMEOUT_EVT	       0x00000004
#define RX2_DELAY_EVT          0x00000008
#define JM_PLATFORM_REPLY_EVT  0x00000010
#define RX3_DELAY_EVT          0x00000020
#define RX3_TIMEOUT_EVT        0x00000040
#define RX4_DELAY_EVT          0x00000080
//#define RX4_TIMEOUT_EVT        0x00000100
#define RX5_DELAY_EVT          0x00000200
#define RX5_TIMEOUT_EVT        0x00000400
#define NET_INIT_EVT	         0x00000800
#define SEND_PLATFORM_EVT      0x00001000 
#define WG_SENDZZ_EVT          0x00002000
#define WG_REPLY_EVT           0x00004000
#define CYCLE_CMD_EVT          0x00008000
#define SET_SLAVEPARAM_EVT     0x00010000
#define WX_SENDZZ_EVT	         0x00020000
#define WX_CMD_EVT	           0x00040000
#define WX_RECEIVE_EVT         0x00080000
#define WX_SET_SLAVEPARAM_EVT  0x00100000
#define WGCOLLECTOR_DATA_EVT   0x00200000
#define TX5_CMD_EVT            0x00400000
#define IO_XUNHUAN_CMD_EVT     0x00800000
typedef struct timer_recall
{	
	u32 eventflag;   // �趨ʱ���־
    u16 timeout;     // �趨����ʱ��
	u16 reload_timeout;
	struct timer_recall *next;
}TimerRec;


#ifdef __cplusplus
extern "C"
{
#endif

void SysClock(void);
void GetSysTime(u32 *Day,u32 *mSec);
void TimerInit(void);
u8   Start_timerEx(u32 eventflag,u16 timeout);
u8   Start_reload_timer(u32 eventflag,u16 reload_timeout);
u8   Stop_timerEx(u32 eventflag);
u16  Get_timeoutEx(u32 eventflag);
u8   Timer_Num_Active(void);
void Set_Event(u32 eventflag);
void Clear_Event(u32 eventflag);
void Delayus(u32 us);//�ɼ�

   
#ifdef __cplusplus
}
#endif

#endif
