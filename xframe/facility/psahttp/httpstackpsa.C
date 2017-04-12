#include "httpstackpsa.h"

#include "json.h"
#include "jsonvalue.h"
#include <string>

#include<string.h>
#include<cstdlib>

using namespace std;

HttpStackPsa::HttpStackPsa() 
{
	mServer = NULL;
	mClient = NULL;
	UniINFO("new a HttpStackPsa");
}

HttpStackPsa::~HttpStackPsa()
{
	if(mServer) delete mServer;
	if(mClient) delete mClient;
}

BOOL HttpStackPsa::onInit(TiXmlElement* extend)
{
	TiXmlHandle handle(extend);
	TiXmlElement* psa = NULL;
	psa = handle.FirstChild("psa").Element();
	if(psa)
	{

		TiXmlHandle psahandle(psa);
		TiXmlElement * binds = NULL;
		binds = psahandle.FirstChild("binds").Element();
		if(binds)
		{
			TiXmlHandle bindhandle(binds);
			TiXmlElement*	bind=NULL;
			bind=bindhandle.FirstChild("bind").Element();
			if(bind)
			{
                CStr addr;
				int port;
				if(!bind->Attribute("port", &port)) port = 8088;
                const char* ip;
                ip = bind->Attribute("ip");
                const char* netType;
                netType = bind->Attribute("netType");
                addr<<netType;
                addr<<":";
                addr<<ip;
                addr<<":";
                addr<<port;
                                
				//mServer = new HttpServer("tcp:124.127.117.203:8196");//实例化协议栈
				mServer = new HttpServer((char *)addr.c_str());//实例化协议栈
                mServer->setServerAdapter(this);
                mServer->start();
                cout<<"listen"<<addr.c_str()<<endl;
			}else
			{

				UniERROR("httppsa etc bind error");
			}
		}else
		{
			UniERROR("httppsa etc binds error");

		}
		TiXmlElement * targets = NULL;
		targets = psahandle.FirstChild("targets").Element();
		if(targets)
		{
			TiXmlHandle targethandle(targets);
			TiXmlElement*	target=NULL;
			target=targethandle.FirstChild("target").Element();
			while(target)
			{
				string targetName;
				int targetID=-1;
				targetName=target->Attribute("targetName");
				target->Attribute("targetID", &targetID);
				if (!targetName.empty()&&targetID!=-1)
				{
					dispatcher[targetName]=targetID;
					UniINFO("target:%s->taskid:%d",targetName.c_str(),targetID);
				}
				target=target->NextSiblingElement();
			}
		}
	}
	else
	{
		UniERROR("httppsa etc error");
	}

	mClient = new HttpClient(this);
    mClient->start();
	return true;

}
IPacketHandler::HPRetCode HttpStackPsa::handlePacket(Connection *connection, Packet *packet)
{
	HttpMessage* httpmsg=dynamic_cast<HttpMessage*>(packet);
	if (httpmsg)
	{
		httpmsg->setConnection(connection);
		TUniNetMsg* unimsg = new TUniNetMsg();
		THttpCtrlMsg* ctrlmsg=NULL;
		if(unimsg)
		{
			unimsg->sender.taskID=0;
			unimsg->sender.taskType=0;
			unimsg->sender.instID=0;
			if(httpmsg->isRequest()) unimsg->msgName=HTTP_MESSAGE_REQ;
			else unimsg->msgName=HTTP_MESSAGE_RSP;
			unimsg->msgType=HTTP_MESSAGE_TYPE;

			HTTPMsgHelper::addCtrlMsgHdr(unimsg,httpmsg);
			HTTPMsgHelper::addMsgBody(unimsg, httpmsg);

			sendMsgSelf((TMsg*)(unimsg));
		}
	}
	else
	{
		TBSYS_LOG(WARN,"Received a message, but is not a http message! ");
	}
	return IPacketHandler::FREE_CHANNEL;
}


