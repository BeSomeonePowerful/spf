#ifndef IDLE_TASK_H
#define IDLE_TASK_H

#include "common.h"

#define REG_TIMER 1

class IdleTask:public TAbstractTask
{
public:
  IdleTask();
  virtual ~IdleTask();
  TAbstractTask *clone() { return (TAbstractTask *)(new IdleTask());}
  BOOL onInit(TiXmlElement *extend);
  void procMsg(std::auto_ptr<TUniNetMsg> msg);
  void onTimeOut(TTimerKey timerKey,void *para);
private:
  TTimerKey regTimer;
  int cnt;
};

#endif
