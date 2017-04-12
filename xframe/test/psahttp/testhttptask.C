

#include "testhttptask.h"

TestHttpTask::TestHttpTask():TAbstractTask()
{
	mCount = 0;
}

TestHttpTask::~TestHttpTask()
{
}
 
BOOL TestHttpTask::onInit(TiXmlElement*	extend)
{
	UniINFO("Send Task %d, Inst %d, on Init, count=%d/T2...", getTaskId(), getInstId(), mCount);
	mTK1=setTimer(1);
	mTK2=setTimer(2);
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
                //mServer->setServerAdapter(this);
                printf("11111111\n");
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
    printf("7777777\n");
	return TRUE;
}

void TestHttpTask::onTimeOut(TTimerKey timerKey, void* para)
{
	printf("qqqqqqqqqqqqq\n");
}

void TestHttpTask::procMsg(std::auto_ptr<TUniNetMsg> msg)
{
	printf("recv data...........\n");
        /*TUniNetMsg* unimsg=msg.get();
        if(!unimsg) return;
	if(unimsg->msgName!=HTTP_MESSAGE_REQ) return;
	THttpCtrlMsg* ctrl=(THttpCtrlMsg*)(unimsg->getCtrlMsgHdr());
	THttpMsg* body=(THttpMsg*)(unimsg->getMsgBody());
	if(ctrl==NULL || body==NULL) return;
	
	tbnet::HttpMessage * mm = dynamic_cast<tbnet::HttpMessage*>(body->hMsg);
	if(mm==NULL) return;
	if(mm->isRequest()) printf("rrrrrrrrrrrrrrrrr\n");*/

}
		
