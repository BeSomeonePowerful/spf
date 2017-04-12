/* mqttlink.C, by Zhang Haibin, 2015-05-26
 */
#include "mqttlink.h"
// #include "msglib_inter_mqtt.h"
#include "mqttserver.h"
#include "msgdef_mqtt.h"
// #include "uniframe.h"

///////////////////////////////////////////////////////
// CMqttLink
///////////////////////////////////////////////////////

// extern bool g_trace;
// extern int g_thisPsaId;

//not quite sure functionality of g_thisPsaId
//we try to minize modification
int g_thisPsaId = 1999;
bool g_trace = 1;

CMqttLink::CMqttLink()
{
   mCurrentPacketId = 0;
   mIdleSec = 0;
   mKeepAlive = 0;
   mAlive = false;
}

int CMqttLink::newPacketId(TMsgAddress& addr)
{
   mCurrentPacketId = (mCurrentPacketId + 1) % 65536;
   mAddrMap.put(mCurrentPacketId, addr);
   return mCurrentPacketId;
}

void CMqttLink::freePacketId(int packetId)
{
   mAddrMap.remove(packetId);
}

bool CMqttLink::getAppAddr(int packetId, TMsgAddress& addr)
{
   if(!mAddrMap.get(packetId, addr))
   {
      UniERROR("can not get app address according to packet id %d", packetId);
      return false;
   }
   return true;
}

void CMqttLink::printAddrMap(CStr& str)
{
   str << "\nPacketId AppAddr Map:";
   mAddrMap.reset();
   int packetId;
   TMsgAddress addr;
   while(mAddrMap.getNext(packetId, addr))
   {
      //commented out by wangpan
      //since we're not going to use mAddrMap in our app
      // str.fCat("\n   %-10d %s", packetId, strMsgAddress1(addr));
   }
}

void CMqttLink::traceSend(const char* content, int length, const char* ip, int port)
{
   if(!g_trace)
      return;
   CStr str;
   str.fCat("Send %d bytes to %s:%d:", length, ip, port);
   str.fCatBin(content, length);
   UniINFO("%s\n", str.c_str());
}

void CMqttLink::traceReceive(const char* content, int length, const char* ip, int port)
{
   if(!g_trace)
      return;
   CStr str;
   str.fCat("Received %d bytes from %s:%d:", length, ip, port);
   str.fCatBin(content, length);
   UniINFO("%s\n", str.c_str());
}

int CMqttLink::msgParse(const char* content, int length)
{
   for(int i=0 ; i<length ; ++i )
   {
     printf("%x,",content[i]);
   }
   printf("\n");
   //cout << "msgParse:length of string: " << length << endl;
   mIdleSec = 0;
   const char* p = content;
   int remainLength = length;

   if(remainLength < 2)
   {
      UniERROR("CMqttLink::msgParse, message length = %d", remainLength);
      return -1;
   }

   unsigned char headByte = *p;
   p++;
   remainLength--;
   int packetType = headByte;
   packetType >>= 4;
   int flags = headByte & 0x0F;

   int bodyLength = CMqttMsg::parseLength(p, remainLength);
   if(bodyLength != remainLength)
   {
      UniERROR("CMqttLink::msgParse, parse length failed");
      return -1;
   }

   cout << "Received: " << packetType << endl;
   cout << "length: " << bodyLength << endl;
   msgProcess(packetType, flags, p, bodyLength);
   return 1;
}


int CMqttLink::msgParseStream(const char* content, int length)
{
   mIdleSec = 0;
   mBufferMsg.nCat(content, length);
   const char* p = mBufferMsg.c_str();
   int remainLength = mBufferMsg.length();

   int msgCount = 0;
   do
   {
      if(remainLength < 2)
         return 0;

      const char* saveP = p;
      int saveLength = remainLength;

      unsigned char headByte = *p;
      p++;
      remainLength--;
      int packetType = headByte;
      packetType >>= 4;
      int flags = headByte & 0x0F;

      int bodyLength = CMqttMsg::parseLength(p, remainLength);
      if(bodyLength == -1)
         return -1;
      if(bodyLength == -2)
      {
         p = saveP;
         remainLength = saveLength;
         break;
      }
      if(bodyLength > remainLength)
      {
         p = saveP;
         remainLength = saveLength;
         break;
      }
      msgProcess(packetType, flags, p, bodyLength);
      msgCount++;
      remainLength -= bodyLength;
      p += bodyLength;
   }
   while(remainLength > 0);

   if(remainLength != 0)
   {
      if(remainLength < mBufferMsg.length())
      //modified by wangpan
         mBufferMsg.cut(-remainLength);
   }
   else
      mBufferMsg.clear();
   return msgCount;
}

