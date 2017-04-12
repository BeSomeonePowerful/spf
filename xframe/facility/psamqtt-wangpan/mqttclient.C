/* mqttclient.C, by Zhang Haibin, 2015-05-25
 */
#include <errno.h>
#include <string.h>
#include "mqttclient.h"
#include "msgdef_mqtt.h"
#include "statemachine.h"

void CMqttClient::init(CStr& ip, int port, CStr& name, CStr& user, CStr& passwd, int keepAlive)
{
   mDestIp = ip;
   mDestPort = port;
   mName = name;
   mUser = user;
   mPasswd = passwd;
   mKeepAlive = keepAlive;
}

void CMqttClient::buildFdSet(CFdSet& fdSet)
{
}

void CMqttClient::procFdSet(CFdSet& fdSet)
{
}

void CMqttClient::close()
{
}

void CMqttClient::list(CStr& str)
{
}



//////////////////////////////////////////////////////////////
// CMqttClientTcp
//////////////////////////////////////////////////////////////
void CMqttClientTcp::init(CStr& ip, int port, CStr& name, CStr& user, CStr& passwd, int keepAlive)
{
   CMqttClient::init(ip, port, name, user, passwd, keepAlive);
   mLink.setParent(this);
   mLink.connect(mDestIp.c_str(), mDestPort, NULL, 0, false);

   mLink.setTask(task);
   //should not send immediately
   //but express wants to send sth in the future
   //mLink.sendConnect(mName, mUser, mPasswd, mKeepAlive);
   //code added by wangpan

  //  CMqttTCPClientTask *t = (CMqttTCPClientTask *)task;
  //
  //  //rather than wants to send ,we send and setup corresponding ds
  // TConArg *arg = new TConArg(name,user,passwd);
  // arg->keepAlive = keepAlive;
  // //  t->wantsToSend(MQTT_P_CONNECT,arg);
  //
  // //setup ds before realsend
  // t->hasSentConnect(arg);
  // mLink.sendConnect(mName,mUser,mPasswd,mKeepAlive);
}

void CMqttClientTcp::buildFdSet(CFdSet& fdSet)
{
   //cout << "CMqttClientTcp::buildFdSet method being called" << endl;
   mLink.buildFdSet(fdSet);
}

void CMqttClientTcp::procFdSet(CFdSet& fdSet)
{
   //cout << "CMqttClientTcp::procFdSet method being called" << endl;
   int re = mLink.process(fdSet);
   //close here is wrong
   if(re < 0)
   {
       UniERROR("connection to %s:%d broken", mDestIp.c_str(), mDestPort);
       mLink.close();
   }
}

void CMqttClientTcp::close()
{
   mLink.close();
}

void CMqttClientTcp::list(CStr& str)
{
   str << "TYPE   FD    SEND     RECEIVE  RBUFFER  LINKID REMOTE-ADDR\n";
   mLink.printState(str);
   listServiceConfig(str);
}

CMqttLink* CMqttClientTcp::getLink(int)
{
   return &mLink;
}

void CMqttClientTcp::secAction()
{
   mLink.secAction();
}

void CMqttClientTcp::closeLink(int)
{
   mLink.closeLink();
}

void CMqttClientTcp::reconnect()
{
   if(mLink.alive())
      return;
   mLink.connect(mDestIp.c_str(), mDestPort, NULL, 0, false);
   mLink.sendConnect(mName, mUser, mPasswd, mKeepAlive);
}

void CMqttClientTcp::wantToSend(shared_ptr<TUniNetMsg> msg)
{
  mLink.wantToSend(msg);
}

void CMqttClientTcp::onTimeOut(TTimerKey timerKey,void *para)
{
  mLink.onTimeOut(timerKey,para);
}
//////////////////////////////////////////////////////////////
// CMqttClientUdp
//////////////////////////////////////////////////////////////
void CMqttClientUdp::init(CStr& ip, int port, CStr& name, CStr& user, CStr& passwd, int keepAlive)
{
   CMqttClient::init(ip, port, name, user, passwd, keepAlive);
   mLink.setParent(this);
   mLink.setAddr(ip, port);
   mLink.sendConnect(mName, mUser, mPasswd, mKeepAlive);
}

void CMqttClientUdp::buildFdSet(CFdSet& fdSet)
{
   mLink.buildFdSet(fdSet);
}

