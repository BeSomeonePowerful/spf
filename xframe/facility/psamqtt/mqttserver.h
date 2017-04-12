/* mqttserver.h, by Zhang Haibin, 2015-05-25
 */
#ifndef _MQTTSERVER_H_
#define _MQTTSERVER_H_

#include "comtypedef_vchar.h"
#include "fdset.h"
#include "socket.h"
#include "mqttlink.h"
#include "mqtt.h"

class CMqttServer : public CMqtt
{
public:
   CMqttServer();
   //class-specific methods,not inherited from
   virtual void init(CStr& ip, int port);

   //
   virtual void close();
   virtual void list(CStr& str);
   virtual void buildFdSet(CFdSet& fdSet);
   virtual void procFdSet(CFdSet& fdSet);

protected:
   CStr mLocalIp;
   int mLocalPort;
};


class CMqttServerTcp : public CMqttServer
{
public:
   CMqttServerTcp();
   void init(CStr& ip, int port);
   void close();
   void list(CStr& str);
   void buildFdSet(CFdSet& fdSet);
   void procFdSet(CFdSet& fdSet);

   void secAction(){}
   void closeLink(int linkId);

   //added by wangpan
   CTcpConnectionManager *getTCPManager() { return &mLinks;}
private:
   CTcpConnectionManager mLinks;
   CTcpListener mListener;
   int mAcceptCount;

   CMqttLink* getLink(int id);
};

class CMqttServerUdpLink : public CMqttServerLink
{
public:
   CMqttServerUdpLink();
   void setInfo(int fd, const char* ip, int port);
   bool matchClient(const char* ip, int port);
   bool matchLinkId(int id) { return mLinkId == id; }
   void printState(CStr& str);
   static void printTitle(CStr& str);
   void sendPacket(CStr& packet);
private:
   int getLinkId() { return mLinkId; }
   void closeLink();
   int mFd;
   CStr mClientIp;
   int mClientPort;
   int mLinkId;
public:
   int mReceivedCount;
   int mSentCount;
};

class CMqttServerUdp : public CMqttServer
{
public:
   CMqttServerUdp();
   void init(CStr& ip, int port);
   void close();
   void list(CStr& str);
   void buildFdSet(CFdSet& fdSet);
   void procFdSet(CFdSet& fdSet);

   //we make this useless for compile reasons
   void secAction();

private:
   int mFd;
   CList<CMqttServerUdpLink*> mLinks;

   CMqttLink* getLink(int id);
};


class CMqttServerTcpLink : public CTcpConnection, public CMqttServerLink
{
public:
   #define INIT_STATE 1
   #define RECV_CON_STATE 2
   #define RECV_SUB_STATE 3
   #define RECV_UNSUB_STATE 4
   #define RECV_PINGREQ_STATE 5
   #define CONNECTED_STATE 6
   #define END_STATE 7
   virtual int needSend() {return 1;}
   CMqttServerTcpLink();
   bool alive();
   virtual void processSend();
  //  {
  //    task->processSend(getLinkId());
  //  }
   virtual int processRecv();
  //  {
  //    return task->processReceive(getLinkId());
  //  }
   void wantToSend(TestMessage &msg);
   void onReceivedMessage(CCode code);
private:
   int processReceived(const char* content, int length);
   void sendPacket(CStr& packet);
   int getLinkId() { return linkId(); }
   void closeLink();
   void closeProcess();
   queue<TestMessage> wantsToSend;
};

#endif
