#ifndef SAT_REG_SM_H
#define SAT_REG_SM_H

class SatRegSm:public MobStateMachine
{
public:
  int hasDone();
  void onRecv(TUniNetMsg *msg);
  void onSend(TUniNetMsg *msg);
  void setTask(TAbstractTask *_task) { task=_task; }
public:
  enum State {INIT_STATE=0,SEND_CON,END_STATE};
  State curState;
  int done;
  TAbstractTask *task;
};

#endif