#define MQTT_PARSE_STRING(msgName, paraName)\
{\
   if(!CMqttMsg::parseString(paraName, p, length))\
   {\
      CMqttMsg::parseError(#msgName, #paraName);\
      return;\
   }\
}

#define MQTT_PARSE_BYTE(msgName, paraName)\
{\
   if(!CMqttMsg::parseByte(paraName, p, length))\
   {\
      CMqttMsg::parseError(#msgName, #paraName);\
      return;\
   }\
}

#define MQTT_PARSE_SHORT(msgName, paraName)\
{\
   if(!CMqttMsg::parseShort(paraName, p, length))\
   {\
      CMqttMsg::parseError(#msgName, #paraName);\
      return;\
   }\
}


void CMqttLink::msgProcess(int packetType, int flags, const char* p, int length)
{
   UniERROR("CMqttLink::msgProcess not defined");
}

void CMqttLink::sendPacket(CStr& packet)
{

   UniERROR("CMqttLink::sendPacket not defined");
}

//what is the functionality of heartbeat
void CMqttLink::sendPublish(CStr& topic, CStr& content)
{
   CStr body;
   CMqttMsg::appendStr(body, topic.c_str());
   body += content;
   CStr msg;
   CMqttMsg::pack(msg, MQTT_P_PUBLISH, body);
   sendPacket(msg);
}

void CMqttLink::setParent(CMqtt* parent)
{
   mParent = parent;
}

//what does it trying to do??
//do I need to use TMsgAddr here,I don't think so ??
void CMqttLink::invokeService(const char* packetName, int msgId, TMsgBody* msgBody)
{
   TUniNetMsg* msg = new TUniNetMsg;

   //what is the functionality of psaId
   //we could use taskId to initialize this field
   //we will set it to taskId of which this link belongs to
   msg->oAddr.logAddr = g_thisPsaId;

   //does thsi field has any functionality

   //send
   //receive method
   msg->oAddr.taskInstID = getLinkId();
   CStr tag = packetName;

   //need to initialize tag and PsaId
   //how to do that or in which way are we going to initialize
   //functionality of this field

   //it should be the task that is responsible for handling the logic
   //should we build a new task

   //where should the message handling be done
   //in task where the link belongs or in a seperate task shouldn't be a matter

   //what is the functionality of this
   //for now we simply ignore this field
   msg->tAddr.logAddr = mParent->getAppKey(tag);
   msg->msgType = MSGTYPE_MQTT;

   //this is the msgtype

   //we should use some data structure to do the logic
   //
   msg->msgName = msgId;
   msg->setMsgBody(msgBody);

   //is this valid or invalid
   //CMqttLink module passes encapsulated message to Task module

   //where is this msg from
   //how are we going to write this method in CMqttServerTask
   //mParent->sendMsgToPSATask(this,msg);

   //task->protocolLogic(std::auto_ptr<TUniNetMsg>(msg));
  //  CUniFrame::sendMsgToFEAM(msg);
}

void CMqttLink::responseService(int msgId, TMsgAddress& addr, TMsgBody* msgBody)
{
   TUniNetMsg* msg = new TUniNetMsg;
   msg->oAddr.logAddr = g_thisPsaId;
   msg->oAddr.taskInstID = getLinkId();
   msg->tAddr = addr;
   msg->msgType = MSGTYPE_MQTT;
   msg->dialogType = DIALOG_CONTINUE;
   msg->msgName = msgId;
   msg->setMsgBody(msgBody);


  //  task->protocolLogic(std::auto_ptr<TUniNetMsg>(msg));
  //  CUniFrame::sendMsgToFEAM(msg);
}

void CMqttLink::secAction()
{
}

///////////////////////////////////////////////////////
// CMqttServerLink
///////////////////////////////////////////////////////

void CMqttServerLink::msgProcess(int packetType, int flags, const char* p, int length)
{
   cout << "packetType: " << packetType << endl;
   switch(packetType)
   {
   case MQTT_P_CONNECT:
      processConnect(p, length);
      return;
   case MQTT_P_PUBLISH:
      processPublish(p, length, flags);
      return;
   case MQTT_P_SUBSCRIBE:
      processSubscribe(p, length);
      return;
   case MQTT_P_UNSUBSCRIBE:
      processUnsubscribe(p, length);
      return;
   case MQTT_P_PINGREQ:
      sendPingResp();
      return;
   case MQTT_P_DISCONNECT:
      processDisconnect();
      return;
   default:
      UniERROR("received invalid packet %d for server link", packetType);
      return;
   }
}

void CMqttServerLink::processDisconnect()
{
   UniINFO("received disconnect message from client %s", mClientName.c_str());
   TMqttDisconnect* disconnectArg = new TMqttDisconnect;

   //currently we think the clientname is unique,there must be a mapping between
   //clientname and client connectionLinkId


   disconnectArg->clientName = mClientName;
   invokeService("disconnect", MQTT_DISCONNECT, disconnectArg);
   mAlive = false;
}

void CMqttServerLink::processConnect(const char* p, int length)
{
   cout << "CMqttServerLink::processConnect method being called " << endl;
   CStr protocolName;
   MQTT_PARSE_STRING(connect, protocolName)

   cout << "length: " << length << endl;
   cout << "protocolName: ";
   {
     const char *tmp = protocolName.c_str();
     for( int i=0 ; i<protocolName.length() ; ++i )
     {
       printf("%x,",tmp[i]);
     }
     printf("\n");
   }

   int protocolLevel;
   MQTT_PARSE_BYTE(connect, protocolLevel)

   int connectFlags;
   MQTT_PARSE_BYTE(connect, connectFlags)

   int keepAlive;
   MQTT_PARSE_SHORT(connect, keepAlive)
   mKeepAlive = keepAlive;

   printf("protocolLevel= %x,connectFlags= %x,keepAlive= %x\n",protocolLevel,connectFlags,keepAlive);

   CStr clientName;
   MQTT_PARSE_STRING(connect, clientName)
   mClientName = clientName;

   CStr willTopic;
   CStr willMessage;

   cout << "After receiving:connectFlags: " << connectFlags << endl;
   if(connectFlags & 0x04)
   {
      MQTT_PARSE_STRING(connect, willTopic)
      MQTT_PARSE_STRING(connect, willMessage)
   }

   CStr userName;
   if(connectFlags & 0x80)
      MQTT_PARSE_STRING(connect, userName)

   CStr password;
   if(connectFlags & 0x40)
      MQTT_PARSE_STRING(connect, password)

   cout << "After parsing,name: " << clientName.c_str() << ",user: " << userName.c_str() << ",passwd: " << password.c_str() << ",keepAlive: " << keepAlive << endl;

   UniINFO("received connect message: %s,%d,0x%02X,%d: %s,%s,%s"
      , protocolName.c_str()
      , protocolLevel
      , connectFlags
      , keepAlive
      , clientName.c_str()
      , userName.c_str()
      , password.c_str()
   );

   //how to do with it
   if(protocolName != "MQTT" || protocolLevel != 4)
   {
      sendConnAck(false, 1);
      return;
   }

   TMqttConnect* connectArg = new TMqttConnect;
   connectArg->clientName = mClientName;
   connectArg->setUserName(userName);
   connectArg->setPassword(password);
   invokeService("connect", MQTT_P_CONNECT, connectArg);
   mAlive = true;
}

void CMqttServerLink::processPublish(const char* p, int length, int flags)
{
   if(flags != 0)
   {
      UniERROR("only support publish message with Qos 0");
      return;
   }
   CStr topicName;
   MQTT_PARSE_STRING(publish, topicName)

   CStr content;
   content.set(p, length);

   TMqttPublish* publishArg = new TMqttPublish;
   publishArg->clientName = mClientName;
   publishArg->topic = topicName;
   publishArg->content = content;
   invokeService("publish", MQTT_P_PUBLISH, publishArg);
}

void CMqttServerLink::processSubscribe(const char* p, int length)
{
   int packetId;
   MQTT_PARSE_SHORT(subscribe, packetId)

   TMqttSubscribe* subscribeArg = new TMqttSubscribe;
   subscribeArg->clientName = mClientName;
   subscribeArg->msgId = packetId;

   while(length > 0)
   {
      CStr topic;
      if(!CMqttMsg::parseString(topic, p, length))
      {
         CMqttMsg::parseError("subscribe", "topic");
         delete subscribeArg;
         return;
      }
      int qos;
      if(!CMqttMsg::parseByte(qos, p, length))
      {
         CMqttMsg::parseError("subscribe", "qos");
         delete subscribeArg;
         return;
      }
      subscribeArg->topic.push_back(topic);
   }
   invokeService("subscribe", MQTT_P_SUBSCRIBE, subscribeArg);
}

void CMqttServerLink::processUnsubscribe(const char* p, int length)
{
   int packetId;
   MQTT_PARSE_SHORT(subscribe, packetId)

   TMqttUnsubscribe* unsubscribeArg = new TMqttUnsubscribe;
   unsubscribeArg->clientName = mClientName;
   unsubscribeArg->msgId = packetId;

   while(length > 0)
   {
      CStr topic;
      if(!CMqttMsg::parseString(topic, p, length))
      {
         CMqttMsg::parseError("unsubscribe", "topic");
         delete unsubscribeArg;
         return;
      }
      unsubscribeArg->topic.push_back(topic);
   }
   invokeService("unsubscribe", MQTT_P_UNSUBSCRIBE, unsubscribeArg);
}

void CMqttServerLink::sendSubAck(int msgId, int code)
{
   CStr body;
   CMqttMsg::appendShort(body, msgId);
   body += (char)(code);
   CStr packet;
   CMqttMsg::pack(packet, MQTT_P_SUBACK, body);
   sendPacket(packet);
}

void CMqttServerLink::sendUnsubAck(int msgId)
{
   CStr body;
   CMqttMsg::appendShort(body, msgId);
   CStr packet;
   CMqttMsg::pack(packet, MQTT_P_UNSUBACK, body);
   sendPacket(packet);
}

void CMqttServerLink::sendPingResp()
{
   CStr msg;
   CMqttMsg::pack(msg, MQTT_P_PINGRESP);
   sendPacket(msg);
}

void CMqttServerLink::sendConnAck(bool sessionPresent, int code)
{
   CStr body;
   body += (char)sessionPresent;
   body += (char)code;
   CStr connAckPacket;
   CMqttMsg::pack(connAckPacket, MQTT_P_CONNACK, body);
   sendPacket(connAckPacket);
   UniINFO("send connack message: %d,%d", sessionPresent, code);

   if(code != 0)
      closeLink();
}

CMqttServerLink::CMqttServerLink()
{
   mParent = NULL;
}

//messages received from external network??
//where is this message
BOOL CMqttServerLink::procAppMsg(TUniNetMsg* msg)
{
   switch(msg->msgName)
   {
   case MQTT_PUBLISH:
   {
      TMqttPublish* publishArg = (TMqttPublish*)(msg->msgBody);
      sendPublish(publishArg->topic, publishArg->content);
      return TRUE;
   }
   case MQTT_CONNACK:
   {
      TMqttConnAck* connAckArg = (TMqttConnAck*)(msg->msgBody);
      sendConnAck(false, connAckArg->code);
      return TRUE;
   }
   case MQTT_SUBACK:
   {
      TMqttSubAck* subAckArg = (TMqttSubAck*)(msg->msgBody);
      sendSubAck(subAckArg->msgId, subAckArg->code);
      return TRUE;
   }
   case MQTT_UNSUBACK:
   {
      TMqttUnsubAck* unsubAckArg = (TMqttUnsubAck*)(msg->msgBody);
      sendUnsubAck(unsubAckArg->msgId);
      return TRUE;
   }
   default:
      UniERROR("server link received invalid msg %s from app", msg->getMsgNameStr());
      return FALSE;
   }
}

void CMqttServerLink::secAction()
{
   if(mKeepAlive <= 0)
      return;
   if(!alive())
      return;
   mIdleSec++;
   if(mIdleSec > mKeepAlive)
   {
      UniERROR("link idle %d seconds, close", mIdleSec);
      closeLink();
   }
}

void CMqttServerLink::closeLink()
{
}
///////////////////////////////////////////////////////
// CMqttClientLink
///////////////////////////////////////////////////////

CMqttClientLink::CMqttClientLink()
{
   mPingTimer = 0;
}

void CMqttClientLink::msgProcess(int packetType, int flags, const char* p, int length)
{
   switch(packetType)
   {
   case MQTT_P_PUBLISH:
      processPublish(p, length, flags);
      return;
   case MQTT_P_SUBACK:
      processSubAck(p, length);
      return;
   case MQTT_P_UNSUBACK:
      processUnsubAck(p, length);
      return;
   case MQTT_P_CONNACK:
      processConnAck(p, length);
      return;
   case MQTT_P_PINGRESP:
      return;
   default:
      UniERROR("received invalid packet %d for client link", packetType);
      return;
   }
}

void CMqttClientLink::processSubAck(const char* p, int length)
{
   int packetId;
   MQTT_PARSE_SHORT(suback, packetId)
   int code;
   MQTT_PARSE_BYTE(suback, code)

   TMqttSubAck* subAckArg = new TMqttSubAck;
   subAckArg->msgId = packetId;
   subAckArg->code = code;

   TMsgAddress addr;
   if(getAppAddr(packetId, addr))
      responseService(MQTT_SUBACK, addr, subAckArg);
}

void CMqttClientLink::processUnsubAck(const char* p, int length)
{
   int packetId;
   MQTT_PARSE_SHORT(unsuback, packetId)

   TMqttUnsubAck* unsubAckArg = new TMqttUnsubAck;
   unsubAckArg->msgId = packetId;

   //no need to use addr,make sure that
   //try to find if we need to delete some code
   TMsgAddress addr;
   if(getAppAddr(packetId, addr))
      responseService(MQTT_UNSUBACK, addr, unsubAckArg);
}

void CMqttClientLink::processConnAck(const char* p, int length)
{
   int code;
   MQTT_PARSE_BYTE(connack, code)
   UniINFO("received connack message: %d", code);
   if(code == 0)
      mAlive = true;
}

void CMqttClientLink::processPublish(const char* p, int length, int flags)
{
   if(flags != 0)
   {
      UniERROR("only support publish message with Qos 0");
      return;
   }
   CStr topicName;
   MQTT_PARSE_STRING(publish, topicName)

   CStr content;
   content.set(p, length);

   TMqttPublish* publishArg = new TMqttPublish;
   publishArg->topic = topicName;
   publishArg->content = content;
   invokeService("publish", MQTT_PUBLISH, publishArg);
}

void CMqttClientLink::sendConnect(CStr& name, CStr& user, CStr& passwd, int keepAlive)
{
   cout << "Before encoding,name: " << name.c_str() << ",user: " << user.c_str() << ",passwd: " << passwd.c_str() << ",keepAlive: " << keepAlive << endl;
   mKeepAlive = keepAlive;

   CStr body;

   //cout << "CMqttClientLink::sendConnnect: size:" << body.size() << endl;
   CMqttMsg::appendStr(body, "MQTT");

   //cout << "Body: " << body.c_str() << endl;

   char protocolLevel = 4;
   body += protocolLevel;

   // User Name Flag (1)
   // Password Flag (1)
   // Will Retain (0)
   // Will QoS (00)
   // Will Flag (0)
   // Clean Session (1)
   // Reserved (0)
   char connectFlags = 0xC2;
   body += connectFlags;

   //the purpose of following code
   //modified by wangpan
   keepAlive=htonl(keepAlive);
   body.nCat((char *)&keepAlive+2, 2);

   //cout << "Body: " << body.c_str() << endl;

   CMqttMsg::appendStr(body, name.c_str());
   CMqttMsg::appendStr(body, user.c_str());
   CMqttMsg::appendStr(body, passwd.c_str());


   //cout << "size of msg before pack: " << body.GetVarCharLen() << endl;
   CStr msg;
   CMqttMsg::pack(msg, MQTT_P_CONNECT, body);

   //cout << "size of msg: " << msg.GetVarCharLen() << endl;

   //after encoding,we try to decode it in place and check if we are right

  //  cout << "###Before inplace parsing" << endl;
  //  msgParse(msg.c_str(),msg.GetVarCharLen());
  //  cout << "###After inplace parsing" << endl;

  //  /////////////////////////////////////////////////////////////////////
  //  for( int i=0 ; i<msg.GetVarCharLen() ; ++i )
  //  {
  //    printf("%x,",msg.getByte(i));
  //  }
  //  printf("\n");
   sendPacket(msg);
   UniINFO("send connect message");
}

void CMqttClientLink::sendSubscribe(CList<CStr>& topics, int packetId)
{
   CStr body;
   CMqttMsg::appendShort(body, packetId);
   if(topics.size() > 0)
   {
      for(CList<CStr>::iterator i = topics.begin(); i != topics.end(); i++)
      {
         CMqttMsg::appendStr(body, (*i).c_str());
         body += (char)0; // qos
      }
   }
   CStr msg;
   CMqttMsg::pack(msg, MQTT_P_SUBSCRIBE, body);

   //to look at implementation of sendPacket
   sendPacket(msg);
}

void CMqttClientLink::sendPingReq()
{
   CStr msg;
   CMqttMsg::pack(msg, MQTT_P_PINGREQ);
   sendPacket(msg);
}

void CMqttClientLink::sendDisconnect()
{
   CStr msg;
   CMqttMsg::pack(msg, MQTT_P_DISCONNECT);
   sendPacket(msg);
   mAlive = false;
}

void CMqttClientLink::sendUnsubscribe(CList<CStr>& topics, int packetId)
{
   CStr body;
   CMqttMsg::appendShort(body, packetId);
   if(topics.size() > 0)
   {
      for(CList<CStr>::iterator i = topics.begin(); i != topics.end(); i++)
         CMqttMsg::appendStr(body, (*i).c_str());
   }
   CStr msg;
   CMqttMsg::pack(msg, MQTT_P_UNSUBSCRIBE, body);
   sendPacket(msg);
}

BOOL CMqttClientLink::procAppMsg(TUniNetMsg* msg)
{
   switch(msg->msgName)
   {
   case MQTT_PUBLISH:
   {
      TMqttPublish* publishArg = (TMqttPublish*)(msg->msgBody);
      sendPublish(publishArg->topic, publishArg->content);
      return TRUE;
   }
   case MQTT_SUBSCRIBE:
   {
      TMqttSubscribe* subscribeArg = (TMqttSubscribe*)(msg->msgBody);
      if(subscribeArg == NULL)
      {
         UniERROR("MQTT_SUBSCRIBE message body is null");
         return FALSE;
      }
      sendSubscribe(subscribeArg->topic, newPacketId(msg->oAddr));
      return TRUE;
   }
   case MQTT_UNSUBSCRIBE:
   {
      TMqttUnsubscribe* unsubscribeArg = (TMqttUnsubscribe*)(msg->msgBody);
      if(unsubscribeArg == NULL)
      {
         UniERROR("MQTT_SUBSCRIBE message body is null");
         return FALSE;
      }
      sendUnsubscribe(unsubscribeArg->topic, newPacketId(msg->oAddr));
      return TRUE;
   }
   default:
      UniERROR("client link received invalid msg %s from app", msg->getMsgNameStr());
      return FALSE;
   }
}

void CMqttClientLink::secAction()
{
   if(mKeepAlive <= 0)
      return;
   if(!alive())
      return;
   mIdleSec++;
   if(mIdleSec > mKeepAlive)
   {
      UniERROR("link idle %d seconds, close", mIdleSec);
      closeLink();
   }
   mPingTimer--;
   if(mPingTimer <= 0)
   {
      sendPingReq();
      mPingTimer = mKeepAlive == 1 ? 1 : mKeepAlive / 2;
   }
}

void CMqttClientLink::closeLink()
{
   sendDisconnect();
}
