#include "mobmsgtype.h"

TUniNetMsg *MobMsgHelper::buildRscMsg()
{
  TUniNetMsg *unimsg = new TUniNetMsg;
  if( unimsg==NULL )
  {
    cout << "null pointer,build rsc msg" << endl;
    return NULL;
  }
  TRscMsgHdr *rschdr = new TRscMsgHdr;
  TRscMsgBody *rscbody = new TRscMsgBody;
  if( rschdr==NULL || rscbody==NULL )
    cout << "null pointer,build rscmsg" << endl;
  rschdr->consumer="consumer";
  rschdr->producer="producer";

  unimsg->msgName = RSC_MESSAGE;
  unimsg->dialogType = DIALOG_MESSAGE;
  unimsg->msgType = RSC_MESSAGE_TYPE;
  unimsg->setTransId();
  //need to use this while build a real rsc msg
  //unimsg->setCSeq(realrid);
  unimsg->setCtrlMsgHdr(rschdr);
  unimsg->setMsgBody(rscbody);
  return unimsg;
}
