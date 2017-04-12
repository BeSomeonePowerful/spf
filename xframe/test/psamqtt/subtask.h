#ifndef SUB_TASK_H
#define SUB_TASK_H

//the subtask is a client connected to server
//it only sends sub messages regularly or once
#include "CommonTask.h"
#define CON_SERVER 4
#define SUB_GEN_TIMER 5
class CMqttClientTcp;
//potential problem
//how do we manage the messages send between tasks
class SubTask:public CommonTask
{
public:
  SubTask();
  virtual ~SubTask();

  TAbstractTask *clone() { return (TAbstractTask*)(new SubTask());}
  virtual BOOL onInit(TiXmlElement *extend);
  void sendToTask(TUniNetMsg *msg);
  void procMsg(std::auto_ptr<TUniNetMsg> msg);
  void onTimeOut(TTimerKey timerKey,void *para);
  virtual void onBuildFDSet(CFdSet &fdset);
  virtual void onCheckFDSet(CFdSet &fdset);
private:
  // map<TTimerKey,int> timerDict;
  // map<int,MqttClientStateMachine> smDict;
  CMqttClientTcp *mqtt;

  TTimerKey connServerTimer;
  // TTimerKey subGenTimer;
  // int msgId;

  CStr remoteIp;
  int remotePort;
  CStr clientName;
  CStr userName;
  CStr password;
  int keepalive;
};

#endif
