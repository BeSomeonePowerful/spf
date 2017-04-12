#ifndef USER_REG_SM_H
#define USER_REG_SM_H

#include "../mobstatemachine.h"

class MainSatMock;
class UserRegSm:public MobStateMachine
{
public:
  int hasDone();
  void onRecv(TUniNetMsg *msg);
  void onSend(TUniNetMsg *msg);
  void setTask(TAbstractTask *_task) { task=_task; }
public:
  enum State{ INIT_STATE=0,RECV_REG,SEND_GET,RECV_GETACK,FORWARD_REG,RECV_REGACK,END_STATE};
  State curState;
  int done;
  TUniNetMsg *msg; //cache of previous message that may be used in latter
  TAbstractTask *task;
};

#endif
