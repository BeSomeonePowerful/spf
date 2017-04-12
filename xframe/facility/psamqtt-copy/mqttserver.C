/* mqttserver.C, by Zhang Haibin, 2015-05-25
 */
#include <errno.h>

#include "mqttserver.h"
#include "msgdef_mqtt.h"
//commented out by wangpan
// #include "msglib_inter_mqtt.h"
#include "mqttmsg.h"

CMqttServer::CMqttServer()
{
}

void CMqttServer::init(CStr& ip, int port)
{
   mLocalIp = ip;
   mLocalPort = port;
}

void CMqttServer::buildFdSet(CFdSet& fdSet)
{
}

void CMqttServer::procFdSet(CFdSet& fdSet)
{
}

void CMqttServer::close()
{
}

void CMqttServer::list(CStr& str)
{
}

CMqttServerTcp::CMqttServerTcp()
{
   mAcceptCount = 0;
}

void CMqttServerTcp::init(CStr& ip, int port)
{
   CMqttServer::init(ip, port);
   mListener.open((char *)mLocalIp.c_str(), mLocalPort);
}

void CMqttServerTcp::buildFdSet(CFdSet& fdSet)
{
   //cout << "CMqttServerTcp::buildFdSet method being called" << endl;
   mListener.buildFdSet(fdSet);
   mLinks.buildFdSet(fdSet);
}

void CMqttServerTcp::procFdSet(CFdSet& fdSet)
{
   //cout << "CMqttServerTcp::procFdSet method being called" << endl;
   if(mListener.accept(fdSet))
   {
      mAcceptCount++;
      CMqttServerTcpLink* link = new CMqttServerTcpLink();
      mListener.dupConnection(link);
      link->linkId() = mAcceptCount;
      link->setParent(this);
      link->setTask(task);
      mLinks.addConnection(link);
   }
   mLinks.procFdSet(fdSet);
}

void CMqttServerTcp::close()
{
}

void CMqttServerTcp::closeLink(int linkId)
{
   mLinks.closeByLinkId(linkId);
}

void CMqttServerTcp::list(CStr& str)
{
   mListener.serverInfo(&str);
   str.fCat(", accept %d\n\n", mAcceptCount);
   str << "TYPE   FD    SEND     RECEIVE  RBUFFER  LINKID REMOTE-ADDR\n";
   mLinks.printConnectionsState(str);
   listServiceConfig(str);
}

CMqttLink* CMqttServerTcp::getLink(int id)
{
   return (CMqttServerTcpLink*)(mLinks.findByLinkId(id));
}

//when to call this secAction
//which module call this secAction

//it seems that this method should be called by keepAlive Timer when time is out
// void CMqttServerTcp::secAction()
// {
//    mLinks.reset();
//    CMqttServerTcpLink* link;
//    while((link = (CMqttServerTcpLink*)mLinks.next()) != NULL)
//    {
//       link->secAction();
//    }
// }

CMqttServerUdp::CMqttServerUdp()
{
   mFd = -1;
}

void CMqttServerUdp::init(CStr& ip, int port)
{
   CMqttServer::init(ip, port);
   mFd = socket(AF_INET, SOCK_DGRAM, 0);
   if(mFd < 0)
   {
      UniERROR("CMqttServerUdp::init, socket() failed: %s", strerror(errno));
      return;
   }
   UniINFO("udp socket initialized, fd = %d", mFd);

   struct sockaddr_in addr;
   addr.sin_family = AF_INET;
   addr.sin_port = htons(port);
   addr.sin_addr.s_addr = INADDR_ANY;
   memset(&(addr.sin_zero), 0, 8);

   if (bind(mFd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) != 0)
   {
      UniERROR("CMqttServerUdp::init, bind() failed: %s", strerror(errno));
      return;
   }
   UniINFO("udp socket bind to port %d", port);
}

void CMqttServerUdp::buildFdSet(CFdSet& fdSet)
{
   if(mFd > 0)
      fdSet.setRead(mFd);
}

