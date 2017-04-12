#include "statemachine.h"
#include "mqttlink.h"

MqttClientStateMachine::MqttClientStateMachine()
{
  msg=NULL;
  curState=CLIENT_INIT_STATE;
  preState=CLIENT_INIT_STATE;
  smChange=false;
  resendTimer=0;
  keepaliveTimer=0;
  resendCnt=0;
}
//called before message being send
//when send a message,we have to make a copy of the message or just a pointer??
CLIENT_SM_CODE MqttClientStateMachine::onSend(shared_ptr<TUniNetMsg> msg)
{
  switch( msg->msgName )
  {
    case MQTT_P_CONNECT:
      {
        if( curState!=CLIENT_INIT_STATE )
        {
          UniERROR("###ERRRO:trying to send a connect while not in INIT_STATE");
          return CLIENT_SEND_REFUSE;
        }
        this->msg = msg;
        preState = curState;
        curState = WAIT_CONACK_STATE;
        smChange = true;
        resendTimer = link->setTimer(RESEND_TIMER);
        return CLIENT_SEND_FORWARD;
      }
      break;
    case MQTT_P_PUBLISH:
      {
        if( curState!=CLIENT_CONNECTED_STATE )
        {
          UniERROR("###ERRRO:trying to send a publish while not in CONNECTED_STATE");
          return CLIENT_SEND_REFUSE;
        }
        this->msg = msg;
        preState = curState;
        smChange = false;
        keepaliveTimer = link->setTimer(ALIVE_TIMER);
        return CLIENT_SEND_FORWARD;
      }
      break;
    case MQTT_P_SUBSCRIBE:
      {
        if( curState!=CLIENT_CONNECTED_STATE )
        {
          UniERROR("###ERRRO:trying to send a subscribe while not in CONNECTED_STATE");
          return CLIENT_SEND_REFUSE;
        }
        this->msg = msg;
        preState = curState;
        curState = WAIT_SUBACK_STATE;
        smChange = true;
        resendTimer = link->setTimer(RESEND_TIMER);
        keepaliveTimer = link->setTimer(ALIVE_TIMER);
        return CLIENT_SEND_FORWARD;
      }
      break;
    case MQTT_P_UNSUBSCRIBE:
      {
        if( curState!=CLIENT_CONNECTED_STATE )
        {
          UniERROR("###ERRRO:trying to send a unsubscribe while not in CONNECTED_STATE");
          return CLIENT_SEND_REFUSE;
        }
        this->msg = msg;
        preState = curState;
        curState = WAIT_UNSUBACK_STATE;
        smChange = true;
        resendTimer = link->setTimer(RESEND_TIMER);
        keepaliveTimer = link->setTimer(ALIVE_TIMER);
        return CLIENT_SEND_FORWARD;
      }
      break;
    case MQTT_P_PINGREQ:
      {
        if( curState!=CLIENT_CONNECTED_STATE )
        {
          UniERROR("###ERRRO:trying to send a pingreq while not in CONNECTED_STATE");
          return CLIENT_SEND_REFUSE;
        }
        this->msg = msg;
        preState = curState;
        curState = WAIT_PINGRESP_STATE;
        smChange = true;
        resendTimer = link->setTimer(RESEND_TIMER);
        keepaliveTimer = link->setTimer(ALIVE_TIMER);
        return CLIENT_SEND_FORWARD;
      }
      break;
    case MQTT_P_DISCONNECT:
      {
        //can be send as long as in any state other than init_state
        if( curState==CLIENT_INIT_STATE )
        {
          UniERROR("###ERRRO:trying to send a disconnect while not in CONNECTED_STATE");
          return CLIENT_SEND_REFUSE;
        }
        this->msg = msg;
        preState = curState;
        curState = CLIENT_END_STATE;
        smChange = true;
        return CLIENT_SEND_FORWARD;
      }
      break;
    default:
      UniERROR("invalid message type: %d from client",msg->msgName);
      return CLIENT_SEND_REFUSE;
      break;
  }
}