void CMqttClientUdp::procFdSet(CFdSet& fdSet)
{
   mLink.procFdSet(fdSet);
}

void CMqttClientUdp::close()
{
   mLink.closeLink();
}

void CMqttClientUdp::list(CStr& str)
{
}

void CMqttClientUdp::secAction()
{
   mLink.secAction();
}

CMqttLink* CMqttClientUdp::getLink(int)
{
   return &mLink;
}

void CMqttClientUdp::closeLink(int)
{
   mLink.closeLink();
}

void CMqttClientUdp::reconnect()
{
   if(mLink.alive())
      return;
   mLink.sendConnect(mName, mUser, mPasswd, mKeepAlive);
}

//////////////////////////////////////////////////////////////
// CMqttClientTcpLink
//////////////////////////////////////////////////////////////
CMqttClientTcpLink::CMqttClientTcpLink()
{
  // state = INIT_STATE;
  // conackTimer = -1;
  // subackTimer = -1;
  // unsubackTimer = -1;
  // pingrspTimer = -1;
  stateMachine = new MqttClientStateMachine();
  stateMachine->setLink(this);
}

CMqttClientTcpLink::~CMqttClientTcpLink()
{
  if( stateMachine!=NULL ) delete stateMachine;
}

void CMqttClientTcpLink::sendPacket(CStr& packet)
{
   //traceSend(packet.c_str(), packet.length(), remoteIp(), remotePort());

   //
  //  printf("before send: ");
  //  const char *tmp = packet.c_str();
  //  for( int i=0 ; i<packet.length() ; ++i )
  //  {
  //    printf("%x,",tmp[i]);
  //  }
  //  printf("\n");
   const char *tmp = packet.c_str();
   CCode code;
   char buf[1024];
   int len = packet.length();
   //strcpy(buf,packet.c_str());
   //we copy byte by byte
   for( int i=0 ; i<len ; ++i )
   {
     buf[i]=tmp[i];
   }
   buf[len]='\0';

   //adding seperating charaters
   strcpy(buf+len,"\r\n\r\n");

   code.content=buf;
   code.length=len+4;
   printf("send: ");
   for( int i=0 ; i<packet.length() ; ++i )
   {
     printf("%x,",code.content[i]);
   }
   printf("\n");
   send(code);
}

int CMqttClientTcpLink::processReceived(const char* content, int length)
{
   //traceReceive(content, length, remoteIp(), remotePort());
   return msgParseStream(content, length);
}

void CMqttClientTcpLink::closeLink()
{
   CMqttClientLink::closeLink();
   close();
}

bool CMqttClientTcpLink::alive()
{
   return mAlive && !isSocketStateClose();
}

void CMqttClientTcpLink::wantToSend(shared_ptr<TUniNetMsg> msg)
{
  // TestMessage m;
  // m.msgContent = msg;
  wantsToSend.push(msg);
}

void CMqttClientTcpLink::onTimeOut(TTimerKey timerKey,void *para)
{
  //resend timer
  //wait for a response timer
  CLIENT_SM_CODE code = CLIENT_TIMEOUT_REFUSE;
  shared_ptr<TUniNetMsg> resend ;
  code = stateMachine->onTimeOut(timerKey,para,resend);

  //how to deal with timeout in statemachine
  if( code==CLIENT_TIMEOUT_FORWARD )
    wantToSend(resend);
  // UniINFO("CMqttClientTcpLink::onTimeOut method being called");
  // if( timerKey==conackTimer )
  // {
  //   UniINFO("conackTimer time out");
  //   if( state!=WAIT_CONACK_STATE )
  //   {
  //     UniERROR("conackTimer out while state not in WAIT_CONACK_STATE");
  //     return;
  //   }
  //
  //   //what to do while conack timeOut
  //   state = INIT_STATE;
  //   conackTimer = -1;
  //   //calling onConackTimeOut() method
  //   return;
  // }
  // if( timerKey==subackTimer )
  // {
  //   if( state!=WAIT_SUBACK_STATE )
  //   {
  //     UniERROR("subackTimer out while state not in WAIT_SUBACK_STATE");
  //     return;
  //   }
  //   UniINFO("subackTimer time out");
  //   state = CONNECTED_STATE;
  //   subackTimer = -1;
  //   return;
  // }
  // if( timerKey==unsubackTimer )
  // {
  //   if( state!=WAIT_UNSUBACK_STATE )
  //   {
  //     UniERROR("unsubackTimer out while state not in WAIT_UNSUBACK_STATE");
  //     return;
  //   }
  //   UniINFO("unsubackTimer time out");
  //   state = CONNECTED_STATE;
  //   unsubackTimer = -1;
  //   return;
  // }
  // if( timerKey==pingrspTimer )
  // {
  //   UniINFO("pingrspTimer time out");
  //   pingrspTimer = -1;
  //   return;
  // }
  // UniERROR("unexpected timer:timerKey: %d",timerKey);
  // // cout << "CMqttClientTcpLink::onTimeOut method being called" << endl;
}

