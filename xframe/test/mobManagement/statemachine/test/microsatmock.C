#include "microsatmock.h"
#include "../mobmsgtype.h"
#include "../microsat/addrresolvesm.h"
#include "msgdef_rsc.h"

MicroSatMock::MicroSatMock()
{
  ridgenerator = new RidGenerator;
}

MicroSatMock::~MicroSatMock()
{
  if( ridgenerator!=NULL )
    delete ridgenerator;
}

BOOL MicroSatMock::onInit(TiXmlElement *extend)
{
  TiXmlHandle handle(extend);
  TiXmlElement *task = NULL;
  task = handle.FirstChild("task").Element();
  if( task )
  {
    if( !task->Attribute("rscpsaaddr",&rscpsaaddr))
      UniERROR("rscpsaaddr not set");
    const char *tmp = task->Attribute("mainsatip");
    if( tmp==NULL || strlen(tmp)==0 )
      UniERROR("mainsatip not set");
    mainsatIP << tmp;

    tmp = task->Attribute("localip");
    if( tmp==NULL || strlen(tmp)==0 )
      UniERROR("localip not set");
    localIP << tmp;
  }
  UniINFO("rscpsaaddr: %d,mainsatIP: %s,localIP: %s",rscpsaaddr,mainsatIP.c_str(),
    localIP.c_str());
  addrResolveTimer = setTimer(1);
  return TRUE;
}

void MicroSatMock::procMsg(std::auto_ptr<TUniNetMsg> msg)
{
  TUniNetMsg *unimsg = msg.get();
  if( unimsg!=NULL )
  {
    if( unimsg->oAddr.logAddr==rscpsaaddr && unimsg->hasMsgBody() )
    {
      TRscMsgHdr *rschdr = dynamic_cast<TRscMsgHdr *>(msg->getCtrlMsgHdr());
      TRscMsgBody *rscbody = dynamic_cast<TRscMsgBody *>(msg->getMsgBody());
      CStr rid = rschdr->rid;
      if( smManager.find(rid)==smManager.end() )
      {
        switch( rschdr->code )
        {
          //
          // case :
          //   break;
          default:
            break;
        }
      }
      else
      {
        MobStateMachine *sm = smManager[rid];
        sm->onRecv(unimsg);
        if( sm->hasDone() )
        {
          delete smManager[rid];
          smManager.erase(rid);
        }
      }
    }
  }
}

void MicroSatMock::onTimeOut(TTimerKey timerKey,void *para)
{
  UniINFO("calling MicroSatMock::onTimeOut method");
  //we call
  if( timerKey==addrResolveTimer )
  {
    startAddrResolve(1);
    startAddrResolve(2);
    delTimer(timerKey);
  }
}

//need to call sm to make state transitions
void MicroSatMock::sendToTask(TUniNetMsg *msg)
{
  TRscMsgHdr *rschdr = dynamic_cast<TRscMsgHdr *>(msg->getCtrlMsgHdr());
  TRscMsgBody *rscbody = dynamic_cast<TRscMsgBody *>(msg->getMsgBody());
  CStr rid = rschdr->rid;
  if( smManager.find(rid)==smManager.end() )
  {
    UniERROR("Invalid msg: rid: %s",rid.c_str());
  }
  else
  {
    MobStateMachine *sm = smManager[rid];
    sm->onSend(msg);
    if( sm->hasDone() )
    {
      delete sm;
      smManager.erase(rid);
    }
    sendMsg(msg);
  }
}

void MicroSatMock::startAddrResolve(int userId)
{
  UniINFO("calling MicroSatMock::startAddrResolve ");
  //build a get msg
  TUniNetMsg *msg = new TUniNetMsg;
  TRscMsgHdr *rschdr = new TRscMsgHdr;
  TRscMsgBody *rscbody = new TRscMsgBody;
  TURI t = TURI(mainsatIP);
  rschdr->host = t;
  rschdr->code = RSC_MOB_GET;
  rschdr->ruri = "/get/satip";
  rschdr->consumer=mainsatIP;
  rschdr->producer=localIP;
  rschdr->rid << ridgenerator->getNextRid();

  //using sprintf to complete the formatting of string
  char tmp[100];
  sprintf(tmp,"{\"uid\":%d}",userId);
  rscbody->rsc = tmp;

  msg->tAddr.logAddr = rscpsaaddr;
  msg->msgName = RSC_MESSAGE;
  msg->dialogType = DIALOG_MESSAGE;
  msg->msgType = RSC_MESSAGE_TYPE;

  msg->setCtrlMsgHdr(rschdr);
  msg->setMsgBody(rscbody);
  //build corresponding sm
  MobStateMachine *sm = new AddrResolveSm(this,userId);
  smManager[rschdr->rid]=sm;
  //call sendToTask
  sendToTask(msg);
}

void MicroSatMock::startUaReg(int uaid,CStr uaip)
{

}

void MicroSatMock::startUserReg(int userid,int uaid)
{
  
}
