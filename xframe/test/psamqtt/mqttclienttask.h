#ifndef MQTT_CLIENT_TASK_H
#define MQTT_CLIENT_TASK_H

#include "CommonTask.h"
#define CON_SERVER 4
class CMqttClientTcp;
//potential problem
//how do we manage the messages send between tasks
class MqttClientTask:public CommonTask
{
public:
  MqttClientTask();
  virtual ~MqttClientTask();

  TAbstractTask *clone() { return (TAbstractTask*)(new MqttClientTask());}
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

  CStr remoteIp;
  int remotePort;
  CStr clientName;
  CStr userName;
  CStr password;
  int keepalive;
};

#endif
