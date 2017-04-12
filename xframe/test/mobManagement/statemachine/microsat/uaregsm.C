#include "uaregsm.h"
#include "../mobmsgtype.h"
#include "../rapidjson/document.h"
#include "../test/microsatmock.h"

using namespace rapidjson;

UaRegSm::UaRegSm(MicroSatMock *_task)
{
    task = _task;
    curState = INIT_STATE;
    done = 0;
}

UaRegSm::~UaRegSm()
{

}

void UaRegSm::onRecv(TUniNetMsg *msg)
{
  TRscMsgHdr *rschdr = dynamic_cast<TRscMsgHdr *>(msg->getCtrlMsgHdr());
  TRscMsgBody *rscbody = dynamic_cast<TRscMsgBody *>(msg->getMsgBody());
  if( rschdr==NULL || rscbody==NULL )
  {
    UniERROR("AddrResolveSm::onRecv: null pointer");
  }
  CStr uri = rschdr->ruri;
  UniINFO("receiving a %x msg,uri: %s,rsc: %s,oAddr.logAddr: %d",rschdr->code,rschdr->ruri.c_str(),
    rscbody->rsc.c_str(),msg->oAddr.logAddr);
  switch( rschdr->code )
  {
    case RSC_MOB_CON:
      {
      
      }
      break;
    case RSC_MOB_CONACK:
      {

      }
      break;
    default:
      UniERROR("invalid msgtype: %d",rschdr->code);
      break;
  }
}

void UaRegSm::onSend(TUniNetMsg *msg)
{
  TRscMsgHdr *rschdr = dynamic_cast<TRscMsgHdr *>(msg->getCtrlMsgHdr());
  TRscMsgBody *rscbody = dynamic_cast<TRscMsgBody *>(msg->getMsgBody());
  if( rschdr==NULL || rscbody==NULL )
  {
    UniERROR("UaRegSm::onSend: null pointer");
  }
  UniINFO("sending a %x msg,uri: %s,rsc: %s,tAddr.logAddr: %d",rschdr->code,rschdr->ruri.c_str(),
    rscbody->rsc.c_str(),msg->tAddr.logAddr);
  switch( rschdr->code )
  {
    case RSC_MOB_CON:
      break;
    case RSC_MOB_CONACK:
      break;
    default:
      break;
  }
}