void CMqttClientTcpLink::onReceivedMessage(CCode code)
{
  UniINFO("###Received in task: %d , %s",task->getTaskId(),code.content);
  // cout << "###Received in task " << task->getTaskId() << ": " << code.content << endl;
}

void CMqttClientTcpLink::processSend()
{
  UniINFO("CMqttClientTcpLink::processSend method being called");
  // cout << "CMqttClientTcpLink::processSend method being called" << endl;
  if( wantsToSend.empty() ) return;
  UniINFO("CMqttClientTcpLink::processSend method being called with data");
  // cout << "CMqttClientTcpLink::processSend method being called with data" << endl;
  shared_ptr<TUniNetMsg> msg = wantsToSend.front();
  wantsToSend.pop();

  CLIENT_SM_CODE code = stateMachine->onSend(msg);
  UniINFO("CMqttClientTcpLink::processSend after stateMachine");
  if( code==CLIENT_SEND_FORWARD )
  {
    switch( msg->msgName )
    {
      case MQTT_P_CONNECT:
        {
          TMqttConnect *arg = (TMqttConnect *)msg->msgBody;
          UniINFO("CMqttClientTcpLink::processSend after pointer casting");
          sendConnect(arg->clientName,arg->userName,arg->password,0);
        }
        break;
      case MQTT_P_PUBLISH:
        {
          TMqttPublish *arg = (TMqttPublish *)msg->msgBody;
          sendPublish(arg->topic,arg->content);
        }
        break;
      case MQTT_P_SUBSCRIBE:
        {
          TMqttSubscribe *arg = (TMqttSubscribe *)msg->msgBody;
          sendSubscribe(arg->topic,arg->msgId);
        }
        break;
      case MQTT_P_UNSUBSCRIBE:
        {
          TMqttUnsubscribe *arg = (TMqttUnsubscribe *)msg->msgBody;
          sendUnsubscribe(arg->topic,arg->msgId);
        }
        break;
      case MQTT_P_DISCONNECT:
        {
          sendDisconnect();
        }
        break;
      default:
        UniERROR("invalid message type: %d",msg->msgName);
        break;
    }
  }
  // else
  // {
  //   //build message and sendTo network
  //   //call sendXXX method and do the real work
  // }
  // switch( msg.msgType )
  // {
  //   case MQTT_P_CONNECT:
  //     {
  //       if( state!=INIT_STATE )
  //       {
  //         UniERROR("###ERROR:trying to send a connect when not in INIT_STATE");
  //         break;
  //       }
  //       TConArg *arg = (TConArg *)msg.arg.get();
  //
  //       UniINFO("sending a connect message:: name: %s,user: %s,passwd: %s,keepalive: %d",
  //         arg->name.c_str(),arg->user.c_str(),arg->passwd.c_str(),arg->keepAlive);
  //       //make sure sendXXX method only do the encapsulate and send stuff
  //       //no more
  //       sendConnect(arg->name,arg->user,arg->passwd,arg->keepAlive);
  //       conackTimer = task->setTimer(ACK_TIMER);
  //       //need to keep some info to be used when receiving an ack
  //       state = WAIT_CONACK_STATE;
  //     }
  //     break;
  //   case MQTT_P_PUBLISH:
  //     {
  //       if( state!=CONNECTED_STATE )
  //       {
  //         UniERROR("##ERROR:trying to send publish while not in CONNECTED_STATE");
  //         break;
  //       }
  //       TPubArg *arg = (TPubArg *)msg.arg.get();
  //       UniINFO("sending a publish message:: topic: %s,content %s",arg->topic.c_str(),
  //         arg->content.c_str());
  //       sendPublish(arg->topic,arg->content);
  //       //don't need a ack at current implementation
  //     }
  //     break;
  //   case MQTT_P_SUBSCRIBE:
  //     {
  //       if( state!=CONNECTED_STATE )
  //       {
  //         UniERROR("##ERROR:trying to send subscribe while not in CONNECTED_STATE");
  //         break;
  //       }
  //       TSubArg *arg = (TSubArg *)msg.arg.get();
  //       //change arg to a list
  //       //packetId managed by which? task? or link? or selftask?
  //       UniINFO("sending a subscribe message:: topics: %s,packetId: %d",
  //       (*(arg->topics.begin())).c_str(),arg->packetId);
  //       //print topics one by one
  //
  //       sendSubscribe(arg->topics,arg->packetId);
  //       subackTimer = task->setTimer(ACK_TIMER);
  //       state = WAIT_SUBACK_STATE;
  //       //need to keep some info while receiving
  //       argDict.insert(pair<int,shared_ptr<TMsgArg> >(MQTT_P_SUBSCRIBE,msg.arg));
  //     }
  //     break;
  //   case MQTT_P_UNSUBSCRIBE:
  //     {
  //       if( state!=CONNECTED_STATE )
  //       {
  //         UniERROR("##ERROR:trying to send subscribe while not in CONNECTED_STATE");
  //         break;
  //       }
  //       TUnsubArg *arg = (TUnsubArg *)msg.arg.get();
  //       UniINFO("sending a unsubscribe message:: topics: %s,packetId: %d",
  //       (*(arg->topics.begin())).c_str(),arg->packetId);
  //       sendUnsubscribe(arg->topics,arg->packetId);
  //       unsubackTimer = task->setTimer(ACK_TIMER);
  //       state = WAIT_UNSUBACK_STATE;
  //       argDict.insert(pair<int,shared_ptr<TMsgArg> >(MQTT_P_UNSUBSCRIBE,msg.arg));
  //     }
  //     break;
  //   case MQTT_P_PINGREQ:
  //     {
  //       //when does a keepalive timer out
  //       //send ping req,problem when keepalive timer out
  //       //generate a pingreq to send msg,but don't send until readable
  //       //we may need a deque to do this
  //       UniINFO("sending a pingreq message");
  //       sendPingReq();
  //       pingrspTimer = task->setTimer(ACK_TIMER);
  //       state = WAIT_PINGRSP_STATE;
  //     }
  //     break;
  //   case MQTT_P_DISCONNECT:
  //     {
  //       UniINFO("sending a disconnect message");
  //       sendDisconnect();
  //       state = END_STATE;
  //       //and close the link
  //     }
  //     break;
  //   default:
  //     UniERROR("invalid msgType: %d",msg.msgType);
  //     break;
  // }

  // //cout << "CMqttClientTcpLink::processSend before real sending" << endl;
  // // CStr str = msg.getStr();
  // CCode code;
  // char buf[1024];
  //
  // int len = msg.msgContent.length();
  // strncpy(buf,msg.msgContent.c_str(),len);
  //
  // //cout << "CMqttClientTcpLink::processSend after copying string" << endl;
  // buf[len]='\0';
  // //cout << "CMqttClientTcpLink::processSend before printing" << endl;
  //
  // UniINFO("##from task: %d, %s",task->getTaskId(),buf);
  // // cout << "##from task: " <<  task->getTaskId() <<", " << buf << endl;
  // code.content = buf;
  // code.length = len;
  // int re=send(code);
  // UniINFO("ClientLink::processSend after send: %d",re);
  // // cout << "ClientLink::processSend after send: " << re << endl;
  // //hasSent.push(msg);
  // //setUpAckTimer();
}

