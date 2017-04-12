#ifndef COMMON_TASK_H
#define COMMON_TASK_H

#include "common.h"
class CommonTask:public TPSATask
{
public:
  CommonTask();
  virtual ~CommonTask();
  TAbstractTask *clone() { return (TAbstractTask*)(new CommonTask());}
  virtual BOOL onInit(TiXmlElement *extend);
  void procMsg(std::auto_ptr<TUniNetMsg> msg);
  void onTimeOut(TTimerKey timerKey,void *para);
  virtual void onBuildFDSet(CFdSet &fdset);
  virtual void onCheckFDSet(CFdSet &fdset);

  //method used in client
  virtual void sendToTask(TUniNetMsg *msg) {}
  virtual void sendToTask(TUniNetMsg *msg,int linkid) {}
  virtual void procPub(TUniNetMsg *msg) {}
  virtual void procSub(TUniNetMsg *msg,int linkid) {}
  virtual void procUnsub(TUniNetMsg *msg,int linkid) {}
};

#endif
