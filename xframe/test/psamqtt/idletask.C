#include "idletask.h"

IdleTask::IdleTask()
{

}

IdleTask::~IdleTask()
{

}

// IdleTask::IdleTask(const IdleTask &rhs):timer(rhs.timer),cnt(rhs.cnt)
// {
//   dstIds = rhs.dstIds;
// }

BOOL IdleTask::onInit(TiXmlElement *extend)
{
  UniINFO("IdleTask::onInit method being called");
  regTimer = setTimer(REG_TIMER);
  cnt = 0;
  return TRUE;
}

void IdleTask::procMsg(std::auto_ptr<TUniNetMsg> msg)
{

}

void IdleTask::onTimeOut(TTimerKey timerKey,void *para)
{
  if( timerKey==regTimer )
  {
    TUniNetMsg *msg = new TUniNetMsg;
    msg->tAddr.logAddr = 101;
    msg->dialogType = DIALOG_BEGIN;
    msg->msgName = COM_MESSAGE_REQ;
    TComMsg *body = new TComMsg;
    body->comMsg << "Idletask: " << cnt;
    msg->setMsgBody(body);
    sendMsg(msg);
    UniINFO("IdleTask::onTimeOut: %d",cnt);
    regTimer = setTimer(REG_TIMER);
    ++cnt;
  }
}
