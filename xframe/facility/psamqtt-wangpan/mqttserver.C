/* mqttserver.C, by Zhang Haibin, 2015-05-25
 */
#include <errno.h>

#include "mqttserver.h"
#include "msgdef_mqtt.h"
//commented out by wangpan
// #include "msglib_inter_mqtt.h"
#include "mqttmsg.h"
#include "common.h"
#include "statemachine.h"

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
   UniINFO("CMqttServerTcp::procFdSet method being called");
   //cout << "CMqttServerTcp::procFdSet method being called" << endl;
   if(mListener.accept(fdSet))
   {
      UniINFO("CMqttServerTcp::procFdSet creating new links");
      mAcceptCount++;
      CMqttServerTcpLink* link = new CMqttServerTcpLink();
      mListener.dupConnection(link);
      link->linkId() = mAcceptCount;
      link->setParent(this);
      link->setServer(this);
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

// void CMqttServerTcp::sendPub(TestMessage msg)
// {
//   // UniINFO("CMqttServerTcp::sendPub being called");
//   TPubArg *arg = (TPubArg *)msg.arg.get();
//   set<int> links = subDict[arg->topic];
//   for( set<int>::iterator iter=links.begin() ; iter!=links.end() ; ++iter)
//   {
//     UniINFO("CMqttServerTcp::sendPub: sub linkid: %d",*iter);
//     CMqttServerTcpLink *link = (CMqttServerTcpLink *)mLinks.findByLinkId(*iter);
//     link->wantToSend(msg);
//   }
// }
//
// int CMqttServerTcp::removeSubItem(CStr topic,int linkid)
// {
//   if( subDict.find(topic)==subDict.end() )
//     return 0;
//   return subDict[topic].erase(linkid)==1;
// }
//
// void CMqttServerTcp::addSubItem(CStr topic,int linkid)
// {
//   UniINFO("inserting an Item: topic: %s,linkid: %d",topic.c_str(),linkid);
//
//   subDict[topic].insert(linkid);
//   //UniINFO("size: %d",subDict.size());
//   //subDict[topic].insert(linkid);
//   // if( subDict.find(topic)==subDict.end() )
//   // {
//   //   UniINFO("Entering this");
//   //   set<int> t;
//   //   t.insert(linkid);
//   //   subDict.insert(pair<CStr,set<int> >(topic,t));
//   // // }
//   // else
//   // {
//   //   UniINFO("Entering another branch");
//   //   subDict[topic].insert(linkid);
//   // }
//   UniINFO("After inserting an item");
// }

void CMqttServerTcp::procPub(TUniNetMsg *msg,set<int> subers)
{
  for(set<int>::iterator iter=subers.begin() ; iter!=subers.end() ; ++iter)
  {
    UniINFO("CMqttServerTcp::procPub inside suber loop");
    TMqttPublish *arg = (TMqttPublish *)msg->msgBody;
    CMqttLink *link = getLink(*iter);
    TUniNetMsg *pubmsg = new TUniNetMsg;
    pubmsg->msgName = MQTT_P_PUBLISH;
    TMqttPublish *body = new TMqttPublish;
    body->clientName = arg->clientName;
    body->topic = arg->topic;
    body->content = arg->content;
    pubmsg->setMsgBody(body);
    link->wantToSend(shared_ptr<TUniNetMsg>(pubmsg));
  }
}

void CMqttServerTcp::procMsg(TUniNetMsg *msg)
{
  TComCtrlMsg *ctrl = (TComCtrlMsg *)msg->getCtrlMsgHdr();
  int linkid = ctrl->status;
  CMqttLink *link = getLink(linkid);
  TUniNetMsg *copy = new TUniNetMsg;
  copy->msgName = msg->msgName;
  copy->msgBody = msg->msgBody->clone();
  link->wantToSend(shared_ptr<TUniNetMsg>(copy));
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
  // state = INIT_STATE;
  stateMachine = new MqttServerStateMachine;
  stateMachine->setLink(this);
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
   for(int i=0 ; i<packet.length() ; ++i )
    buf[i]=packet.getByte(i);

   //add seperating charaters \r\n\r\n
   strcpy(buf+packet.length(),"\r\n\r\n");

   code.content=buf;
   code.length=packet.length()+4;
   printf("send: ");
   for( int i=0 ; i<packet.length() ; ++i )
   {
     printf("%x,",code.content[i]);
   }
   printf("\n");
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
  UniINFO("ServerLink::processSend method being called with data" )
  // cout << "ServerLink::processSend method being called with data" << endl;
  shared_ptr<TUniNetMsg> msg = wantsToSend.front();
  wantsToSend.pop();

  SERVER_SM_CODE rt = stateMachine->onSend(msg);
  if( rt==SERVER_SEND_FORWARD )
  {
    switch( msg->msgName )
    {
      case MQTT_P_CONNACK:
        {
          TMqttConnAck *arg = (TMqttConnAck *)(msg->msgBody);
          sendConnAck(false,arg->code);
        }
        break;
      case MQTT_P_PUBLISH:
        {
          TMqttPublish *arg = (TMqttPublish *)(msg->msgBody);
          sendPublish(arg->topic,arg->content);
        }
        break;
      case MQTT_P_SUBACK:
        {
          TMqttSubAck *arg = (TMqttSubAck *)(msg->msgBody);
          sendSubAck(arg->msgId,arg->code);
        }
        break;
      case MQTT_P_UNSUBACK:
        {
          TMqttUnsubAck *arg = (TMqttUnsubAck *)(msg->msgBody);
          sendUnsubAck(arg->msgId);
        }
        break;
      default:
        UniERROR("invalid message:  msgType:%d ",msg->msgName);
        break;
    }
  }

  // switch( msg.msgType )
  // {
  //   case MQTT_P_CONNACK:
  //     {
  //       if( state!=RECV_CON_STATE )
  //       {
  //         UniERROR("trying to send conack while not in RECV_CON_STATE::link: %d",linkId());
  //         break;
  //       }
  //       TConAckArg *arg = (TConAckArg *)msg.arg.get();
  //       UniINFO("sending a conack message::code %d,link: %d",arg->code,linkId());
  //       sendConnAck(false,arg->code);
  //       state = CONNECTED_STATE;
  //     }
  //     break;
  //   case MQTT_P_PUBLISH:
  //     {
  //       TPubArg *arg = (TPubArg *)msg.arg.get();
  //       UniINFO("sending a publish message:: topic: %s,content: %s,link: %d",arg->topic.c_str(),
  //         arg->content.c_str(),linkId());
  //       sendPublish(arg->topic,arg->content);
  //     }
  //     break;
  //   case MQTT_P_SUBACK:
  //     {
  //       if( state!=RECV_SUB_STATE )
  //       {
  //         UniERROR("trying to send a suback message while not in RECV_SUB_STATE");
  //         break;
  //       }
  //       TSubAckArg *arg = (TSubAckArg *)msg.arg.get();
  //       UniINFO("sending a suback message:: packetId: %d,code: %d,link: %d",arg->packetId,
  //         arg->code,linkId());
  //       sendSubAck(arg->packetId,arg->code);
  //       state = CONNECTED_STATE;
  //     }
  //     break;
  //   case MQTT_P_UNSUBACK:
  //     {
  //       if( state!=RECV_UNSUB_STATE )
  //       {
  //         UniERROR("trying to send unsuback while nont in RECV_UNSUB_STATE");
  //         break;
  //       }
  //       TUnsubAckArg *arg = (TUnsubAckArg *)msg.arg.get();
  //       UniINFO("sending a unsuback message:: packetId: %d,code: %d,link: %d",arg->packetId,
  //         arg->code,linkId());
  //       sendUnsubAck(arg->packetId);
  //       state = CONNECTED_STATE;
  //     }
  //     break;
  //   default:
  //     UniERROR("unsupported response:: %d,link: %d",msg.msgType,linkId());
  //     break;
  // }
  // // CStr str = msg.getStr();
  // CCode code;
  // char buf[1024];
  //
  // // int len = msg.msgContent.length();
  // // strncpy(buf,msg.msgContent.c_str(),len);
  // // buf[len]='\0';
  // // code.content = buf;
  // // code.length = len;

  // UniINFO("##from task: %d, %s",task->getTaskId(),buf);
  // // cout << "##from task: " << task->getTaskId() << ", " << buf << endl;
  // int re=send(code);
  // UniINFO("ServerLink::processSend after send: %d",re);
  //cout << "ServerLink::processSend after send: " << re << endl;
  //hasSent.push(msg);
  //setUpAckTimer();
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

int CMqttServerTcpLink::processRecv()
{
  UniINFO("ServerLink::processRecv method being called");
  // cout << "ServerLink::processRecv method being called" << endl;
  CCode code;
  char buf[1024];
  code.content=buf;
  code.length=0;

  int recvstatus = receive(code);

  UniINFO("ServerLink::processRecv after receive: status:%d,len:%d",recvstatus,code.length);
  //buf[code.length]='\0';
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
     printf("received: ");
     for(int i=0 ; i<code.length ; ++i )
     {
       printf("%x,",buf[i]);
     }
     printf("\n");

     //the CStr::cut doesn't fit the need of network message processing
     //build something that takes a cstr
     //return: a cstr
     //char *str,int len,char *res,int len
     char *pat="\r\n\r\n";
     char *res = NULL;
     int reslen = 0;
     char *start = buf;
     int startlen = code.length;

     //for every char in buf
     // for every char in pat
     // if buf[i]==pat[j] ++i,++j
     // if pat[j]=='\0' break, loc = i;

     do {
       find(start,startlen,pat,res,reslen);
      //  if( res!=NULL )
      //  {
      //    for( int i=0 ; i<reslen ; ++i )
      //    {
      //      printf("%x,",res[i]);
      //    }
      //    printf("\n");
      //  }
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

    //
    //  UniINFO("Before Calling msgParse");
    //  mRecvBuffer.nCat(buf,code.length);
    //  CStr result;
    //  do
    //  {
    //    result="";
    //    mRecvBuffer.cut("\r\n\r\n",result);
    //    if( result.length() )
    //    {
    //      cout << "before length: " << code.length << ",after length: " << result.length() << endl;
    //      msgParse(result.c_str(),result.length());
    //    }
    //
    //  } while( result.length() );
    //
    // //  msgParse(code.content,code.length);
    //  UniINFO("After Calling msgParse");
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
  TUniNetMsg *resp = NULL;
  SERVER_SM_CODE rt = stateMachine->onRecv(msg);
  UniINFO("after calling stateMachine->onRecv");
  if( rt==SERVER_RECV_FORWARD )
  {
    switch( msg->msgName )
    {
      case MQTT_P_PUBLISH:
        {
          UniINFO("CMqttServerTcpLink::protocolLogic: receiving pub,linkid: %d",
            linkId());
          task->procPub(msg);
        }
        break;
      case MQTT_P_SUBSCRIBE:
        {
          task->procSub(msg,linkId());
        }
        break;
      case MQTT_P_UNSUBSCRIBE:
        {
          task->procUnsub(msg,linkId());
        }
        break;
      default:
        break;
    }
    //build a copy of the msg or change the prototype of protocolLogic
    UniINFO("before calling task->sendToTask");

    //build a copy of msg
    TUniNetMsg *copy = new TUniNetMsg;
    copy->msgName = msg->msgName;
    copy->msgBody = msg->msgBody->clone();
    task->sendToTask(copy,linkId());
  }
  // switch( msg->msgName )
  // {
  //   case MQTT_P_CONNECT:
  //     {
  //       if( state!=INIT_STATE )
  //       {
  //         UniERROR("received a connect message while state not in INIT_STATE: linkid: %d",linkId());
  //         break;
  //       }
  //       TMqttConnect *arg = (TMqttConnect *)msg->msgBody;
  //
  //       UniINFO("received a connect message: linkid: %d,name: %s,user: %s,passwd: %s",linkId(),
  //         arg->clientName.c_str(),arg->userName.c_str(),arg->password.c_str());
  //       TestMessage msg;
  //       msg.msgType = MQTT_P_CONNACK;
  //       msg.arg = shared_ptr<TMsgArg>(new TConAckArg);
  //       TConAckArg *argt = (TConAckArg *)msg.arg.get();
  //
  //       //during parsing process,we get the code
  //       //how do we pass it out of parsing methods
  //       argt->code = 0;
  //
  //       wantsToSend.push(msg);
  //       state = RECV_CON_STATE;
  //     }
  //     break;
  //   case MQTT_P_PUBLISH:
  //     {
  //       if( state!=CONNECTED_STATE )
  //       {
  //         UniERROR("received a publish message while not in CONNECTED_STATE: linkid: %d",linkId());
  //         break;
  //       }
  //       TMqttPublish *arg = (TMqttPublish *)msg->msgBody;
  //       UniINFO("received a publish message: linkid: %d,topic: %s,content: %s,clientName: %s",
  //         linkId(),arg->topic.c_str(),arg->content.c_str(),arg->clientName.c_str());
  //
  //       TestMessage msg;
  //       msg.msgType = MQTT_P_PUBLISH;
  //       msg.arg = shared_ptr<TMsgArg>(new TPubArg);
  //
  //       TPubArg *argt = (TPubArg *)msg.arg.get();
  //       argt->topic = arg->topic;
  //       argt->content = arg->content;
  //
  //       //server->sendSub(msg);
  //       server->sendPub(msg);
  //       //must depend on Server to do the job
  //       //get list of linkid which subed the topic
  //       //then call wanttosend method of every link
  //
  //     }
  //     break;
  //   case MQTT_P_SUBSCRIBE:
  //     {
  //       if( state!=CONNECTED_STATE )
  //       {
  //         UniERROR("received a subscribe message while not in CONNECTED_STATE: linkid: %d",
  //           linkId());
  //         break;
  //       }
  //       TMqttSubscribe *arg = (TMqttSubscribe *)msg->msgBody;
  //       //build ds in server
  //
  //       for(CList<CStr>::iterator iter=(arg->topic).begin() ; iter!=(arg->topic).end() ; ++iter )
  //       {
  //         UniINFO("topic: %s",(*iter).c_str());
  //         UniINFO("server:%x,parent: %x",server,mParent);
  //         server->addSubItem(*iter,linkId());
  //       }
  //       UniINFO("received a subscribe message: linkid: %d,clientName: %s,packetId: %d,topic: %s",
  //         linkId(),arg->clientName.c_str(),arg->msgId,(*(arg->topic.begin())).c_str());
  //
  //       TestMessage msg;
  //       msg.msgType = MQTT_P_SUBACK;
  //       msg.arg = shared_ptr<TMsgArg>(new TSubAckArg);
  //
  //       TSubAckArg *argt = (TSubAckArg *)msg.arg.get();
  //       argt->packetId = arg->msgId;
  //
  //       //how to generate the code
  //       argt->code = 0;
  //
  //       wantsToSend.push(msg);
  //       state = RECV_SUB_STATE;
  //     }
  //     break;
  //   case MQTT_P_UNSUBSCRIBE:
  //     {
  //       if( state!=CONNECTED_STATE )
  //       {
  //         UniERROR("received a unsubscribe message while not in CONNECTED_STATE: linkid: %d",linkId());
  //         break;
  //       }
  //       TMqttUnsubscribe *arg = (TMqttUnsubscribe *)msg->msgBody;
  //       for(CList<CStr>::iterator iter=arg->topic.begin() ; iter!=arg->topic.end() ; ++iter )
  //       {
  //         server->removeSubItem(*iter,linkId());
  //       }
  //       UniINFO("received a unsubscribe message: linkid: %d,clientName: %s,packetId: %d,topic: %s",
  //         linkId(),arg->clientName.c_str(),arg->msgId,(*(arg->topic.begin())).c_str());
  //
  //       TestMessage msg;
  //       msg.msgType = MQTT_P_UNSUBACK;
  //       msg.arg = shared_ptr<TMsgArg>(new TUnsubAckArg);
  //
  //       TUnsubAckArg *argt = (TUnsubAckArg *)msg.arg.get();
  //       argt->packetId = arg->msgId;
  //       argt->code = 0;
  //
  //       wantsToSend.push(msg);
  //       state = RECV_UNSUB_STATE;
  //     }
  //     break;
  //     //assuming server-side does not set up keepalive timer
  //   case MQTT_P_PINGREQ:
  //     {
  //
  //     }
  //     break;
  //   case MQTT_P_DISCONNECT:
  //     {
  //
  //     }
  //     break;
  //   default:
  //     UniERROR("invalid msgType received in server side:%d",msg->msgName);
  // }
}

void CMqttServerTcpLink::wantToSend(shared_ptr<TUniNetMsg> msg)
{
  wantsToSend.push(msg);
}

void CMqttServerTcpLink::onReceivedMessage(CCode code)
{
  UniINFO("###Received in task: %d , %s",task->getTaskId(),code.content);
  // cout << "###Received in task " << task->getTaskId() << ": " << code.content << endl;
}

void CMqttServerTcpLink::setServer(CMqttServerTcp *ptr)
{
    server = ptr;
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
      UniERROR("send udp message to %s:%d failed: %s", mClientIp.c_str(),
        mClientPort, strerror(errno));
   }
   else
   {
      mSentCount++;
   }
}
