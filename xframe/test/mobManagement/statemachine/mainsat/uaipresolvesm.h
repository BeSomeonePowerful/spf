#ifndef UAIP_RESOLVE_SM_H
#define UAIP_RESOLVE_SM_H

#include "msgdef_com.h"
#include "../mobstatemachine.h"

class MainSatMock;
class UaipResolveSm:public MobStateMachine
{
public:
  UaipResolveSm(MainSatMock *_task);
  virtual ~UaipResolveSm();
  int hasDone() {return done; }
  void onRecv(TUniNetMsg *msg);
  void onSend(TUniNetMsg *msg);
  //void setTask(MainSatMock *_task) { task=_task; }
public:
  enum State { INIT_STATE=0,RECV_GET_UAIP,SEND_GET_UAIP,RECV_GETACK_UAIP,END_STATE};
  State curState;
  int done;
  TUniNetMsg *msg;
  //TAbstractTask *task;
  MainSatMock *task;
  CStr ipport;
};

#endif
