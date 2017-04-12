#include "nmemock.h"
#include "../mobmsgtype.h"
#include "../rapidjson/document.h"
#include <iostream>

using std::cout;
using std::endl;

using namespace rapidjson;

NmeMock::NmeMock()
{

}

NmeMock::~NmeMock()
{

}

BOOL NmeMock::onInit(TiXmlElement *extend)
{
  TiXmlHandle handle(extend);
  TiXmlElement *task = NULL;
  task = handle.FirstChild("task").Element();
  if( task )
  {
    if( !task->Attribute("rscpsaaddr",&rscpsaaddr) )
      UniERROR("rscpsaaddr not set");
    if( !task->Attribute("cmeaddr",&cmeaddr) )
      UniERROR("cmeaddr not set");
    const char *tmp = task->Attribute("localip");
    if( tmp==NULL || strlen(tmp)==0 )
      UniERROR("localip not set");
    localIP << tmp;
  }
  TiXmlElement *usermaplist = NULL;
  usermaplist = handle.FirstChild("usermaplist").Element();
  if( usermaplist )
  {
    TiXmlHandle usermaplisthandle(usermaplist);
    TiXmlElement *usermapitem = usermaplisthandle.FirstChild("usermapitem").Element();
    while( usermapitem )
    {
      int userid;
      if( !usermapitem->Attribute("userid",&userid) )
        UniERROR("uaid not set in usermapitem");
      const char *tmp = usermapitem->Attribute("ipport");
      if( tmp==NULL || strlen(tmp)==0 )
        UniERROR("ipport not set in usermapitem");
      CStr ipport;
      ipport << tmp;
      userid_ip_mapping[userid]=ipport;
      usermapitem = usermapitem->NextSiblingElement();
    }
  }
  TiXmlElement *uamaplist = NULL;
  uamaplist = handle.FirstChild("uamaplist").Element();
  if( uamaplist )
  {
    TiXmlHandle uamaplisthandle(uamaplist);
    TiXmlElement *uamapitem = uamaplisthandle.FirstChild("uamapitem").Element();
    while( uamapitem )
    {
      int uaid;
      if( !uamapitem->Attribute("uaid",&uaid) )
        UniERROR("uaid not set in uamapitem");
      const char *tmp = uamapitem->Attribute("ipport");
      if( tmp==NULL || strlen(tmp)==0 )
        UniERROR("ipport not set in uamapitem");
      CStr ipport;
      ipport << tmp;
      uaid_ip_mapping[uaid]=ipport;
      uamapitem = uamapitem->NextSiblingElement();
    }
  }
  TiXmlElement *userreglist = NULL;
  userreglist = handle.FirstChild("userreglist").Element();
  if( userreglist )
  {
    TiXmlHandle userreglisthandle(userreglist);
    TiXmlElement *userregitem=NULL;
    userregitem = userreglisthandle.FirstChild("userregitem").Element();
    while( userregitem )
    {
      int userid;
      int uaid;
      if( !userregitem->Attribute("userid",&userid))
        UniERROR("userid not set in userregitem");
      if( !userregitem->Attribute("uaid",&uaid) )
        UniERROR("uaid not set in userregitem");
      userid_uaid_mapping[userid]=uaid;
      userregitem = userregitem->NextSiblingElement();
    }
  }
  TiXmlElement *uareglist = NULL;
  uareglist = handle.FirstChild("uareglist").Element();
  if( uareglist )
  {
    TiXmlHandle uareglisthandle(uareglist);
    TiXmlElement *uaregitem = NULL;
    uaregitem = uareglisthandle.FirstChild("uaregitem").Element();
    while( uaregitem )
    {
      int uaid;
      CStr uaip;
      if( !uaregitem->Attribute("uaid",&uaid))
        UniERROR("uaid not set in uaregitem");
      const char *tmp = uaregitem->Attribute("uaip");
      if( tmp==NULL || strlen(tmp)==0 )
        UniERROR("uaip not set in uaregitem");
      uaip << tmp;
      uaid_uaip_mapping[uaid]=uaip;
      uaregitem = uaregitem->NextSiblingElement();
    }
  }
  return TRUE;
}

