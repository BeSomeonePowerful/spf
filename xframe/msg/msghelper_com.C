#include "msghelper_com.h"
#include "cmd5.h"


//Convert Message Type
int COMMsgHelper::log_Level(CStr& s)
{
	if(s=="LOG_DEBUG")
	return  0;
	else if(s=="LOG_INFO")
	return 1;
	else if(s=="LOG_NOTICE")
	return 2;
	else if(s=="LOG_WARNING")
	return 3;
	else if(s=="LOG_ERR")
	return 4;
	else if(s=="LOG_CRIT")
	return 5;
	else if(s=="LOG_ALERT")
	return 6;
	else if(s=="LOG_EMERG")
	return 7;
	else return 1;
}

void COMMsgHelper::addMsgBody(PTUniNetMsg uniMsg, CStr& msg)
{
  TComMsgBody* msgbody=NULL;
  if(uniMsg==NULL) return;
  if(uniMsg->hasMsgBody())
  {
        delete uniMsg->msgBody;
	uniMsg->msgBody=NULL;
  }
 
  msgbody=new TComMsgBody();
  uniMsg->msgBody = msgbody;
  uniMsg->setMsgBody();
  msgbody->info=msg;
}

void COMMsgHelper::addCtrlMsgHdr(PTUniNetMsg uniMsg, INT st, INT pr, INT fa)
{
  TComMsgHdr* comctrl=NULL;
  if(uniMsg==NULL) return;
  if(uniMsg->hasCtrlMsgHdr())
  {
	delete uniMsg->ctrlMsgHdr;
	uniMsg->ctrlMsgHdr=NULL;
  }
  
  comctrl = new TComMsgHdr();
  uniMsg->ctrlMsgHdr=comctrl;
  uniMsg->setCtrlMsgHdr();
  comctrl->status = st; 
  comctrl->pri 	  = pr; 
  comctrl->facility = fa; 
}

TComMsgHdr* COMMsgHelper::getComMsgCtrlHdr(PTUniNetMsg unimsg)
{
	if(!unimsg) return NULL;
        if(!unimsg->hasCtrlMsgHdr()) return NULL;
	
  	TComMsgHdr* comctrl=NULL;
        comctrl=dynamic_cast<TComMsgHdr*>(unimsg->ctrlMsgHdr);
         if(!comctrl) return NULL;	
	return comctrl;
}



TComMsgBody*  COMMsgHelper::getComMsgBody(PTUniNetMsg unimsg)
{
	if(!unimsg) return NULL;
	if(!unimsg->hasMsgBody()) return NULL;  

	TComMsgBody* combody=NULL;
	combody=dynamic_cast<TComMsgBody*>(unimsg->msgBody);
	if(!combody) return NULL;
	
	return combody;
}

TUniNetMsg*  COMMsgHelper::genComMsgResp(PTUniNetMsg unimsg, INT code)
{
	TUniNetMsg* resp=NULL;
	resp=new TUniNetMsg();
	resp->oAddr=unimsg->tAddr;
	resp->tAddr=unimsg->oAddr;
	resp->dialogType=unimsg->dialogType;
	if(code==100) resp->msgName=COM_MESSAGE_TMP;
	else resp->msgName=COM_MESSAGE_RSP;
	resp->msgType=unimsg->msgType;
	if(unimsg->hasCSeq()) resp->setCSeq(unimsg->getCSeq());
	if(unimsg->hasTransId()) resp->setTransId(unimsg->getTransId());
	if(unimsg->hasCtrlMsgHdr())
	{
		TComMsgHdr* ctrl=new TComMsgHdr();
		*ctrl=*((TComMsgHdr*)(unimsg->getCtrlMsgHdr()));
		ctrl->status=code;
		resp->setCtrlMsgHdr(ctrl);
	}
	return resp;
}

INT COMMsgHelper::getComMsgPri(PTUniNetMsg unimsg)
{
	TComMsgHdr* comctrl=NULL;
    comctrl=dynamic_cast<TComMsgHdr*>(unimsg->ctrlMsgHdr);
    if(!comctrl) return 0;
	else return comctrl->pri;
}

void COMMsgHelper::brief(PTUniNetMsg unimsg, CStr& brief)
{
	if(unimsg==NULL) return;
	unimsg->brief(brief);
	if(unimsg->msgName==COM_MESSAGE_REQ) 	brief<<"\r\nCOM:{ req:{ ";
	else if(unimsg->msgName==COM_MESSAGE_RSP)  brief<<"\r\nCOM:{ rsp:{ ";
	else brief<<"\r\nCOM:{ tmp:{ ";
	if(unimsg->hasCtrlMsgHdr())
	{
		TComMsgHdr* hdr=(TComMsgHdr*)(unimsg->getCtrlMsgHdr());
		brief<<"status:"<<hdr->status<<"; pri:"<<hdr->pri<<"; facility:"<<hdr->facility;
	}
	if(unimsg->hasMsgBody())
	{
		TComMsgBody* body=(TComMsgBody*)(unimsg->getMsgBody());
		brief<<"; info:"<<body->info;
	}
	brief<<"}}\r\n";
}