static void find(char *str,int len1,char *pat,char *&match,int &len)
{
  for( int i=0 ; i<len1 ; ++i )
  {
    int k=0;
    int j;
    for( j=i,k=0 ; j<len1 && k<4  ;++j,++k )
    {
      if( str[j]!=pat[k]) break;
    }
    if( k==4 )
    {
       match = str;
       len = i;
       return;
    }
  }
  match = str;
  len = len1;
}

int CMqttClientTcpLink::processRecv()
{
  UniINFO("CMqttClientTcpLink::processRecv method being called");
  //cout << "ClientLink::processRecv method being called" << endl;
  CCode code;
  char buf[1024];
  code.content=buf;
  code.length=0;

  int recvstatus = receive(code);
  if( recvstatus )
  {
    //TestMessage msg = TestMessage::parse(code.content,code.length);
    //report receive
      //onReceivedMessage(code);
      printf("received: ");
      for(int i=0 ; i<code.length ; ++i )
      {
        printf("%x,",buf[i]);
      }
      printf("\n");

      char *pat="\r\n\r\n";
      char *res = NULL;
      int reslen = 0;
      char *start = buf;
      int startlen = code.length;
  
      do {
        find(start,startlen,pat,res,reslen);
        msgParse(res,reslen);
        start = res+reslen;
        startlen = startlen-reslen;
        if( startlen!=reslen )
        {
          start += strlen(pat);
          startlen -= strlen(pat);
        }
        /* code */
      } while(startlen>0);
      // mRecvBuffer.nCat(buf,code.length);
      // CStr result;
      // do
      // {
      //   result="";
      //   mRecvBuffer.cut("\r\n\r\n",result);
      //   if( result.length() )
      //   {
      //     msgParse(result.c_str(),result.length());
      //   }
      //
      // } while( result.length() );

      // msgParse(code.content,code.length);
    //delAckTimer();
    //hasSent.pop();
    //received.push(msg);
    return 1;
  }
  else
    UniERROR("Error in receive func");
  return 0;
}

