#include "TestTask.h"

TTestTask::TTestTask() : _addr_(0), TAbstractTask()
{
	//nothing to do here
}

TTestTask::~TTestTask()
{
	//nothing to do here
}

BOOL TTestTask::reloadTaskEnv(CStr& cmd, TiXmlElement* & extend)
{
	return TRUE;
}

BOOL TTestTask::onInit(TiXmlElement*	extend)
{
	TiXmlHandle handle(extend);
	TiXmlElement*	task=NULL;
	task=handle.FirstChild("task").Element();
	if(task)
	{
		TiXmlHandle psahandle(task);
		TiXmlElement*	target=NULL;
		target=psahandle.FirstChild("target").Element();
		
		if(target)
		{
			target->Attribute("psaTask", &_addr_);
		}
		else
		{
			_addr_ = 6;
		}
	}

	return TRUE;
}

void TTestTask::onTimeOut(TTimerKey timerKey, void* para)
{
	return;
}

void TTestTask::procMsg(std::auto_ptr<TUniNetMsg> msg)
{
	std::cout << "message from RSCPSA!" << std::endl;
	TUniNetMsg* unimsg=msg.get();
	if(unimsg==NULL) return;

	TRscMsgHdr* rschdr=new TRscMsgHdr();
	TRscMsgBody* rscbody=new TRscMsgBody();

	rschdr=dynamic_cast<TRscMsgHdr*> (unimsg->getCtrlMsgHdr());
	rscbody=dynamic_cast<TRscMsgBody*> (unimsg->getMsgBody());
    
    std::cout << "-----------tAddr header are :---------" << std::endl;
    std::cout << "tAddr:" << unimsg->tAddr.logAddr << std::endl;
    std::cout << "oAddr:" << unimsg->oAddr.logAddr << std::endl;

	std::cout << "-----------Message header are :---------" << std::endl;
	std::cout << "code:" << rschdr->code << std::endl;
	std::cout << "ruri:" << rschdr->ruri.c_str() << std::endl;
	std::cout << "consumer:" << rschdr->consumer.c_str() << std::endl;
	std::cout << "producer:" << rschdr->producer.c_str() << std::endl;
	std::cout << "host:" << rschdr->host.host.c_str() << std::endl;
	std::cout << "port:" << rschdr->host.port << std::endl;
	std::cout << "rid:" << rschdr->rid.c_str() << std::endl;
    
    std::cout << "-----------Message is :---------" << std::endl;
    ostrstream st;
    rscbody->print(st);
    std::cout << st.str() << std::endl;
    delete st.str();

    std::cout << rscbody->rsc.c_str() << std::endl;

	return;
}