#ifndef __MYPID_H
#define __MYPID_H
/*PID结构体定义*/
typedef struct {
  float maximum;               /*输出值上限*/
  float minimum;                /*输出值下限*/

  float setpoint;                /*设定值*/
  float kp;                      /*比例系数*/
  float ki;                     /*积分系数*/
                       

  float lasterror;              /*前一拍偏差*/
  float preerror;               /*前两拍偏差*/
  float result;                /*PID控制器结果*/
  float output;                 /*输出值*/

  float errorabsmax;
  float errorabsmin;
  float deadband;               /*死区*/
  float integralValue;
}vPID;


void PID_init(vPID *mypid,float vMin,float vMax,float sp);
void PIDRegulator(vPID *mypid,float pv); //pv为processvalue 实际测量值

#endif
