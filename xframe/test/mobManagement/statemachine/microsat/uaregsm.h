#ifndef UA_REG_SM_H
#define UA_REG_SM_H

#include "../mobstatemachine.h"
#include "msgdef_com.h"

class MicroSatMock;
class UaRegSm:public MobStateMachine
{
public:
  UaRegSm(MicroSatMock *task);
  virtual ~UaRegSm();
  int hasDone() { return done; }
  void onRecv(TUniNetMsg *msg);
  void onSend(TUniNetMsg *msg);
private:
  enum State {INIT_STATE=0,SEND_CON,END_STATE};
  State curState;
  int done;
  MicroSatMock *task;
};
#endif
