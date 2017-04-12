#include "serverselftask.h"

ServerSelfTask::ServerSelfTask()
{

}

ServerSelfTask::~ServerSelfTask()
{

}

BOOL ServerSelfTask::onInit(TiXmlElement *extend)
{

}

void ServerSelfTask::sendToTask(TUniNetMsg *msg)
{
  msg->tAddr.logAddr = 101;
  sendMsg(msg);
}

void ServerSelfTask::procMsg(std::auto_ptr<TUniNetMsg> msg)
{
  UniINFO("ServerSelfTask::procMsg method being called");
  TUniNetMsg *unimsg = msg.get();
  if( unimsg!=NULL )
  {
    if( unimsg->oAddr.logAddr==101 )
    {
      //msgName is the type of the message,
      //then is the body of the message
      switch( unimsg->msgName )
      {
        case MQTT_P_CONNECT:
          {
            //response with a connack
            TMqttConnect *arg = (TMqttConnect *)unimsg->msgBody;
            UniINFO("receiving a connect message: clientName: %s,user: %s,password: %s",
              arg->clientName.c_str(),arg->userName.c_str(),arg->password.c_str());
            TUniNetMsg *msgtmp = new TUniNetMsg;
            msgtmp->msgName = MQTT_P_CONNACK;
            TMqttConnAck *body = new TMqttConnAck;
            body->code = 0;
            TComCtrlMsg *ctrl = new TComCtrlMsg;
            TComCtrlMsg *old = (TComCtrlMsg *)(unimsg->ctrlMsgHdr);
            ctrl->status = old->status;
            msgtmp->setCtrlMsgHdr(ctrl);
            msgtmp->setMsgBody(body);
            sendToTask(msgtmp);
          }
          break;
        case MQTT_P_PUBLISH:
          {
            //we need to transfer ctrl to psatask to do publish
            TMqttPublish *arg = (TMqttPublish *)unimsg->msgBody;
            UniINFO("receiving a pub message: topic: %s,content: %s",arg->topic.c_str(),
              arg->content.c_str());
          }
          break;
        case MQTT_P_SUBSCRIBE:
          {
            //we need to transfer ctrl to psatask to do sub
            //but it's a little complicated now
            //so we let the psatask do the sub first,then forward msg to app-task
            TMqttSubscribe *arg = (TMqttSubscribe *)unimsg->msgBody;
            UniINFO("receiving a sub message: msgId: %d",arg->msgId);
            for(CList<CStr>::iterator iter=(arg->topic).begin(); iter!=(arg->topic).end();
              ++iter)
            {
              UniINFO("topic: %s",(*iter).c_str());
            }
            TUniNetMsg *msgtmp = new TUniNetMsg;
            msgtmp->msgName = MQTT_P_SUBACK;
            TMqttSubAck *body = new TMqttSubAck;

            body->msgId = arg->msgId;
            body->code = 0;
            TComCtrlMsg *ctrl = new TComCtrlMsg;
            TComCtrlMsg *old = (TComCtrlMsg *)(unimsg->ctrlMsgHdr);
            ctrl->status = old->status;
            msgtmp->setMsgBody(body);
            msgtmp->setCtrlMsgHdr(ctrl);
            sendToTask(msgtmp);
          }
          break;
        case MQTT_P_UNSUBSCRIBE:
          {
            TMqttUnsubscribe *arg = (TMqttUnsubscribe *)unimsg->msgBody;
            UniINFO("receiving a unsub message: msgId: %d",arg->msgId);
            for(CList<CStr>::iterator iter=(arg->topic).begin(); iter!=(arg->topic).end();
              ++iter)
            {
              UniINFO("topic: %s",(*iter).c_str());
            }
            TUniNetMsg *msgtmp = new TUniNetMsg;
            msgtmp->msgName = MQTT_P_UNSUBACK;
            TMqttUnsubAck *body = new TMqttUnsubAck;

            body->msgId = arg->msgId;
            TComCtrlMsg *ctrl = new TComCtrlMsg;
            TComCtrlMsg *old = (TComCtrlMsg *)(unimsg->ctrlMsgHdr);
            ctrl->status = old->status;
            msgtmp->setMsgBody(body);
            msgtmp->setCtrlMsgHdr(ctrl);
            sendToTask(msgtmp);
          }
          break;
        case MQTT_P_DISCONNECT:
          {
            UniINFO("receiving a disconnect message");
          }
          break;
        default:
          UniERROR("invalid message: msgType: %d",unimsg->msgName);
          break;
      }
    }
  }
}

void ServerSelfTask::onTimeOut(TTimerKey timerKey,void *para)
{

}
