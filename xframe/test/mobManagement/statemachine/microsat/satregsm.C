#include "SatRegSm.h"

SatRegSm::SatRegSm()
{
  curState = INIT_STATE;
  done = 0;
}


void SatRegSm::onRecv(TUniNetMsg *msg)
{
  TRscMsgHdr *rschdr = dynamic_cast<TRscMsgHdr *>(msg->getCtrlMsgHdr());
  TRscMsgBody *rscbody = dynamic_cast<TRscMsgBody *>(msg->getMsgBody());
  if( rschdr==NULL || rscbody==NULL )
  {
    UniERROR("SatRegSm::onRecv: null pointer");
  }
  switch( rschdr->code )
  {
    case RSC_MOB_CONACK:
      curState = END_STATE;
      done = 1;
      //what other things should we do?
      //we need to check uri and status
      break;
    default:
      UniERROR("invalid message");
      break;
  }
}

//onSend method should be called by task before sending message out to other task
void SatRegSm::onSend(TUniNetMsg *msg)
{
  TRscMsgHdr *rschdr = dynamic_cast<TRscMsgHdr *>(msg->getCtrlMsgHdr());
  TRscMsgBody *rscbody = dynamic_cast<TRscMsgBody *>(msg->getMsgBody());
  if( rschdr==NULL || rscbody==NULL )
  {
    UniERROR("SatRegSm::onSend: null pointer");
  }
  switch( rschdr->code )
  {
    case RSC_MOB_CON:
      curState = SEND_CON;
      break;
    default:
      UniERROR("invalid message");
      break;
  }
}
