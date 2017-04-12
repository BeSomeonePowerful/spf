#include "uaipresolvesm.h"
#include "../mobmsgtype.h"
#include "../rapidjson/document.h"
#include "../test/mainsatmock.h"

UaipResolveSm::UaipResolveSm(MainSatMock *_task)
{
  task = _task;
  curState = INIT_STATE;
  done = 0;
  msg = NULL;
}

UaipResolveSm::~UaipResolveSm()
{

}

void UaipResolveSm::onRecv(TUniNetMsg *msg)
{
  TRscMsgHdr *rschdr = dynamic_cast<TRscMsgHdr *>(msg->getCtrlMsgHdr());
  TRscMsgBody *rscbody = dynamic_cast<TRscMsgBody *>(msg->getMsgBody());
  if( rschdr==NULL || rscbody==NULL )
  {
    UniERROR("AddrResolveSm::onRecv: null pointer");
  }
  UniINFO("receiving a %x msg,uri: %s,rsc: %s,oAddr.logAddr: %d",rschdr->code,rschdr->ruri.c_str(),
    rscbody->rsc.c_str(),msg->oAddr.logAddr);
  CStr uri = rschdr->ruri;
  switch( rschdr->code )
  {
    case RSC_MOB_GET:
      if( uri.index("/uaip")!=-1 )
      {
        curState = RECV_GET_UAIP;
        //build new msg,send to hss(nme)
        this->ipport = rschdr->producer;
        TUniNetMsg *unimsg = MobMsgHelper::buildRscMsg();
        TRscMsgHdr *newrschdr = dynamic_cast<TRscMsgHdr *>(unimsg->getCtrlMsgHdr());
        TRscMsgBody *newrscbody = dynamic_cast<TRscMsgBody *>(unimsg->getMsgBody());
        MainSatMock *t = (MainSatMock *)task;
        unimsg->tAddr.logAddr = t->nmeaddr;

        newrschdr->code = RSC_MOB_GET;
        CStr host = CStr(t->localIP);
        newrschdr->host = TURI(host);
        newrschdr->rid = rschdr->rid;
        newrschdr->ruri = "/get/uaip";
        newrschdr->consumer = t->localIP;
        newrschdr->producer = t->localIP;
        newrscbody->rsc = rscbody->rsc;
        t->sendToTask(unimsg);
        //UAId is get from certain file
      }
      break;
    case RSC_MOB_GETACK:
      if( uri.index("/uaip")!=-1 )
      {
        curState = RECV_GETACK_UAIP;

        //build getack msg,send to cme-u
        TUniNetMsg *unimsg = MobMsgHelper::buildRscMsg();
        TRscMsgHdr *newrschdr = dynamic_cast<TRscMsgHdr *>(unimsg->getCtrlMsgHdr());
        TRscMsgBody *newrscbody = dynamic_cast<TRscMsgBody *>(unimsg->getMsgBody());
        MainSatMock *t = (MainSatMock *)task;
        unimsg->tAddr.logAddr = t->rscpsaaddr;

        newrschdr->code = RSC_MOB_GETACK;
        CStr host = CStr(this->ipport);
        newrschdr->host = TURI(host);
        newrschdr->rid = rschdr->rid;
        newrschdr->ruri = "/getack/uaip";
        newrschdr->consumer = this->ipport;
        newrschdr->producer = t->localIP;
        newrscbody->rsc = rscbody->rsc;
        t->sendToTask(unimsg);
      }
      break;
    default:
      break;
  }
}

void UaipResolveSm::onSend(TUniNetMsg *msg)
{
  TRscMsgHdr *rschdr = dynamic_cast<TRscMsgHdr *>(msg->getCtrlMsgHdr());
  TRscMsgBody *rscbody = dynamic_cast<TRscMsgBody *>(msg->getMsgBody());
  if( rschdr==NULL || rscbody==NULL )
  {
    UniERROR("AddrResolveSm::onRecv: null pointer");
  }
  UniINFO("sending a %x msg,uri: %s,rsc: %s,tAddr.logAddr: %d",rschdr->code,rschdr->ruri.c_str(),
    rscbody->rsc.c_str(),msg->tAddr.logAddr);
  CStr uri = rschdr->ruri;
  switch( rschdr->code )
  {
    case RSC_MOB_GET:
      if( uri.index("/uaip")!=-1 )
      {
        curState = SEND_GET_UAIP;
      }
      break;
    case RSC_MOB_GETACK:
      if( uri.index("/uaip")!=-1 )
      {
        curState = END_STATE;
        done = 1;
      }
      break;
    default:
      break;
  }
}
