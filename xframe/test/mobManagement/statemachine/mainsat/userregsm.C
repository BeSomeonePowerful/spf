#include "userregsm.h"
#include "mobmsgtype.h"

UserRegSm::UserRegSm()
{
  curState = INIT_STATE;
  done = 0;
  msg = NULL;
}


//in stateMachin, check if sm in right state if not report an error
void UserRegSm::onRecv(TUniNetMsg *msg)
{
  TRscMsgHdr *rschdr = dynamic_cast<TRscMsgHdr *>(msg->getCtrlMsgHdr());
  TRscMsgBody *rscbody = dynamic_cast<TRscMsgBody *>(msg->getMsgBody());

  //report error and return
  if( rschdr==NULL || rscbody==NULL )
  {
    UniERROR("UserRegSm::onRecv: null pointer");
  }
  switch( rschdr->code )
  {
    //if in curState received a wrong msg,just report an error
    case RSC_MOB_REG:
      if( curState!=INIT_STATE )
      {
        UniERROR("Received a reg msg while not in INIT_STATE");
      }
      curState = RECV_REG;
      TUniNetMsg *clonemsg = new TUniNetMsg;
      clonemsg->setCtrlMsgHdr(msg->getCtrlMsgHdr()->clone());
      clonemsg->setMsgBody(msg->getMsgBody()->clone());
      this->msg = clonemsg;
      //build a get msg to be sent to nme
      //does xframe has any json-resolve library??

      //another way is to use test para,depending on the value of test,
      //different code is executed
      TUniNetMsg *unimsg = buildMsg(RSC_MOB_GET,);
      onSend(unimsg);
      task->sendMsg(unimsg);
      break;
    case RSC_MOB_GETACK:
      //get the IP of home sat
      //if ip is this machine
      //send reg to hss of nme
      //else send reg to cme-h of other machine
      curState = RECV_GETACK;
      TUniNetMsg *unimsg = buildMsg(RSC_MOB_REG,);
      onSend(unimsg);
      task->sendMsg(unimsg);
      break;
    case RSC_MOB_REGACK:
      //check if in right state
      //build a new regack,change the host to previous host and send it
      curState = RECV_REGACK;
      TUniNetMsg *unimsg = buildMsg(RSC_MOB_REGACK,);
      onSend(unimsg);
      task->sendMsg(unimsg);
      break;
    default:
      //report error
      UniERROR("UserRegSm received an invalid msg: type: %d",rschdr->code);
      break;
  }
}

//when will this method be called
void UserRegSm::onSend(TUniNetMsg *msg)
{
  TRscMsgHdr *rschdr = dynamic_cast<TRscMsgHdr *>(msg->getCtrlMsgHdr());
  TRscMsgBody *rscbody = dynamic_cast<TRscMsgBody *>(msg->getMsgBody());
  if( rschdr==NULL || rscbody==NULL )
  {
    UniERROR("UserRegSm::onRecv: null pointer");
  }
  switch( rschdr->code )
  {
    case RSC_MOB_GET:
      curState = SEND_GET;
      break;
    case RSC_MOB_REG:
      curState = FORWARD_REG;
      break;
    case RSC_MOB_REGACK:
      curState = END_STATE;
      done = 1;
      //should the memory deallocation recursively be done?I think we should.
      if( msg!=NULL ) delete msg;
    default:
      break;
  }
}
