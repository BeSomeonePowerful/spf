#include "msghelper_http.h"
#include "cmd5.h"

void HTTPMsgHelper::addMsgBody(PTUniNetMsg uniMsg, HttpMessage* msg)
{
	//HttpRequestPacket *request = (HttpRequestPacket*) packet;
	THttpMsg* httpBody = NULL;
	if(uniMsg==NULL || msg==NULL) return ;
	if(uniMsg->hasMsgBody())
	{
		if(httpBody=dynamic_cast<THttpMsg*>(uniMsg->msgBody))
		{	
			if(httpBody->hMsg!=NULL)
			{
				delete httpBody->hMsg;
				httpBody->hMsg=NULL;
			}
		}
		else
		{
			delete uniMsg->msgBody;
		}
	}
    if(httpBody==NULL)
	{
	    httpBody = new THttpMsg();
	    uniMsg->msgBody = httpBody;
	    uniMsg->setMsgBody();
	}
	httpBody->hMsg = msg;
}

void HTTPMsgHelper::addCtrlMsgHdr(PTUniNetMsg uniMsg, HttpMessage* msg)
{
  THttpCtrlMsg* httpctrl=NULL;
  if(uniMsg==NULL || msg==NULL) return;
  if(uniMsg->hasCtrlMsgHdr())
  {
	if(httpctrl=dynamic_cast<THttpCtrlMsg*>(uniMsg->ctrlMsgHdr))
	{
		if(!httpctrl) delete uniMsg->ctrlMsgHdr;
	}
  }
  if(httpctrl==NULL)
  {
        httpctrl = new THttpCtrlMsg();
        uniMsg->ctrlMsgHdr=httpctrl;
        uniMsg->setCtrlMsgHdr();
  }
  httpctrl->status = msg->isRequest() ? 0 : 1; //0-request 1-response
}

tbnet::HttpMessage* HTTPMsgHelper::makeHTTPResponse(tbnet::HttpMessage* hmsg)
{
    HttpMessage* httpmsg=new HttpMessage(0);
    httpmsg->setConnection(hmsg->getConnection());
    //httpmsg->setStatus(true);
    httpmsg->setKeepAlive(hmsg->isKeepAlive());
    if (!hmsg->isKeepAlive()) {
        httpmsg->getConnection()->setWriteFinishClose(true);
    }
    return  httpmsg;
}