CLIENT_SM_CODE MqttClientStateMachine::onRecv(TUniNetMsg *msg)
{
  switch( msg->msgName )
  {
    case MQTT_P_PUBLISH:
      {
        return CLIENT_RECV_FORWARD;
      }
      break;
    case MQTT_P_CONNACK:
      {
        if( curState!=WAIT_CONACK_STATE )
        {
          UniERROR("received a conack message while not in WAIT_CONACK_STATE");
          return CLIENT_RECV_REFUSE;
        }
        preState = curState;
        curState = CLIENT_CONNECTED_STATE;
        smChange = true;
        if( resendTimer!=0 ) { link->delTimer(resendTimer); resendTimer=0; }
        keepaliveTimer = link->setTimer(ALIVE_TIMER);
        return CLIENT_RECV_FORWARD;
      }
      break;
    case MQTT_P_SUBACK:
      {
        if( curState!=WAIT_SUBACK_STATE )
        {
          UniERROR("received a suback message while not in WAIT_SUBACK_STATE");
          return CLIENT_RECV_REFUSE;
        }
        preState = curState;
        curState = CLIENT_CONNECTED_STATE;
        smChange = true;
        if( resendTimer!=0 ) { link->delTimer(resendTimer); resendTimer=0; }
        keepaliveTimer = link->setTimer(ALIVE_TIMER);
        return CLIENT_RECV_FORWARD;
      }
      break;
    case MQTT_P_UNSUBACK:
      {
        if( curState!=WAIT_UNSUBACK_STATE )
        {
          UniERROR("received a unsuback message while not in WAIT_UNSUBACK_STATE");
          return CLIENT_RECV_REFUSE;
        }
        preState = curState;
        curState = CLIENT_CONNECTED_STATE;
        smChange = true;
        if( resendTimer!=0 ) { link->delTimer(resendTimer); resendTimer=0; }
        keepaliveTimer = link->setTimer(ALIVE_TIMER);
        return CLIENT_RECV_FORWARD;
      }
      break;
    case MQTT_P_PINGRESP:
      {
        if( curState!=WAIT_PINGRESP_STATE )
        {
          UniERROR("received a pingresp message while not in WAIT_PINGRESP_STATE");
          return CLIENT_RECV_REFUSE;
        }
        preState = curState;
        curState = CLIENT_CONNECTED_STATE;
        smChange = true;
        if( resendTimer!=0 ) { link->delTimer(resendTimer); resendTimer=0; }
        keepaliveTimer = link->setTimer(ALIVE_TIMER);
        return CLIENT_RECV_FORWARD;
      }
      break;
    case MQTT_P_DISCONNECT:
      {
        preState = curState;
        curState = CLIENT_END_STATE;
        smChange = true;
        if( resendTimer!=0 ) { link->delTimer(resendTimer); resendTimer=0; }
        if( keepaliveTimer!=0 ) { link->delTimer(keepaliveTimer); keepaliveTimer=0;}
        return CLIENT_RECV_FORWARD;
        //when a disconnect is received ,all timers should be reset
      }
      break;
    default:
      {
        UniERROR("invalid message type: %d",msg->msgName);
        return CLIENT_RECV_REFUSE;
      }
      break;
  }
}

CLIENT_SM_CODE MqttClientStateMachine::onTimeOut(TTimerKey timerKey,void *para,
  shared_ptr<TUniNetMsg> &resend)
{
  //resend = NULL;
  if( timerKey==resendTimer )
  {
    //how to set state when resendTimer is TimeOut
    resend = msg;
    //at some time ,the ownership of the resource is transfered,other timer,they shared the
    //the owenership.
    curState = preState;
    smChange = true;
    link->delTimer(resendTimer);
    return CLIENT_TIMEOUT_FORWARD;
    //then the resend message may be readd to wantToSend queue
    //thus causing the state machine to be called a second time
  }
  else if( timerKey==keepaliveTimer )
  {
    //build pingreq message
    //add it to wantToSend queue
    //msgdef_mqtt has not def a PINGREQ or PINGRESP message
    //as such,we don't support keepalive timer
    UniINFO("keepaliveTimer timeOut");
    link->delTimer(keepaliveTimer);
    keepaliveTimer = 0;
    // TUniNetMsg *unimsg = new TUniNetMsg;
    // unimsg->msgName = MQTT_P_PINGREQ;
    // link->delTimer(keepaliveTimer);
    // return CLIENT_TIMEOUT_FORWARD;
    return CLIENT_TIMEOUT_FORWARD;
  }
  else
  {
    UniERROR("invalid timerKey: %d",timerKey);
    return CLIENT_TIMEOUT_REFUSE;
  }
}

MqttServerStateMachine::MqttServerStateMachine()
{
  msg = NULL;
  curState = SERVER_INIT_STATE;
  preState = SERVER_INIT_STATE;
  smChange = false;
}

