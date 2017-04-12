#include "subselftask.h"

SubSelfTask::SubSelfTask()
{

}

SubSelfTask::~SubSelfTask()
{

}

BOOL SubSelfTask::onInit(TiXmlElement *extend)
{
  //init generateTimer from configuration file
  //do we need to delete every timer,after it's not needed anymore
  generateTimer = setTimer(GEN_TIMER);
  msgId = 0;
}

void SubSelfTask::procMsg(std::auto_ptr<TUniNetMsg> msg)
{
  //client self task only need to echo it
  TUniNetMsg *unimsg = msg.get();
  if( unimsg!=NULL )
  {
    if( unimsg->oAddr.logAddr==104 )
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

void SubSelfTask::sendToTask(TUniNetMsg* msg)
{
  msg->tAddr.logAddr = 104;
  msg->dialogType = DIALOG_BEGIN;
  sendMsg(msg);
}

void SubSelfTask::onTimeOut(TTimerKey timerKey,void *para)
{
  if( timerKey==generateTimer )
  {
    //generate pub messages regularly
      TUniNetMsg *msg = new TUniNetMsg;
      msg->msgName = MQTT_P_SUBSCRIBE;
      TMqttSubscribe *body = new TMqttSubscribe;
      body->clientName << "MqttSubClient";
      body->msgId = msgId;
      CStr topic = "/mqttclient/pub";
      body->topic.push_back(topic);
      msg->setMsgBody(body);
      sendToTask(msg);
      //mqtt->wantToSend(shared_ptr<TUniNetMsg>(msg));

      //to prevent this task being blocked on a empty message queue
      //we reset the gen_timer
      generateTimer = setTimer(GEN_TIMER);
      ++msgId;
  }
}
