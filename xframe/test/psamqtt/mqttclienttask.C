#include "mqttclienttask.h"
#include "mqttclient.h"

MqttClientTask::MqttClientTask()
{
  mqtt = new CMqttClientTcp();
  mqtt->setTask(this);
}

MqttClientTask::~MqttClientTask()
{
  mqtt->close();
  delete mqtt;
}

BOOL MqttClientTask::onInit(TiXmlElement *extend)
{
  TiXmlHandle handle(extend);
  TiXmlElement *task = NULL;
  task = handle.FirstChild("task").Element();
  if( task )
  {
    const char *tmp = task->Attribute("remoteIp");
    if( tmp!=NULL && strlen(tmp)!=0 ) remoteIp << tmp;
    else remoteIp << "127.0.0.1";
    if( !task->Attribute("remotePort",&remotePort)) remotePort = 9001;

    tmp = task->Attribute("clientName");
    if( tmp==NULL || strlen(tmp)==0 ) { UniERROR("clientName not set");}
    else { clientName << tmp; }
    tmp = task->Attribute("userName");
    if( tmp==NULL || strlen(tmp)==0 )  { UniERROR("userName not set"); }
    else userName << tmp;
    tmp = task->Attribute("password");
    if( tmp==NULL || strlen(tmp)==0 )  { UniERROR("password not set"); }
    else password << tmp;
    if( !task->Attribute("keepalive",&keepalive)) keepalive=10;
  }
  UniINFO("remoteIp = %s,remotePort = %d...",remoteIp.c_str(),remotePort);
  UniINFO("clientName = %s,userName = %s,password = %s,keepalive = %d",clientName.c_str(),
    userName.c_str(),password.c_str(),keepalive);

  connServerTimer = setTimer(CON_SERVER);

  // mqtt->init(remoteIp,remotePort,clientName,userName,password,keepalive);

  TUniNetMsg *msg = new TUniNetMsg;
  msg->msgName = MQTT_P_CONNECT;
  TMqttConnect *arg = new TMqttConnect;
  arg->clientName = clientName;
  arg->userName = userName;
  arg->password = password;
  msg->setMsgBody(arg);
  mqtt->wantToSend(shared_ptr<TUniNetMsg>(msg));
}

//client -side psatask is responsible for send con message on connection
//how to do this
//other kind of message should be send by app-task
//for client ,it's only one link

//for client,a statemachine should be created at constrution time
//for server,when a new link is created,a statemachine should be created

//whenerver a message is send or received,it should go throught the statemachine

//depending on the topic of the message,it should go which task or respond
//which code should go which part
//should be a mqttmessage


//what to do in state machine,the result of state machine
//if it needs to go forward to task or just send a response immediately

//distribute to task according to topic and message
//make all message topic-like,con-conack things are done by stack
//when a pub is recived,it should go to app
//what about sub and unsub??

void MqttClientTask::sendToTask(TUniNetMsg* msg)
{
  //should distribute to task using a mapping table,currently just send to task1001
  //if it is a pub mesage
  TUniNetMsg *unimsg = msg;
  unimsg->tAddr.logAddr=1002;
  unimsg->dialogType=DIALOG_BEGIN;
  sendMsg(unimsg);
}

//message from app
void MqttClientTask::procMsg(std::auto_ptr<TUniNetMsg> msg)
{
  TUniNetMsg *unimsg = msg.get();
  if( unimsg!=NULL )
  {
    if( unimsg->oAddr.logAddr==1002 && unimsg->hasMsgBody() )
      {
        //will there be a problem?
        //build a copy of the message
        TUniNetMsg *copy = new TUniNetMsg;
        copy->msgName = unimsg->msgName;
        //may have some problems here.....
        copy->msgBody = unimsg->msgBody->clone();

        //depending on the type of the message ,the body is copyed differently
        mqtt->wantToSend(shared_ptr<TUniNetMsg>(copy));
      }
  }
}

void MqttClientTask::onTimeOut(TTimerKey timerKey,void *para)
{
  if( timerKey==connServerTimer )
  {
    UniINFO("calling MqttClientTask::onTimeOut method");
    mqtt->init(remoteIp,remotePort,clientName,userName,password,keepalive);
    delTimer(connServerTimer);
  }
  else
  //if resend timer,then resend message
  //if keepalive timer,send other message
    mqtt->onTimeOut(timerKey,para);
}

void MqttClientTask::onBuildFDSet(CFdSet &fdset)
{
  mqtt->buildFdSet(fdset);
}

void MqttClientTask::onCheckFDSet(CFdSet &fdset)
{
  mqtt->procFdSet(fdset);
}
