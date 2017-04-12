/* mqttlink.h, by Zhang Haibin, 2015-05-26
 */
#ifndef _MQTTLINK_H_
#define _MQTTLINK_H_

#include "comtypedef_vchar.h"
#include "comserv/comtypedef.h"
//commented out by wangpan
// #include "msgdef_uninet.h"
#include "unihashtable.h"
#include "clist.h"
#include "mqttmsg.h"
#include "CommonTask.h"
// #include "CMqttTCPClientTask.h"

#define MSGTYPE_MQTT 1023

class CMqtt;

// a link only responsible for sending and receiving
//not for processing
//processing handled by client

//link interfaces with whom
//
class CMqttLink
{
public:
   CMqttLink();
   static void traceSend(const char* content, int length, const char* ip, int port);
   static void traceReceive(const char* content, int length, const char* ip, int port);

//what does procAppMsg do
//only modify some interfaces
//or build decorator on top of it

//which module call procAppMsg
//passed in what kind of msg

  //should we setup a timer
  //call sendPublish or sendConnAck according to msg->msgName

  //problems might arise in this method
  //at the bottom of call chain there is a method called send with two args
  //but can't find the declaration of this method currently
  //thus,if you compile this,chances are great that you could run into a error

  //send corresponding messages according to msg received
   virtual BOOL procAppMsg(TUniNetMsg* msg)=0;
   void setParent(CMqtt* parent);

   virtual int getLinkId() { return 0; }
   void printAddrMap(CStr& str);

   virtual void secAction();
   virtual void closeLink()=0;
   virtual bool alive() { return mAlive; }

   //which module calls msgParse
   //this module call msgProcess //msgParseStream also call msgProcess
   //which call processConnect or processSub or else
   //which in turn call invokeService or reponseService // such as processUnsubAck call later method

   //which in turn call CUniframe::sendMsgToFEAM (paras passed into this method is of
   //type TUniNetMsg*)
   int msgParse(const char* content, int length);

   //this method originally is a protected method
   //now changed to a public method
   void sendPublish(CStr& topic, CStr& content);

   virtual void wantToSend(shared_ptr<TUniNetMsg> msg) {}
   virtual TTimerKey setTimer(TTimeMark timerId,void *para=NULL) { return task->setTimer(timerId,para); }
   virtual BOOL delTimer(TTimerKey timerKey) { return task->delTimer(timerKey); }
   virtual void protocolLogic(std::auto_ptr<TUniNetMsg> unimsg);
   void setTask(CommonTask *p) {task=p;}
protected:

  //was called by processReceived in CMqttServerTcpLink
   int msgParseStream(const char* content, int length);

   virtual void sendPacket(CStr& packet);


   CMqtt* mParent;
   void invokeService(const char* packetName, int msgId, TMsgBody* msgBody);
   void responseService(int msgId, TMsgBody* msgBody);

   int newPacketId(TMsgAddress& addr);
   void freePacketId(int packetId);
   bool getAppAddr(int packetId, TMsgAddress& addr);

   int mKeepAlive;
   int mIdleSec;
   bool mAlive;

   CommonTask *task;
private:
   CStr mBufferMsg;

   // return -1, error detected
   // return -2, msg not complete
   int msgParseLength(const char*& content, int& length);

   virtual void msgProcess(int packetType, int flags, const char* p, int length);

   int mCurrentPacketId;
   CHashTable<int, TMsgAddress> mAddrMap;

   //pointer to task,used to call protocolLogic method
};

class CMqttServerLink : public CMqttLink
{
public:
   void msgProcess(int packetType, int flags, const char* p, int length);
   void processConnect(const char* p, int length);
   void processDisconnect();
   void processSubscribe(const char* p, int length);
   void processUnsubscribe(const char* p, int length);
   void processPublish(const char* p, int length, int flags);
   void sendConnAck(bool sessionPresent, int code);
   void sendSubAck(int msgId, int code);
   void sendUnsubAck(int msgId);
   void sendPingResp();

protected:
   CStr mClientName;

public:
   CMqttServerLink();
   BOOL procAppMsg(TUniNetMsg* msg);

   //what do secAction do
   void secAction();
   virtual void closeLink();
};

class CMqttClientLink : public CMqttLink
{
public:
   void msgProcess(int packetType, int flags, const char* p, int length);
   void sendSubscribe(CList<CStr>& topics, int packetId);
   void sendUnsubscribe(CList<CStr>& topics, int packetId);
   void sendPingReq();
   void sendDisconnect();

   void processSubAck(const char* p, int length);
   void processUnsubAck(const char* p, int length);
   void processConnAck(const char* p, int length);
   void processPublish(const char* p, int length, int flags);
   int mPingTimer;
public:
   CMqttClientLink();
   void sendConnect(CStr& name, CStr& user, CStr& passwd, int keepAlive);
   BOOL procAppMsg(TUniNetMsg* msg);
   void secAction();
   virtual void closeLink();
};

#endif
