#ifndef SUB_SELF_TASK_H
#define SUB_SELF_TASK_H

#include "common.h"
#define GEN_TIMER 1

class SubSelfTask:public TAbstractTask
{
public:
  SubSelfTask();
  virtual ~SubSelfTask();
  TAbstractTask *clone() { return (TAbstractTask *)(new SubSelfTask()); }
  BOOL onInit(TiXmlElement *extend);
  void procMsg(std::auto_ptr<TUniNetMsg> msg);

  void sendToTask(TUniNetMsg *msg);
  void onTimeOut(TTimerKey timerKey,void *para);
private:
  //generate pub message on a regular interval
  TTimerKey generateTimer;
  int msgId;
};

#endif