void CMqttClientTcpLink::protocolLogic(std::auto_ptr<TUniNetMsg> unimsg)
{
  TUniNetMsg *msg = unimsg.get();
  CLIENT_SM_CODE rt = stateMachine->onRecv(msg);
  if( rt==CLIENT_RECV_REFUSE )
  {
    UniINFO("message refused to recv: msgtype:%d",msg->msgName);
  }
  else if( rt==CLIENT_RECV_FORWARD )
  {
    //forward to application,so need a reference to task
    UniINFO("CMqttClientTcpLink::protocolLogic before calling sendToTask");
    TUniNetMsg *copy = new TUniNetMsg;
    copy->msgName = msg->msgName;
    copy->msgBody = msg->msgBody->clone();
    task->sendToTask(copy);
  }
  // TUniNetMsg *msg = unimsg.get();
  // switch( msg->msgName )
  // {
  //   case MQTT_P_PUBLISH:
  //     {
  //       TMqttPublish *arg = (TMqttPublish *)msg->msgBody;
  //       //publish can be received at any time
  //       UniINFO("received a publish message: topic: %s,content: %s,clientName: %s",
  //       arg->topic.c_str(),arg->content.c_str(),arg->clientName.c_str());
  //     }
  //     break;
  //   case MQTT_P_CONNACK:
  //     {
  //       if( state!=WAIT_CONACK_STATE )
  //       {
  //         UniERROR("received a connack message while state not in WAIT_CONACK_STATE");
  //         break;
  //       }
  //       TMqttConnAck *arg = (TMqttConnAck *)msg->msgBody;
  //       UniINFO("received a conack message:: code: %d",arg->code);
  //       if( conackTimer!=-1 )
  //       {
  //         task->delTimer(conackTimer);
  //         conackTimer = -1;
  //       }
  //
  //       //may need to set state according to arg->code
  //       if( arg->code )
  //       {
  //         state = END_STATE;
  //         //close link
  //       }
  //       else
  //         state = CONNECTED_STATE;
  //     }
  //     break;
  //   case MQTT_P_SUBACK:
  //     {
  //       if( state!=WAIT_SUBACK_STATE )
  //       {
  //         UniERROR("received a suback message while state not in WAIT_SUBACK_STATE");
  //         break;
  //       }
  //       if( subackTimer!=-1 )
  //       {
  //         task->delTimer(subackTimer);
  //         subackTimer = -1;
  //       }
  //
  //       TMqttSubAck *arg = (TMqttSubAck *)msg->msgBody;
  //       int packetId = arg->msgId;
  //       TSubArg *subarg =(TSubArg *)argDict[MQTT_P_SUBSCRIBE].get();
  //       int oldPacketId = arg->msgId;
  //       argDict.erase(MQTT_P_SUBSCRIBE);
  //
  //       UniINFO("received a suback message: packetId:%d,oldPacketId:%d",
  //        packetId,oldPacketId);
  //       if( oldPacketId!=packetId )
  //       {
  //         UniERROR("##ERROR:packetId does not match in suback message");
  //       }
  //       state = CONNECTED_STATE;
  //     }
  //     break;
  //   case MQTT_P_UNSUBACK:
  //     {
  //       if( state!=WAIT_UNSUBACK_STATE )
  //       {
  //         UniERROR("received a unsuback message while state not in WAIT_UNSUBACK_STATE");
  //         break;
  //       }
  //       if( unsubackTimer!=-1 )
  //       {
  //         task->delTimer(unsubackTimer);
  //         unsubackTimer = -1;
  //       }
  //
  //       TMqttUnsubAck *arg = (TMqttUnsubAck *)msg->msgBody;
  //       int packetId = arg->msgId;
  //       TUnsubArg *subarg =(TUnsubArg *)argDict[MQTT_P_UNSUBSCRIBE].get();
  //       int oldPacketId = arg->msgId;
  //       argDict.erase(MQTT_P_UNSUBSCRIBE);
  //
  //       UniINFO("received a unsuback message: packetId:%d,oldPacketId:%d",
  //        packetId,oldPacketId);
  //       if( oldPacketId!=packetId )
  //       {
  //         UniERROR("##ERROR:packetId does not match in unsuback message");
  //       }
  //       state = CONNECTED_STATE;
  //     }
  //     break;
  //   case MQTT_P_PINGRESP:
  //     {
  //       //keepalive timer and acktimer relation
  //       if( state!=WAIT_PINGRSP_STATE )
  //       {
  //         UniERROR("received a pingresp message while state not in WAIT_PINGRSP_STATE");
  //         break;
  //       }
  //       if( pingrspTimer!=-1 )
  //       {
  //         task->delTimer(pingrspTimer);
  //         pingrspTimer = -1;
  //       }
  //       UniINFO("received a pingresp message");
  //       //which state should get back to when receiving a pingrsp
  //       //may need to keep old state
  //     }
  //     break;
  //   case MQTT_P_DISCONNECT:
  //     {
  //       UniINFO("received a disconnect message");
  //       state = END_STATE;
  //       //close the link
  //     }
  //     break;
  //   default:
  //     UniERROR("invalid msgType: %d",msg->msgName);
  //     break;
  // }
}
//////////////////////////////////////////////////////////////
// CMqttClientUdpLink
//////////////////////////////////////////////////////////////

