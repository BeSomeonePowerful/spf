#ifndef ADDR_RESOLVE_SM_H
#define ADDR_RESOLVE_SM_H

#include "../mobstatemachine.h"
#include "msgdef_com.h"

class MicroSatMock;

class AddrResolveSm:public MobStateMachine
{
public:
  AddrResolveSm(MicroSatMock *_task,int _userid);
  virtual ~AddrResolveSm();
  int hasDone() {return done; }
  void onRecv(TUniNetMsg *msg);
  void onSend(TUniNetMsg *msg);
//  void setTask(TAbstractTask *_task) { task=_task; }
public:
  enum State{ INIT_STATE=0,SEND_GET_SATIP0,RECV_GETACK_SATIP0,SEND_GET_UAID,RECV_GETACK_UAID,
    SEND_GET_SATIP1,RECV_GETACK_SATIP1,SEND_GET_UAIP,END_STATE};
  State curState;
  int done;
  TUniNetMsg *msg; //cache of previous message that may be used in latter

  //TAbstractTask *t;
  MicroSatMock *task;
  //TAbstractTask *task;
  int userid; //what type is userid
};

#endif