SERVER_SM_CODE MqttServerStateMachine::onSend(shared_ptr<TUniNetMsg> msg)
{
  switch( msg->msgName )
  {
    case MQTT_P_CONNACK:
      {
        if( curState!=RECV_CON_STATE )
        {
          UniERROR("trying to send a conack while not in RECV_CON_STATE: linkid:%d",
            link->getLinkId());
          return SERVER_SEND_REFUSE;
        }
        preState = curState;
        curState = SERVER_CONNECTED_STATE;
        smChange = true;
        return SERVER_SEND_FORWARD;
      }
      break;
    case MQTT_P_PUBLISH:
      {
        return SERVER_SEND_FORWARD;
      }
      break;
    case MQTT_P_SUBACK:
      {
        if( curState!=RECV_SUB_STATE )
        {
          UniERROR("trying to send a subck while not in RECV_SUB_STATE: linkid:%d",
            link->getLinkId());
          return SERVER_SEND_REFUSE;
        }
        preState = curState;
        curState = SERVER_CONNECTED_STATE;
        smChange = true;
        return SERVER_SEND_FORWARD;
      }
      break;
    case MQTT_P_UNSUBACK:
      {
        if( curState!=RECV_UNSUB_STATE )
        {
          UniERROR("trying to send a unsubck while not in RECV_UNSUB_STATE: linkid:%d",
            link->getLinkId());
          return SERVER_SEND_REFUSE;
        }
        preState = curState;
        curState = SERVER_CONNECTED_STATE;
        smChange = true;
        return SERVER_SEND_FORWARD;
      }
      break;
    default:
      {
        UniERROR("unsupported response:: %d,linkid: %d",msg->msgName,link->getLinkId());
        return SERVER_SEND_REFUSE;
      }
      break;
  }
}


//what to do in server-side
//when a pub or sub message is recved ;sublist need to be maintained by server
//when a discon is recved; which message should be forwarded to application task;
//we forward all message to application,application can further sendback response
//to do things like closing the link
//one application task per server-side link,per task per link currrently
SERVER_SM_CODE MqttServerStateMachine::onRecv(TUniNetMsg *msg)
{
  switch( msg->msgName )
  {
    case MQTT_P_CONNECT:
      {
        if( curState!=SERVER_INIT_STATE )
        {
          UniERROR("recved a connect message while not in INIT_STATE: linkid:%d ",
            link->getLinkId());
          return SERVER_RECV_REFUSE;
        }
        //build ack message and add to wantToSend Queue
        //in our programming model,now the application is responsible
        //for building ack message
        preState = curState;
        curState = RECV_CON_STATE;
        smChange = true;
        return SERVER_RECV_FORWARD;
      }
      break;
    case MQTT_P_PUBLISH:
      {
        if( curState!=SERVER_CONNECTED_STATE )
        {
          UniERROR("recved a publish message while not in CONNNECTED_STATE: linkid: %d",
            link->getLinkId());
          return SERVER_RECV_REFUSE;
        }
        return SERVER_RECV_FORWARD;
      }
      break;
    case MQTT_P_SUBSCRIBE:
      {
        if( curState!=SERVER_CONNECTED_STATE )
        {
          UniERROR("recved a subscribe message while not in CONNNECTED_STATE: linkid: %d",
            link->getLinkId());
          return SERVER_RECV_REFUSE;
        }
        preState = curState;
        curState = RECV_SUB_STATE;
        smChange = true;
        return SERVER_RECV_FORWARD;
      }
      break;
    case MQTT_P_UNSUBSCRIBE:
      {
        if( curState!=SERVER_CONNECTED_STATE )
        {
          UniERROR("recved a unsubscribe message while not in CONNNECTED_STATE: linkid: %d",
            link->getLinkId());
          return SERVER_RECV_REFUSE;
        }
        preState = curState;
        curState = RECV_UNSUB_STATE;
        smChange = true;
        return SERVER_RECV_FORWARD;
      }
      break;
    case MQTT_P_PINGREQ:
      {
        //we don't support keepalive feature currently
      }
      break;
    case MQTT_P_DISCONNECT:
      {
        //when recv a disconnect message,connection should be closed

        preState = curState;
        curState = SERVER_END_STATE;
        smChange = true;
        return SERVER_RECV_FORWARD;
      }
      break;
    default:
      {
        UniERROR("invalid message type: %d",msg->msgName);
        return SERVER_RECV_REFUSE;
      }
      break;
  }
}
