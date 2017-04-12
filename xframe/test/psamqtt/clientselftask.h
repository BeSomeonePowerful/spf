#ifndef CLIENT_SELF_TASK_H
#define CLIENT_SELF_TASK_H

#include "common.h"
#define GEN_TIMER 1

class ClientSelfTask:public TAbstractTask
{
public:
  ClientSelfTask();
  virtual ~ClientSelfTask();
  TAbstractTask *clone() { return (TAbstractTask *)(new ClientSelfTask()); }
  BOOL onInit(TiXmlElement *extend);
  void procMsg(std::auto_ptr<TUniNetMsg> msg);

  void sendToTask(TUniNetMsg *msg);
  void onTimeOut(TTimerKey timerKey,void *para);
private:
  //generate pub message on a regular interval
  TTimerKey generateTimer;
  int cnt;
};

#endif
