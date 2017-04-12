#ifndef MQTT_SERVER_TASK_H
#define MQTT_SERVER_TASK_H
//when a message is received from network
//keepalive timer should be acounted too
//1. processReceive method of link being called to receive the message
//2. then message being parsed into mqtt message
//3. let the message go through state machine;if the return-code,a response can be made
//immediately,then response,otherwise forward to appplication-task.whenever we send or
//receive a message ,we should set the keepaliveTimer  (para of onTimeOut?what is it??)
//whenever we send a message ,if it need a response,we set resend timer and timerOuttimer

//how do we distribute message to task flexibly? using message type?

//(is linkid a unique number?when will it get reused?)
//when a timer is timeOut,we need to know which link it belongs
//and the state machine related to this link

//when send a msg in server,it needs to go through state machine,too
//It's an error to send some msg in some state. Need to report error to app.
#include "CommonTask.h"
class CMqttServerTcp;

class MqttServerTask:public CommonTask
{
public:
  MqttServerTask();
  virtual ~MqttServerTask();

  TAbstractTask *clone() { return (TAbstractTask*)(new MqttServerTask());}
  virtual BOOL onInit(TiXmlElement *extend);
  void sendToTask(TUniNetMsg *msg,int linkid);
  void procPub(TUniNetMsg *msg);
  void procSub(TUniNetMsg *msg,int linkid);
  void procUnsub(TUniNetMsg *msg,int linkid);
  void procMsg(std::auto_ptr<TUniNetMsg> msg);
  void onTimeOut(TTimerKey timerKey,void *para);
  virtual void onBuildFDSet(CFdSet &fdset);
  virtual void onCheckFDSet(CFdSet &fdset);
private:
  // map<TTimerKey,int> timerDict;
  // map<int,MqttClientStateMachine> smDict;
  map<CStr,set<int> > subDict;
  CMqttServerTcp *mqtt;
  CStr localIP;
  int localPort;
};

#endif
