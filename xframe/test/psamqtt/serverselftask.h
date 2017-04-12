#ifndef SERVER_SELF_TASK_H
#define SERVER_SELF_TASK_H

#include "common.h"

class ServerSelfTask:public TAbstractTask
{
public:
  ServerSelfTask();
  virtual ~ServerSelfTask();
  TAbstractTask *clone() { return (TAbstractTask *)(new ServerSelfTask()); }
  BOOL onInit(TiXmlElement *extend);
  void procMsg(std::auto_ptr<TUniNetMsg> msg);
  void sendToTask(TUniNetMsg *msg);
  void onTimeOut(TTimerKey timerKey,void *para);
private:
  //how to process sub and pub message
  //when a pub message is received in a link,first ctrl is transfered to
  //task
  //mqttservertask need to be improved
};

#endif
