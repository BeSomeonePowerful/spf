#include "addrresolvesm.h"
#include "../mobmsgtype.h"
#include "../rapidjson/document.h"
#include "../test/microsatmock.h"


using namespace rapidjson;


AddrResolveSm::AddrResolveSm(MicroSatMock *_task,int _userid)
{
  task = _task;
  userid = _userid;
  curState = INIT_STATE;
  done = 0;
  msg = NULL;
}

AddrResolveSm::~AddrResolveSm()
{

}

void AddrResolveSm::onRecv(TUniNetMsg *msg)
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
    case RSC_MOB_GETACK:
      if( curState==SEND_GET_SATIP0 )
      {
        if( uri.index("/satip")!=-1 )
        {
          curState = RECV_GETACK_SATIP0;
          //build get uaid msg out of previous information and send to nme
          TUniNetMsg *unimsg = MobMsgHelper::buildRscMsg();
          TRscMsgHdr *newrschdr = dynamic_cast<TRscMsgHdr *>(unimsg->getCtrlMsgHdr());
          TRscMsgBody *newrscbody = dynamic_cast<TRscMsgBody *>(unimsg->getMsgBody());
          unimsg->tAddr.logAddr=task->rscpsaaddr;
          newrschdr->code =RSC_MOB_GET;
          newrschdr->ruri="/get/uaid";
          newrschdr->rid = rschdr->rid;
          newrschdr->producer = task->localIP;

          Document d;
          d.Parse(rscbody->rsc.c_str());
          Value &status = d["status"];
          if( status.GetInt() )
          {
            Value &ipport = d["satip"];
            CStr host = CStr(ipport.GetString());
            newrschdr->host = TURI(host);
            newrschdr->consumer = ipport.GetString();
          }
          else
          {
            //return from
            UniERROR("failed to getIP");
            delete unimsg;
            return;
          }
          char buf[100];
          sprintf(buf,"{\"userid\":%d}",d["uid"].GetInt());
          newrscbody->rsc = buf;
          task->sendToTask(unimsg);
        }
        else
        {
          UniERROR("invalid msg type: %x",rschdr->code);
        }
      }
      else
      {
        if( uri.index("/satip")!=-1 )
        {
          curState = RECV_GETACK_SATIP1;
          //build get uaip msg and send
          TUniNetMsg *unimsg = MobMsgHelper::buildRscMsg();
          TRscMsgHdr *newrschdr = dynamic_cast<TRscMsgHdr *>(unimsg->getCtrlMsgHdr());
          TRscMsgBody *newrscbody = dynamic_cast<TRscMsgBody *>(unimsg->getMsgBody());
          unimsg->tAddr.logAddr = task->rscpsaaddr;
          newrschdr->code =RSC_MOB_GET;
          newrschdr->ruri="/get/uaip";
          newrschdr->rid = rschdr->rid;
          newrschdr->producer = task->localIP;


          Document d;
          d.Parse(rscbody->rsc.c_str());
          Value &status = d["status"];
          if( status.GetInt() )
          {
            Value &ipport = d["satip"];
            CStr host = CStr(ipport.GetString());
            newrschdr->host = TURI(host);
            newrschdr->consumer = ipport.GetString();
          }
          else
          {
            //return from
            UniERROR("failed to getIP");
            delete unimsg;
            return;
          }
          char buf[100];
          sprintf(buf,"{\"uaid\":%d}",d["uid"].GetInt());
          newrscbody->rsc = buf;
          task->sendToTask(unimsg);
        }
        else if( uri.index("/uaid")!=-1 )
        {
          curState = RECV_GETACK_UAID;
          //build get satip msg and send
          TUniNetMsg *unimsg = MobMsgHelper::buildRscMsg();
          TRscMsgHdr *newrschdr = dynamic_cast<TRscMsgHdr *>(unimsg->getCtrlMsgHdr());
          TRscMsgBody *newrscbody = dynamic_cast<TRscMsgBody *>(unimsg->getMsgBody());
          unimsg->tAddr.logAddr = task->rscpsaaddr;
          newrschdr->code =RSC_MOB_GET;
          newrschdr->ruri="/get/satip";
          newrschdr->rid = rschdr->rid;
          newrschdr->producer = task->localIP;
          newrschdr->consumer = task->mainsatIP;

          Document d;
          d.Parse(rscbody->rsc.c_str());
          Value &status = d["status"];
          if( status.GetInt() )
          {
            newrschdr->host = TURI(task->mainsatIP);
            char buf[100];
            sprintf(buf,"{\"uid\":%d}",d["uaid"].GetInt());
            newrscbody->rsc = buf;
          }
          else
          {
            //return from
            UniERROR("failed to get uaid");
            delete unimsg;
            return;
          }
          task->sendToTask(unimsg);
        }
        else if( uri.index("/uaip")!=-1 )
        {
          //log it
          Document d;
          d.Parse(rscbody->rsc.c_str());
          Value &ipport = d["uaip"];
          UniINFO("#####Get userid: %d####,uaip: %s",this->userid,ipport.GetString());
          curState = END_STATE;
          done = 1;
        }
        else
        {
          UniERROR("invalid msg type: %x",rschdr->code);
        }
      }
      break;
    default:
      break;
  }
}

void AddrResolveSm::onSend(TUniNetMsg *msg)
{
  TRscMsgHdr *rschdr = dynamic_cast<TRscMsgHdr *>(msg->getCtrlMsgHdr());
  TRscMsgBody *rscbody = dynamic_cast<TRscMsgBody *>(msg->getMsgBody());
  if( rschdr==NULL || rscbody==NULL )
  {
    UniERROR("AddrResolveSm::onSend: null pointer");
  }
  UniINFO("sending a %x msg,uri: %s,rsc: %s,tAddr.logAddr: %d",rschdr->code,rschdr->ruri.c_str(),
    rscbody->rsc.c_str(),msg->tAddr.logAddr);
  switch( rschdr->code )
  {
    case RSC_MOB_GET:
      {
        CStr uri = rschdr->ruri;
        if( curState==INIT_STATE )
        {
          if( uri.index("/satip")!=-1 )
          {
            curState = SEND_GET_SATIP0;
          }
          else
          {
            //report error
            UniERROR("invalid msg type: %x",rschdr->code);
          }
        }
        else
        {
          if( uri.index("/satip")!=-1 )
          {
            curState = SEND_GET_SATIP1;
          }
          else if( uri.index("/uaid")!=-1 )
          {
            curState = SEND_GET_UAID;
          }
          else if( uri.index("/uaip")!=-1 )
          {
            curState = SEND_GET_UAIP;
          }
          else
          {
            UniERROR("invalid msg type: %x",rschdr->code);
          }
        }
      }
      break;
    default:
      break;
  }
}