void NmeMock::procMsg(std::auto_ptr<TUniNetMsg> msg)
{
  TUniNetMsg *unimsg = msg.get();
  if( unimsg!=NULL )
  {
    TRscMsgHdr *rschdr = dynamic_cast<TRscMsgHdr *>(msg->getCtrlMsgHdr());
    TRscMsgBody *rscbody = dynamic_cast<TRscMsgBody *>(msg->getMsgBody());

    UniINFO("receiving a %x msg,uri: %s,rsc: %s,oAddr.logAddr: %d",rschdr->code,rschdr->ruri.c_str(),
      rscbody->rsc.c_str(),unimsg->oAddr.logAddr);
    TUniNetMsg *unimsg = MobMsgHelper::buildRscMsg();
    TRscMsgHdr *newrschdr = dynamic_cast<TRscMsgHdr *>(unimsg->getCtrlMsgHdr());
    TRscMsgBody *newrscbody = dynamic_cast<TRscMsgBody *>(unimsg->getMsgBody());

    newrschdr->code = RSC_MOB_GETACK;
    newrschdr->host = TURI(rschdr->producer);
    newrschdr->rid = rschdr->rid;
    newrschdr->consumer = rschdr->producer;
    newrschdr->producer = this->localIP;
    //three kinds of get
    //two kinds of nty
    CStr uri = rschdr->ruri;
    UniINFO("before switch: code:%x,uri: %s,rsc: %s",rschdr->code,rschdr->ruri.c_str(),
      rscbody->rsc.c_str());
    switch( rschdr->code )
    {
      case RSC_MOB_GET:
        UniINFO("Entering Get case");
        if( uri.index("/satip")!=-1 )
        {
          UniINFO("Entering /satip case");
          unimsg->tAddr.logAddr = this->rscpsaaddr;
          //get userid or uaid out of content
          Document d;
          d.Parse(rscbody->rsc.c_str());
          //parsing
          Value &uid = d["uid"];
          int id = uid.GetInt();

          newrschdr->ruri = "/getack/satip";
          if( userid_ip_mapping.find(id)!=userid_ip_mapping.end() )
          {
            char buf[100];
            sprintf(buf,"{\"uid\":%d,\"status\":1,\"satip\":\"%s\"}",id,userid_ip_mapping[id].c_str());
            newrscbody->rsc << buf;
          }
          else
          {
            char buf[100];
            sprintf(buf,"{\"uid\":%d,\"status\":0,\"satip\":\"\"}",id);
            newrscbody->rsc << buf;
          }
        }
        else if( uri.index("/uaid")!=-1 )
        {
          //UniINFO("Entering /uaid case");
          unimsg->tAddr.logAddr = this->cmeaddr;
          newrschdr->ruri = "/getack/uaid";

          //UniINFO("before json parsing");
          Document d;
          d.Parse(rscbody->rsc.c_str());

          Value &userid = d["userid"];
          int id = userid.GetInt();
          //
          // int id;
          // if( d.HasMember("userid") )
          // {
          //   cout << "document has member userid" << endl;
          //   Value &userid = d["userid"];
          //   id = userid.GetInt();
          // }
          // else
          // {
          //   cout << " document has no member userid" << endl;
          // }
          // UniINFO("before getting intvalue out\n");
          //UniINFO("before sprintf");

          char buf[100];
          if( userid_uaid_mapping.find(id)!=userid_uaid_mapping.end())
          {
            sprintf(buf,"{\"userid\":%d,\"status\":1,\"uaid\":%d}",id,userid_uaid_mapping[id]);
          }
          else
          {
            sprintf(buf,"{\"userid\":%d,\"status\":0,\"uaid\":-1}",id);
          }
          newrscbody->rsc << buf;
          //UniINFO("exiting /uaid case");
          //cout << "end of processing /get/uaid" << endl;
          //get userid out of content
        }
        else if( uri.index("/uaip")!=-1 )
        {
          unimsg->tAddr.logAddr = this->cmeaddr;
          newrschdr->ruri = "/getack/uaip";
          Document d;
          d.Parse(rscbody->rsc.c_str());

          Value &uaid = d["uaid"];
          int id = uaid.GetInt();

          char buf[100];
          if( uaid_uaip_mapping.find(id)!=uaid_uaip_mapping.end())
          {
            sprintf(buf,"{\"uaid\":%d,\"status\":1,\"uaip\":\"%s\"}",id,uaid_uaip_mapping[id].c_str());
          }
          else
          {
            sprintf(buf,"{\"uaid\":%d,\"status\":0,\"uaip\":\"\"}",id);
          }
          newrscbody->rsc << buf;
          //get uaid out of content
        }
        break;
      default:
        break;
    }
    sendToTask(unimsg);
  }
}

void NmeMock::sendToTask(TUniNetMsg *msg)
{
  //UniINFO("NmeMock::sendToTask being called");
  TRscMsgHdr *rschdr = dynamic_cast<TRscMsgHdr *>(msg->getCtrlMsgHdr());
  TRscMsgBody *rscbody = dynamic_cast<TRscMsgBody *>(msg->getMsgBody());
  UniINFO("sending a %x msg,uri: %s,rsc: %s,tAddr.logAddr: %d",rschdr->code,rschdr->ruri.c_str(),
    rscbody->rsc.c_str(),msg->tAddr.logAddr);
  sendMsg(msg);
}

void NmeMock::onTimeOut(TTimerKey timerKey,void *para)
{

}
