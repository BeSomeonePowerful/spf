#ifndef UAID_RESOLVE_SM_H
#define UAID_RESOLVE_SM_H

#include "../mobstatemachine.h"

class MainSatMock;
class UaidResolveSm:public MobStateMachine
{
public:
  UaidResolveSm(MainSatMock *_task);
  virtual ~UaidResolveSm();
  int hasDone() { return done; }
  void onRecv(TUniNetMsg *msg);
  void onSend(TUniNetMsg *msg);
  //void setTask(TAbstractTask *_task) { t=_task; task=(MainSatMock *)_task; }
public:
  enum State { INIT_STATE=0,RECV_GET_UAID,SEND_GET_UAID,RECV_GETACK_UAID,END_STATE};
  State curState;
  int done;
  TUniNetMsg *msg;

  //TAbstractTask *t;
  MainSatMock *task;
  //TAbstractTask *task;

  //ipport of the one starts the uaid resolve process
  CStr ipport;
};

#endif
