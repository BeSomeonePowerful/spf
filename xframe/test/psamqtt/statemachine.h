#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include "common.h"
class CMqttServerLink;
class CMqttClientLink;

#define RESEND_TIMER 2
#define ALIVE_TIMER 3
//change all TUniNetMsg * to shared_ptr<TUniNetMsg>
//complete the coding of SelfTask,clientSelfTask,serverSelfTask
//clientSelfTask: generating pub message
//serverSelfTask: dealing with message,responding acks

enum CLIENT_SM_STATE {CLIENT_INIT_STATE=0,WAIT_CONACK_STATE,CLIENT_CONNECTED_STATE,WAIT_PINGRESP_STATE,
  WAIT_SUBACK_STATE,WAIT_UNSUBACK_STATE,CLIENT_END_STATE};
enum CLIENT_SM_CODE {CLIENT_SEND_REFUSE=0,CLIENT_SEND_FORWARD,CLIENT_RECV_REFUSE,
  CLIENT_RECV_FORWARD,CLIENT_TIMEOUT_REFUSE,CLIENT_TIMEOUT_FORWARD};

class MqttClientStateMachine
{
public:
  MqttClientStateMachine();
  void setLink(CMqttClientLink *_link) { link=_link; }
  CLIENT_SM_CODE onSend(shared_ptr<TUniNetMsg> msg);
  CLIENT_SM_CODE onRecv(TUniNetMsg* msg);
  CLIENT_SM_CODE onTimeOut(TTimerKey timerKey,void *para,shared_ptr<TUniNetMsg> &resend);
private:
  shared_ptr<TUniNetMsg> msg;
  CLIENT_SM_STATE curState;
  CLIENT_SM_STATE preState;
  int smChange;
  TTimerKey resendTimer;

  //when to init the keepalive timer
  //after connected if for a while has no message being send or receive then then
  //the keepalive timer will timeOut
  TTimerKey keepaliveTimer;
  int resendCnt; //default to 0
  CMqttClientLink *link;
};

enum SERVER_SM_STATE {  SERVER_INIT_STATE=0,RECV_CON_STATE,RECV_SUB_STATE,RECV_UNSUB_STATE,RECV_PINGREQ_STATE,
  SERVER_CONNECTED_STATE,SERVER_END_STATE};
enum SERVER_SM_CODE {SERVER_SEND_REFUSE,SERVER_SEND_FORWARD,SERVER_RECV_REFUSE,
  SERVER_RECV_FORWARD};
class MqttServerStateMachine
{
public:
  MqttServerStateMachine();
  void setLink(CMqttServerLink *_link) { link=_link; }
  SERVER_SM_CODE onSend(shared_ptr<TUniNetMsg> msg);
  SERVER_SM_CODE onRecv(TUniNetMsg *msg);
  //SERVER_SM_CODE onTimeOut(TTimerKey timerKey,void *para);
private:
  std::shared_ptr<TUniNetMsg> msg;
  SERVER_SM_STATE curState;
  SERVER_SM_STATE preState;
  int smChange;

  // TTimerKey resendTimer;

  //when to init the keepalive timer
  //after connected if for a while has no message being send or receive then then
  //the keepalive timer will timeOut
  // TTimerKey keepaliveTimer;
  // int resendCnt; //default to 0
  CMqttServerLink *link;
};

#endif
