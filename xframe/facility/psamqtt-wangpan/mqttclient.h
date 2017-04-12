/* mqttclient.h, by Zhang Haibin, 2015-05-25
 */
#ifndef _MQTTCLIENT_H_
#define _MQTTCLIENT_H_

#include "fdset.h"
#include "comtypedef_vchar.h"
#include "mqttlink.h"
#include "socket.h"
#include "mqtt.h"

class MqttClientStateMachine;

class CMqttClient : public CMqtt
{
public:
   virtual void init(CStr& ip, int port, CStr& name, CStr& user, CStr& passwd, int keepAlive);
   virtual void close();
   virtual void list(CStr& str);
   virtual void buildFdSet(CFdSet& fdSet);
   virtual void procFdSet(CFdSet& fdSet);

protected:
   CStr mDestIp;
   int mDestPort;
   CStr mName;
   CStr mUser;
   CStr mPasswd;
   int mKeepAlive;
};

//how to modify
//sending a basic message back and forth using current architecture

//need to get to know why having a class of This kind
//the relation is rather obscure
class CMqttClientTcpLink : public CTcpConnection, public CMqttClientLink
{
public:
  // #define INIT_STATE 1
  // #define WAIT_CONACK_STATE 2
  // #define END_STATE 3
  // #define WAIT_PINGRSP_STATE 4
  // #define CONNECTED_STATE 5
  // #define WAIT_SUBACK_STATE 6
  // #define WAIT_UNSUBACK_STATE 7
   CMqttClientTcpLink();
   ~CMqttClientTcpLink();
   void sendPacket(CStr& packet);
   int processReceived(const char* content, int length);
   void closeLink();
   bool alive();

   virtual int needSend() {return 1;}

   //msgtype,msgarg
   void wantToSend(shared_ptr<TUniNetMsg> msg);
   void onTimeOut(TTimerKey timerKey,void *para);
   void onReceivedMessage(CCode code);
   //we see if we need to override needsend function
   virtual void processSend();
  //  {
  //    //task->processSend(getLinkId());
  //  }
   virtual int processRecv();
   void protocolLogic(std::auto_ptr<TUniNetMsg> unimsg);
  //  {
  //    //return task->processReceive(getLinkId());
  //  }
 private:

   //init of keepAlive while sending
   //after send an msg,always setup keepalive timer

   //after receiving ,always del keepalive timer
   //to make things simple currently
   //doesn't concern with keepalive timer

   //timers
  //  TTimerKey conackTimer;
  //  TTimerKey subackTimer;
  //  TTimerKey unsubackTimer;
  //  TTimerKey pingrspTimer;

   //int packetId;
   //static const int MAXPACKETID = 65536;
  //  int state;

   MqttClientStateMachine *stateMachine;
   queue<std::shared_ptr<TUniNetMsg> > wantsToSend;
   CStr mRecvBuffer;
  //  map<int,shared_ptr<TMsgArg> > argDict;
};

//what interface should a client or a link provide
//a client,like a task,should be running in a thread ,thus responsible for
//managing all the links
//what to do onFdSet

//the links generally have nothing to do with a fdset
//it's called by client when fdset is ready

//a udp client is able to connect to many server
//is mqtt tcp based or udp based or unspecified

//asynchornous message handling framework and
//synchronous message handling framework
class CMqttClientTcp : public CMqttClient
{
public:
   void init(CStr& ip, int port, CStr& name, CStr& user, CStr& passwd, int keepAlive);
   void close();
   void list(CStr& str);
   void buildFdSet(CFdSet& fdSet);
   void procFdSet(CFdSet& fdSet);

   void secAction();
   void closeLink(int);
   void reconnect();

   void wantToSend(shared_ptr<TUniNetMsg> msg);
   void onTimeOut(TTimerKey timerKey,void *para);
   CMqttClientTcpLink *getLink() { return &mLink; }
private:
   CMqttClientTcpLink mLink;
   CMqttLink* getLink(int id);
};

class CMqttClientUdpLink : public CMqttClientLink
{
public:
   CMqttClientUdpLink();
   void sendPacket(CStr& packet);
   void closeLink();
   void setAddr(CStr& ip, int port);
   void buildFdSet(CFdSet& fdSet);
   void procFdSet(CFdSet& fdSet);
private:
   int mFd;
   CStr mIp;
   int mPort;
};

class CMqttClientUdp : public CMqttClient
{
public:
   void init(CStr& ip, int port, CStr& name, CStr& user, CStr& passwd, int keepAlive);
   void close();
   void list(CStr& str);
   void buildFdSet(CFdSet& fdSet);
   void procFdSet(CFdSet& fdSet);

   void secAction();
   void closeLink(int);
   void reconnect();

private:
   CMqttClientUdpLink mLink;
   CMqttLink* getLink(int id);
};

#endif
