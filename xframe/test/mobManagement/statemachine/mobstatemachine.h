#ifndef MOB_STATE_MACHINE_H
#define MOB_STATE_MACHINE_H

#include "msgdef_com.h"

class MobStateMachine
{
public:
  virtual int hasDone()=0;
  virtual void onRecv(TUniNetMsg *msg)=0;
  virtual void onSend(TUniNetMsg *msg)=0;
  virtual ~MobStateMachine() {}
  //virtual void setTask(TAbstractTask *_task)=0;
protected:
  //TAbstractTask *task;
};

#endif
