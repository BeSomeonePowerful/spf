#ifndef SELF_TASK_H
#define SELF_TASK_H

class SelfTask:public TAbstractTask
{
public:
  SelfTask();
  virtual ~SelfTask();
  TAbstractTask *clone() { return (TAbstractTask *)(new SelfTask()); }
  BOOL onInit(TiXmlElement *extend);
  void procMsg(std::auto_ptr<TUniNetMsg> msg);

  void onTimeOut(TTimerKey timerKey,void *para);
private:
  //generate pub message on a regular interval
  TTimerKey generateTimer;
};

#endif
