#include "mainsatmock.h"
#include "../mainsat/uaidresolvesm.h"
#include "../mainsat/uaipresolvesm.h"
#include "../mobmsgtype.h"

MainSatMock::MainSatMock()
{

}

MainSatMock::~MainSatMock()
{

}
BOOL MainSatMock::onInit(TiXmlElement *extend)
{
  TiXmlHandle handle(extend);
  TiXmlElement *task = NULL;
  task = handle.FirstChild("task").Element();
  if( task )
  {
    if( !task->Attribute("rscpsaaddr",&rscpsaaddr))
      UniERROR("rscpsaaddr not set");
    if( !task->Attribute("nmeaddr",&nmeaddr))
      UniERROR("nmeaddr not set");
    if( !task->Attribute("rmeaddr",&rmeaddr))
      UniERROR("rmeaddr not set");
    const char *tmp=task->Attribute("localip");
    if( tmp==NULL || strlen(tmp)==0 )
      UniERROR("localip not set");
    localIP << tmp;
  }
  return TRUE;
}

//1.build one sm ?
//receiving get /uaid msg,recv getack /uaid,recv get /uaip,recv getack /uaip
//mainsat that receives two get msg may not be the same sat,so solution is not
//feasible

//2.build a concurrent sm?
//do we need to consider concurrent access so as to use lock in xframe??

void MainSatMock::procMsg(std::auto_ptr<TUniNetMsg> msg)
{
  TUniNetMsg *unimsg = msg.get();
  if( unimsg!=NULL )
  {
    TRscMsgHdr *rschdr = dynamic_cast<TRscMsgHdr *>(msg->getCtrlMsgHdr());
    TRscMsgBody *rscbody = dynamic_cast<TRscMsgBody *>(msg->getMsgBody());
    CStr rid = rschdr->rid;
    if( smManager.find(rid)!=smManager.end())
    {
      MobStateMachine *sm = smManager[rid];
      sm->onRecv(unimsg);
      if( sm->hasDone() )
      {
        delete sm;
        smManager.erase(rid);
      }
    }
    else
    {
      //if received on the first time,create new statemachine
      MobStateMachine *sm = NULL;
      switch( rschdr->code )
      {
        case RSC_MOB_GET:
          CStr uri = rschdr->ruri;
          if( uri.index("/uaid")!=-1 )
          {
            sm = new UaidResolveSm(this);
            smManager[rid] = sm;
          }
          else if( uri.index("/uaip")!=-1 )
          {
            sm = new UaipResolveSm(this);
            smManager[rid] = sm;
          }
          else
          {
            UniERROR("invalide msg type: %d",rschdr->code);
          }
          break;
      }
      if( sm!=NULL )
      {
        sm->onRecv(unimsg);
        if( sm->hasDone() )
        {
          delete sm;
          smManager.erase(rid);
        }
      }
    }
  }
}

void MainSatMock::onTimeOut(TTimerKey timerKey,void *para)
{

}

void MainSatMock::sendToTask(TUniNetMsg *msg)
{
  // UniINFO("sending a %x msg,uri: %s,rsc: %s,tAddr.logAddr: %d",rschdr->code,rschdr->ruri.c_str(),
  //   rscbody->rsc.c_str(),msg->tAddr.logAddr);
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
    //to prevent it from

    // if( sm->hasDone() )
    // {
    //   UniINFO("sm hasDone");
    //   delete sm;
    //   smManager.erase(rid);
    // }
    UniINFO("before sending the msg");
    sendMsg(msg);
  }
}