CMqttClientUdpLink::CMqttClientUdpLink()
{
   mFd = -1;
}

void CMqttClientUdpLink::setAddr(CStr& ip, int port)
{
   mIp = ip;
   mPort = port;
}

void CMqttClientUdpLink::sendPacket(CStr& packet)
{
   if(mFd < 0)
   {
      mFd = socket(AF_INET, SOCK_DGRAM, 0);
      if(mFd < 0)
      {
         UniERROR("CMqttClientUdpLink::sendPacket, socket() failed: %s", strerror(errno));
         return;
      }
      UniINFO("udp socket initialized, fd = %d", mFd);
   }
   struct sockaddr_in remoteAddr;
   remoteAddr.sin_family = AF_INET;
   remoteAddr.sin_addr.s_addr = inet_addr(mIp.c_str());
   remoteAddr.sin_port = htons(mPort);
   memset(&(remoteAddr.sin_zero), '\0', 8);
   traceSend(packet.c_str(), packet.length(), mIp.c_str(), mPort);
   int n = sendto(mFd, packet.c_str(), packet.length(), 0, (struct sockaddr *)&remoteAddr, sizeof(struct sockaddr));
   if(n <= 0)
      UniERROR("send udp message to %s:%d failed: %s", mIp.c_str(), mPort, strerror(errno));
}

void CMqttClientUdpLink::closeLink()
{
   CMqttClientLink::closeLink();
   close(mFd);
   mFd = -1;
}

void CMqttClientUdpLink::buildFdSet(CFdSet& fdSet)
{
   if(mFd > 0)
      fdSet.setRead(mFd);
}

void CMqttClientUdpLink::procFdSet(CFdSet& fdSet)
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
      traceReceive(buffer, length, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
      msgParse(buffer, length);
   }
}
