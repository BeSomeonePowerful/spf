#include "clientselftask.h"

ClientSelfTask::ClientSelfTask()
{

}

ClientSelfTask::~ClientSelfTask()
{

}

BOOL ClientSelfTask::onInit(TiXmlElement *extend)
{
  //init generateTimer from configuration file
  //do we need to delete every timer,after it's not needed anymore
  generateTimer = setTimer(GEN_TIMER);
  cnt = 0;
}

void ClientSelfTask::procMsg(std::auto_ptr<TUniNetMsg> msg)
{
  //client self task only need to echo it
  TUniNetMsg *unimsg = msg.get();
  if( unimsg!=NULL )
  {
    if( unimsg->oAddr.logAddr==102 )
    {
      switch( unimsg->msgName )
      {
        case MQTT_P_CONNACK:
          {
            TMqttConnAck *arg = (TMqttConnAck *)msg->msgBody;
            UniINFO("receiving conack: code: %d",arg->code);
          }
          break;
        case MQTT_P_PUBLISH:
          {
            TMqttPublish *arg = (TMqttPublish *)msg->msgBody;
            UniINFO("receiving publish: topic: %s,content: %s",arg->topic.c_str(),
            arg->content.c_str());
          }
          break;
        case MQTT_P_SUBACK:
          {
            TMqttSubAck *arg = (TMqttSubAck *)msg->msgBody;
            UniINFO("receiving suback: msgid: %d,code: %d",arg->msgId,arg->code);
          }
          break;
        case MQTT_P_UNSUBACK:
          {
            TMqttUnsubAck *arg = (TMqttUnsubAck *)msg->msgBody;
            UniINFO("receiving unsuback: msgId: %d",arg->msgId);
          }
          break;
        case MQTT_P_DISCONNECT:
          {
            UniINFO("receiving disconnect");
          }
          break;
        default:
          UniERROR("receiving an invalid message: type: %d",msg->msgName);
          break;
      }
    }
  }
}

void ClientSelfTask::sendToTask(TUniNetMsg* msg)
{
  msg->tAddr.logAddr = 102;
  msg->dialogType = DIALOG_BEGIN;
  sendMsg(msg);
}

void ClientSelfTask::onTimeOut(TTimerKey timerKey,void *para)
{
  if( timerKey==generateTimer )
  {
    //generate pub messages regularly
    TUniNetMsg *msg = new TUniNetMsg;
    msg->msgName = MQTT_P_PUBLISH;
    TMqttPublish *body = new TMqttPublish;
    body->clientName << "MqttClient";
    body->topic << "/mqttclient/pub";
    body->content << cnt;
    msg->setMsgBody(body);
    sendToTask(msg);
    ++cnt;
    generateTimer = setTimer(GEN_TIMER);
  }
}
