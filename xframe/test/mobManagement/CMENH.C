
void CMENH::procMsg(TUniNetMsg *msg)
{
  //check msg!=NULL
  //receiving from psa on the same machine
  TRscMsgHdr *rschdr = dynamic_cast<TRscMsgHdr *>(msg->getCtrlMsgHdr());
  TRscMsgBody *rscbody = dynamic_cast<TRscMsgBody *>(msg->getMsgBody());
  if( unimsg->oAddr.logAddr == psaAddr )
  {
    int rid = atoi(rschdr->rid.c_str());
    MobStateMachine *sm = smManager[rid];

    //statemachine need to be constructed in here
    if( sm==NULL ) return;
    sm->onRecv(msg);
    if( sm.hasDone() ) smManager.erase(rid);
    //according msgType decide what should be done
    //how to maintain callid,one communication per callid maintainer
    //receiving reg
    //get uaid out of reg msg
    //send get msg to nme to get the homeAddr of uaid
    switch( rschdr->code )
    {
      //receiving reg
      case RSC_MOB_REG:
        break;
      //receiving regack
      case RSC_MOB_REGACK:
        break;
      //receiving get /UAID
      case RSC_MOB_GET:
        break;
      //receiving getack /UAID
      //receiving get /UAIP
      //receiving getack /UAIP
      //receiving sub /uaid
      //receiving suback /uaid
      //receiving sub /uaip
      //receiving suback /uaip
      //receiving nfy /uaid
      //receiving nfyack /uaid
      //receiving nfy /uaip
      //receiving nfyack /uaip
    }
  }
  //receiving from nme on the same machine
  //what protocol is used for two tasks on the same machine,rsc
  else if( unimsg->oAddr.logAddr == nmeAddr )
  {
    //receiving getack
    //receiving regack
  }
  //receiving from rme on the same machine
  else if( unimsg->oAddr.logAddr == rmeAddr )
  {

  }
  else
  {
    //log and report error
  }
}


//in a simple case,how do we implement the process of registering