IPacketHandler::HPRetCode HttpStackPsa::handlePacket(Packet *packet,void *args)
{
	cout<<"send to java success-----------------------------------------"<<endl;
	//Connection* _conn = packet->getConnection();
	HttpMessage* httpmsg=dynamic_cast<HttpMessage*>(packet);
	//cout << httpmsg->getStatusMessage() << endl;
	if(httpmsg)
	  {
	    Connection* _conn = httpmsg->getConnection();
		cout << "response body: " << httpmsg->getStatusMessage() << endl;
	    TUniNetMsg* unimsg = new TUniNetMsg();
	    THttpCtrlMsg* ctrlmsg=NULL;
	    if(unimsg)
		{
			unimsg->sender.taskID=0;
			unimsg->sender.taskType=0;
			unimsg->sender.instID=0;
			if(httpmsg->isResponse()) unimsg->msgName=HTTP_MESSAGE_RSP;
			else unimsg->msgName=HTTP_MESSAGE_REQ;
			unimsg->msgType=HTTP_MESSAGE_TYPE;

			HTTPMsgHelper::addCtrlMsgHdr(unimsg,httpmsg);
			HTTPMsgHelper::addMsgBody(unimsg, httpmsg);

			sendMsgSelf((TMsg*)(unimsg));
		}
	  }
	  
	  cout << "响应包封包错误！" << endl;
	return IPacketHandler::KEEP_CHANNEL;
}

void HttpStackPsa::procMsg(std::auto_ptr<TUniNetMsg> msg)
{

	TUniNetMsg* unimsg=msg.get();
	if(!unimsg) return;
	if(!unimsg->hasCtrlMsgHdr()) return;
	if(!unimsg->hasMsgBody()) return;

	if (unimsg->msgType==HTTP_MESSAGE_TYPE)
	{  
        THttpMsg* httpbody=NULL;
        httpbody=dynamic_cast<THttpMsg*>(unimsg->msgBody);
        if(!httpbody) return;
        THttpCtrlMsg* httpctrl=NULL;
        httpctrl=dynamic_cast<THttpCtrlMsg*>(unimsg->ctrlMsgHdr);
        if(!httpctrl) return;

        HttpMessage *hmsg;
        hmsg = httpbody->hMsg;
        if(hmsg==NULL) return;

        if(unimsg->sender.taskID==0)
        {
            UniDEBUG("  Received a msg from wire....");
            
            cout<<hmsg->getQuery()<<endl;
			UniDEBUG("request line:%s",hmsg->getQuery());
			if(!hmsg->getQuery())
			  {
			    cout << "response------------" << endl;
			    return;
			  }
			string reqline(hmsg->getQuery());
			string dis=getRequestLinePrefix(reqline);

			//匹配规则：前缀匹配
			map<string,int>::iterator it;
			for(it=dispatcher.begin();it!=dispatcher.end();++it)
			{
				string prefix=it->first;
				if(dis.find(prefix)!=std::string::npos)
				{
					UniDEBUG("to task::%d",it->second);
					unimsg->tAddr.logAddr=it->second;
					sendMsg(msg);
					break;
				}
			}
			
			return;
        }
        else//从其他task来的
        {
            UniDEBUG("HttpStackPsa receive message from other task:%d",unimsg->sender.taskID);
            if(unimsg->msgName==HTTP_MESSAGE_RSP)
            {
            	Connection *connection = hmsg->getConnection();
	            if (hmsg->getbody()!=NULL)
	            {
	                UniDEBUG("HttpStackPsa body: %s,length:%d",hmsg->getbody(),strlen(hmsg->getbody()));
	            }
		        if (connection)
		        {
					connection->postPacket(hmsg);
		        }
		        else
		        {
		        	UniERROR("connection NULL");
		        }
            }
            else if(unimsg->msgName==HTTP_MESSAGE_REQ)
            {
            	sendHttpReq(hmsg);
				cout << "send to java!------------------" << endl;
            }
            else
            {
            	UniERROR("HTTP_MESSAGE_TYPE ERROR");
            }
        }
	}
	else
	{
		UniERROR("Received a msg,but is not a http message!");
	}
    return;
}

void HttpStackPsa::onTimeOut(TTimerKey timerKey, void *para)
{
}

string HttpStackPsa::getRequestLinePrefix(string reqline)
{
	std::size_t index=reqline.find("?");
	if(index!=std::string::npos)
	{
		return reqline.substr(0,index);
	}
	else
	{
		return reqline;
	}
}

void HttpStackPsa::sendHttpReq(HttpMessage* msg)
{
	/*HttpMessage* msg=new HttpMessage(1);
	msg->setReqLine("POST /notify HTTP/1.1");
	string a="{\"rsc\":{\"notify\":{\"did\":\"100001_pVFZ0xZu3R\",\"offline\":1,\"nfy\":\"efwef\"}}}";
	msg->setBody(a.c_str(),a.length());

	const char* ip="10.109.247.85";
	int port=8196;*/
	const char* ip=msg->getip();
	int port=msg->getport();
	if(ip&&port)
	{
		mClient->sendPacket(ip,port,msg);
	}
	else
	{
		UniERROR("ip port error")
	}
}
