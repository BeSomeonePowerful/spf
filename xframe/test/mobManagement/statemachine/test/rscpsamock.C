#include "rscpsamock.h"
#include "../mobmsgtype.h"
#include "../rapidjson/document.h"

RscPsaMock::RscPsaMock()
{

}

RscPsaMock::~RscPsaMock()
{

}

BOOL RscPsaMock::onInit(TiXmlElement *extend)
{
  //UniERROR("RscPsaMock::onInit being called");
  TiXmlHandle handle(extend);
  TiXmlElement *iplist = NULL;
  iplist = handle.FirstChild("iplist").Element();
  if( iplist!=NULL )
  {
    TiXmlHandle iplisthandle(iplist);
    TiXmlElement *ipitem = NULL;
    ipitem= iplisthandle.FirstChild("ipitem").Element();
    while( ipitem )
    {
      const char *tmp =  ipitem->Attribute("ipport");
      if( tmp==NULL || strlen(tmp)==0 )
        UniERROR("ipport not set");
      CStr ipport = tmp;
      TiXmlHandle ipitemhandle(ipitem);
      TiXmlElement *urlmap = NULL;
      urlmap = ipitemhandle.FirstChild("urlmap").Element();
      while( urlmap )
      {
          const char *t = urlmap->Attribute("uri");
          if( t==NULL || strlen(t)==0 )
            UniERROR("url not set int urlmap");
          CStr uri=t;

          int taskid;
          if( !urlmap->Attribute("taskid",&taskid) )
            UniERROR("taskid not set");

          CStr ipuri;
          ipuri << ipport <<  ":" << uri;
          taskManager[ipuri]=taskid;
          urlmap = urlmap->NextSiblingElement();
      }
      ipitem = ipitem->NextSiblingElement();
    }
  }
  return TRUE;
}

void RscPsaMock::procMsg(std::auto_ptr<TUniNetMsg> msg)
{
  for(auto iter = taskManager.begin() ; iter!=taskManager.end() ; ++iter )
  {
    UniINFO("ipuri: %s,taskid: %d",iter->first.c_str(),iter->second);
  }
  UniINFO("taskManager size: %d",taskManager.size());
  TUniNetMsg *unimsg = msg.get();
  //distribute msg to tasks according to the ip and uri of msg
  if( unimsg!=NULL )
  {
    TRscMsgHdr *rschdr = dynamic_cast<TRscMsgHdr *>(unimsg->getCtrlMsgHdr());
    TRscMsgBody *rscbody = dynamic_cast<TRscMsgBody *>(unimsg->getMsgBody());
    UniINFO("receiving a %x msg,uri: %s,rsc: %s,oAddr.logAddr: %d",rschdr->code,rschdr->ruri.c_str(),
      rscbody->rsc.c_str(),unimsg->oAddr.logAddr);
    //the msg is destroyed after exiting this method
    //we need to clone this msg

    //from the ip,we get a list of tasks belong to this ip

    //from the uri,we get the tasks that needed to handle this msg
    //tasks that need to handle this msg has to be located by ip and uri
    CStr ipport = rschdr->host.c_str();
    CStr uri = rschdr->ruri;
    CStr ipuri;
    ipuri << ipport << ":" << uri;
    if( taskManager.find(ipuri)!=taskManager.end())
    {
      int taskid = taskManager[ipuri];
      TUniNetMsg *clonemsg = new TUniNetMsg;
      clonemsg->setCtrlMsgHdr(unimsg->getCtrlMsgHdr()->clone());
      clonemsg->setMsgBody(unimsg->getMsgBody()->clone());
      clonemsg->tAddr.logAddr=taskid;
      sendToTask(clonemsg);
    }
    else
    {
      UniERROR("target not found: ipport: %s,uri: %s",ipport.c_str(),uri.c_str());
    }
  }
}


void RscPsaMock::sendToTask(TUniNetMsg *msg)
{
  TRscMsgHdr *rschdr = dynamic_cast<TRscMsgHdr *>(msg->getCtrlMsgHdr());
  TRscMsgBody *rscbody = dynamic_cast<TRscMsgBody *>(msg->getMsgBody());
  UniINFO("sending a %x msg,uri: %s,rsc: %s,tAddr.logAddr: %d",rschdr->code,rschdr->ruri.c_str(),
    rscbody->rsc.c_str(),msg->tAddr.logAddr);
  sendMsg(msg);
}

void RscPsaMock::onTimeOut(TTimerKey timerKey,void *para)
{

}
