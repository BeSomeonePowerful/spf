#include "CMEU.h"

TUniNetMsg *CMEU::procMsg(TUniNetMsg *msg)
{
  //receiving con of mqtt or reg
  //build a map of userid to mqtt link
  //convert it to a msg message
  //

  //message from mqttpsa
  if( msg->oAddr.logAddr == psamqttaddr )
  {

    switch()
  }
  //message from rscpsa
  else if( msg->oAddr.logAddr == psarscaddr )
  {
    TRscMsgHdr *rschdr = dynamic_cast<TRscMsgHdr *>(msg->getCtrlMsgHdr());
    TRscMsgBody *rscbody = dynamic_cast<TRscMsgBody *>(msg->getMsgBody());
    //receiving conack
  }
  else
  {
    //report error
  }
  //receiving regack of rsc

  //receiving conack
  //receiving getack /satip
  //receiving getack /uaid
  //receiving getack /uaip
  //receiving suback /uaid
  //receiving suback /uaip
  //receiving nty /uaid
  //receiving nty /uaip
  //sending get
  //sending sub
}
