#include "uaidresolvesm.h"
#include "../mobmsgtype.h"
#include "../rapidjson/document.h"
#include "../test/mainsatmock.h"

using namespace rapidjson;

UaidResolveSm::UaidResolveSm(MainSatMock *_task)
{
  task = _task;
  curState = INIT_STATE;
  done = 0;
  msg = NULL;
}

UaidResolveSm::~UaidResolveSm()
{

}

void UaidResolveSm::onRecv(TUniNetMsg *msg)
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
      if( uri.index("/uaid")!=-1 )
      {
        curState = RECV_GET_UAID;
        ipport = rschdr->producer;
        //build new msg,send to hss(nme)
        TUniNetMsg *unimsg = MobMsgHelper::buildRscMsg();
        TRscMsgHdr *newrschdr = dynamic_cast<TRscMsgHdr *>(unimsg->getCtrlMsgHdr());
        TRscMsgBody *newrscbody = dynamic_cast<TRscMsgBody *>(unimsg->getMsgBody());
        MainSatMock *t = (MainSatMock *)task;
        unimsg->tAddr.logAddr = t->nmeaddr;

        newrschdr->code = RSC_MOB_GET;

        newrschdr->host = TURI(t->localIP);
        newrschdr->rid = rschdr->rid;
        newrschdr->ruri = "/get/uaid";
        newrschdr->consumer = t->localIP;
        newrschdr->producer = t->localIP;
        newrscbody->rsc = rscbody->rsc;
        t->sendToTask(unimsg);
      }
      break;
    case RSC_MOB_GETACK:
      if( uri.index("/uaid")!=-1 )
      {
        curState = RECV_GETACK_UAID;
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
        newrschdr->ruri = "/getack/uaid";
        newrschdr->consumer = this->ipport;
        newrschdr->producer = t->localIP;
        newrscbody->rsc = rscbody->rsc;
        task->sendToTask(unimsg);
      }
      break;
    default:
      break;
  }
}

void UaidResolveSm::onSend(TUniNetMsg *msg)
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
      if( uri.index("/uaid")!=-1 )
      {
        curState = SEND_GET_UAID;
      }
      break;
    case RSC_MOB_GETACK:
      if( uri.index("/uaid")!=-1 )
      {
        curState = END_STATE;
        done = 1;
      }
      break;
    default:
      break;
  }
}