void CMqttServerUdp::procFdSet(CFdSet& fdSet)
{
   if(mFd > 0 && fdSet.readyToRead(mFd))
   {
      struct sockaddr_in clientAddr;
      socklen_t addrLen = sizeof(struct sockaddr);
      char buffer[65536];
      int length = recvfrom(mFd, buffer, sizeof(buffer), 0, (struct sockaddr *)&clientAddr, &addrLen);
      if(length <= 0)
      {
         UniERROR("CMqttClientUdpLink::procFdSet, recvfrom() failed: %s", strerror(errno));
         return;
      }
      CMqttLink::traceReceive(buffer, length, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
      CMqttServerUdpLink* link = NULL;
      if(CMqttMsg::isPacket(buffer, MQTT_P_CONNECT))
      {
         link = new CMqttServerUdpLink;
         link->setParent(this);
         link->setInfo(mFd, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
         mLinks.push_back(link);
      }
      else
      {
         for(CList<CMqttServerUdpLink*>::iterator i = mLinks.begin(); i != mLinks.end(); i++)
            if((*i)->matchClient(inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port)))
            {
               link = (*i);
               break;
            }
         if(link == NULL)
         {
            UniERROR("can not find link for %s:%d", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
            return;
         }
      }
      link->mReceivedCount++;
      link->msgParse(buffer, length);
   }
}

void CMqttServerUdp::close()
{
   if(mFd > 0)
      ::close(mFd);
   mFd = -1;
  //  mLinks.deleteAndClear();
  //modified by wangpan
  mLinks.clear();
}

CMqttLink* CMqttServerUdp::getLink(int id)
{
   for(CList<CMqttServerUdpLink*>::iterator i = mLinks.begin(); i != mLinks.end(); i++)
      if((*i)->matchLinkId(id))
         return (*i);
   return NULL;
}

void CMqttServerUdp::list(CStr& str)
{
   str.fCat("Port = %d, FD = %d\n\n", mLocalPort, mFd);
   str.fCat("--- Links ---\n");
   CMqttServerUdpLink::printTitle(str);
   for(CList<CMqttServerUdpLink*>::iterator i = mLinks.begin(); i != mLinks.end(); i++)
      (*i)->printState(str);
   listServiceConfig(str);
}

void CMqttServerUdp::secAction()
{
   for(CList<CMqttServerUdpLink*>::iterator i = mLinks.begin(); i != mLinks.end(); i++)
   {
      (*i)->secAction();
   }
}

CMqttServerTcpLink::CMqttServerTcpLink()
{
}

int CMqttServerTcpLink::processReceived(const char* content, int length)
{
   //traceReceive(content, length, remoteIp(), remotePort());
   return msgParseStream(content, length);
}

void CMqttServerTcpLink::sendPacket(CStr& packet)
{
   //traceSend(packet.c_str(), packet.length(), remoteIp(), remotePort());
   CCode code;
   char buf[1024];
   strncpy(buf,packet.c_str(),packet.length());
   code.content=buf;
   code.length=packet.length();
   send(code);
}

void CMqttServerTcpLink::closeLink()
{
   close();
}

bool CMqttServerTcpLink::alive()
{
   return mAlive && !isSocketStateClose();
}

void CMqttServerTcpLink::processSend()
{
  UniINFO("ServerLink::processSend method being called");
  // cout << "ServerLink::processSend method being called" << endl;
  if( wantsToSend.empty() ) return;
  UniINFO("ServerLink::processSend method being called with data" );
  // cout << "ServerLink::processSend method being called with data" << endl;
  TestMessage msg = wantsToSend.front();
  wantsToSend.pop();

  // CStr str = msg.getStr();
  CCode code;
  char buf[1024];

  int len = msg.msgContent.length();
  strncpy(buf,msg.msgContent.c_str(),len);
  buf[len]='\0';
  code.content = buf;
  code.length = len;

  UniINFO("##from task: %d, %s",task->getTaskId(),buf);
  // cout << "##from task: " << task->getTaskId() << ", " << buf << endl;
  int re=send(code);
  UniINFO("ServerLink::processSend after send: %d",re);
  //cout << "ServerLink::processSend after send: " << re << endl;
  //hasSent.push(msg);
  //setUpAckTimer();
}

int CMqttServerTcpLink::processRecv()
{
  UniINFO("ServerLink::processRecv method being called");
  // cout << "ServerLink::processRecv method being called" << endl;
  CCode code;
  char buf[1024];
  code.content=buf;
  code.length=0;

  int recvstatus = receive(code);
  buf[code.length]='\0';
  if( recvstatus )
  {
    //TestMessage msg = TestMessage::parse(code.content,code.length);
    //report receive
      // onReceivedMessage(code);
      // //is code.content a null-terminated string
      // TestMessage msg;
      // msg.msgContent << code.content;
      // msg.msgContent << ":";
      // msg.msgContent << task->getTaskId();
      // msg.msgContent << " received:" ;
      // msg.msgContent << linkId();
      // wantToSend(msg);

     msgParse(code.content,code.length);
    //delAckTimer();
    //hasSent.pop();
    //received.push(msg);
    return 1;
  }
  else
    UniERROR("Error in receive func");
  return 0;
}

//in log we have to print out linkid
void CMqttServerTcpLink::protocolLogic(std::auto_ptr<TUniNetMsg> unimsg)
{
  TUniNetMsg *msg = unimsg.get();
  switch( msg->msgName )
  {
    case MQTT_P_CONNECT:
      {
        if( state!=INIT_STATE )
        {
          UniERROR("received a connect message while state not in INIT_STATE: linkid: %d",linkId());
          break;
        }
        TMqttConnect *arg = (TMqttConnect *)msg->msgBody;

        UniINFO("received a connect message: linkid: %d,name: %s,user: %s,passwd: %s",linkId(),
          arg->clientName.c_str(),arg->userName.c_str(),arg->password.c_str());
        TestMessage msg;
        msg.msgType = MQTT_P_CONNACK;
        msg.arg = shared_ptr<TMsgArg>(new TConAckArg);
        TConAckArg *argt = (TConAckArg *)msg.arg.get();

        //during parsing process,we get the code
        //how do we pass it out of parsing methods
        argt->code = 0;

        wantsToSend.push(msg);
        state = RECV_CON_STATE;
      }
      break;
    case MQTT_P_PUBLISH:
      {
        if( state!=CONNECTED_STATE )
        {
          UniERROR("received a publish message while not in CONNECTED_STATE: linkid: %d",linkId());
          break;
        }
        TMqttPublish *arg = (TMqttPublish *)msg->msgBody;
        UniINFO("received a publish message: linkid: %d,topic: %s,content: %s,clientName: %s",
          linkId(),arg->topic.c_str(),arg->content.c_str(),arg->clientName.c_str());

        TestMessage msg;
        msg.msgType = MQTT_P_PUBLISH;
        msg.arg = shared_ptr<TMsgArg>(new TPubArg);

        TPubArg *argt = (TPubArg *)msg.arg.get();
        argt->topic = arg->topic;
        argt->content = arg->content;

        //server->sendSub(msg);

        //must depend on Server to do the job
        //get list of linkid which subed the topic
        //then call wanttosend method of every link

      }
      break;
    case MQTT_P_SUBSCRIBE:
      {
        if( state!=CONNECTED_STATE )
        {
          UniERROR("received a subscribe message while not in CONNECTED_STATE: linkid: %d",
            linkId());
          break;
        }
        TMqttSubscribe *arg = (TMqttSubscribe *)msg->msgBody;

        UniINFO("received a subscribe message: linkid: %d,clientName: %s,packetId: %d,topic: %s",
          linkId(),arg->clientName.c_str(),arg->msgId,(*(arg->topic.begin())).c_str());

        TestMessage msg;
        msg.msgType = MQTT_P_SUBACK;
        msg.arg = shared_ptr<TMsgArg>(new TSubAckArg);

        TSubAckArg *argt = (TSubAckArg *)msg.arg.get();
        argt->packetId = arg->msgId;

        //how to generate the code
        argt->code = 0;

        wantsToSend.push(msg);
        state = RECV_SUB_STATE;
      }
      break;
    case MQTT_P_UNSUBSCRIBE:
      {
        if( state!=CONNECTED_STATE )
        {
          UniERROR("received a unsubscribe message while not in CONNECTED_STATE: linkid: %d",linkId());
          break;
        }
        TMqttUnsubscribe *arg = (TMqttUnsubscribe *)msg->msgBody;
        UniINFO("received a unsubscribe message: linkid: %d,clientName: %s,packetId: %d,topic: %s",
          linkId(),arg->clientName.c_str(),arg->msgId,(*(arg->topic.begin())).c_str());

        TestMessage msg;
        msg.msgType = MQTT_P_UNSUBACK;
        msg.arg = shared_ptr<TMsgArg>(new TUnsubAckArg);

        TUnsubAckArg *argt = msg.arg.get();
        argt->packetId = arg->msgId;
        argt->code = 0;

        wantsToSend.push(msg);
        state = RECV_UNSUB_STATE;
      }
      break;
      //assuming server-side does not set up keepalive timer
    case MQTT_P_PINGREQ:
      {
        
      }
      break;
    case MQTT_P_DISCONNECT:
      {

      }
      break;
    default:
      UniERROR("invalid msgType received in server side:%d",msg->msgName);
  }
}

void CMqttServerTcpLink::wantToSend(TestMessage &msg)
{
  wantsToSend.push(msg);
}

void CMqttServerTcpLink::onReceivedMessage(CCode code)
{
  UniINFO("###Received in task: %d , %s",task->getTaskId(),code.content);
  // cout << "###Received in task " << task->getTaskId() << ": " << code.content << endl;
}

void CMqttServerTcpLink::closeProcess()
{
   TMqttDisconnect* disconnectArg = new TMqttDisconnect;
   disconnectArg->clientName = mClientName;
   invokeService("disconnect", MQTT_P_DISCONNECT, disconnectArg);
}

CMqttServerUdpLink::CMqttServerUdpLink()
{
   mFd = -1;
   mClientPort = 0;
   mReceivedCount = 0;
   mSentCount = 0;
}

void CMqttServerUdpLink::setInfo(int fd, const char* ip, int port)
{
static int _linkId = 0;
   mLinkId = _linkId++;
   mFd = fd;
   mClientIp = ip;
   mClientPort = port;
}

bool CMqttServerUdpLink::matchClient(const char* ip, int port)
{
   return mClientIp == ip && mClientPort == port;
}

void CMqttServerUdpLink::closeLink()
{
   CMqttServerLink::closeLink();
   mSentCount = 0;
   mReceivedCount = 0;
}

void CMqttServerUdpLink::printTitle(CStr& str)
{
   str << "ID    CLIENT-NAME ALIVE RECEIVED SENT     REMOTE-ADDR\n";
}

void CMqttServerUdpLink::printState(CStr& str)
{
   str.fCat("%-5d %-11s %-5d %-8d %-8d %s:%d\n", mLinkId, mClientName.c_str(), mAlive, mReceivedCount, mSentCount, mClientIp.c_str(), mClientPort);
}

void CMqttServerUdpLink::sendPacket(CStr& packet)
{
   if(mFd < 0)
   {
      UniERROR("CMqttServerUdpLink::sendPacket, fd < 0");
      return;
   }
   struct sockaddr_in remoteAddr;
   remoteAddr.sin_family = AF_INET;
   remoteAddr.sin_addr.s_addr = inet_addr(mClientIp.c_str());
   remoteAddr.sin_port = htons(mClientPort);
   memset(&(remoteAddr.sin_zero), '\0', 8);
   traceSend(packet.c_str(), packet.length(), mClientIp.c_str(), mClientPort);
   int n = sendto(mFd, packet.c_str(), packet.length(), 0, (struct sockaddr *)&remoteAddr, sizeof(struct sockaddr));
   if(n <= 0)
   {
      UniERROR("send udp message to %s:%d failed: %s", mClientIp.c_str(), mClientPort, strerror(errno));
   }
   else
   {
      mSentCount++;
   }
}
