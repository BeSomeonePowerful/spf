#ifndef _PSAMQTT_TASK_H
#define _PSAMQTT_TASK_H

#include "socket.h"
#include "clist.h"
#include "comtypedef.h"
#include "psatask.h"
#include "msgdef_com.h"
#include <queue>
#include <iostream>
#include "mqttmsg_arg.h"

using std::cout;
using std::endl;
using std::queue;

#define ACK_TIMER 1
#define GEN_TIMER 2


class CMqtt;
class CommonTask:public TPSATask
{
public:
  virtual ~CommonTask();
  TAbstractTask *clone()=0;
  BOOL onInit(TiXmlElement *extend)=0;
  void procMsg(std::auto_ptr<TUniNetMsg> msg)=0;

  void onTimeOut(TTimerKey timerKey,void *para);
  void onBuildFDSet(CFdSet &fdset);
  void onCheckFDSet(CFdSet &fdset);
  void setMqtt(CMqtt *m);
protected:
  CMqtt *mqtt;
};
#endif
