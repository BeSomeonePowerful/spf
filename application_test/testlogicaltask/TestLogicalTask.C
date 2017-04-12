#include "TestLogicalTask.h"

TTestLogicalTask::TTestLogicalTask() : _addr_(0), TAbstractTask()
{
	//nothing to do here
}

TTestLogicalTask::~TTestLogicalTask()
{
	//nothing to do here
}

BOOL TTestLogicalTask::reloadTaskEnv(CStr& cmd, TiXmlElement* & extend)
{
	return TRUE;
}

BOOL TTestLogicalTask::onInit(TiXmlElement*	extend)
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
			_addr_ = 4;
		}
	}

	return TRUE;
}

void TTestLogicalTask::onTimeOut(TTimerKey timerKey, void* para)
{
	return;
}

BOOL TTestLogicalTask::preTaskStart()
{
    TUniNetMsg* unimsg = new TUniNetMsg();
	TRscMsgHdr* rschdr=new TRscMsgHdr();
	TRscMsgBody* rscbody=new TRscMsgBody();

    CStr _message_("This is a trival message from TestLogicalTask!");
    rscbody->rsc = _message_;

    CStr _destination_("10.109.247.80:8500");
    TURI _host_info_(_destination_);
    rschdr->host = _host_info_;

    rschdr->code = 100;


    CStr _ruri_("/satellite");
    rschdr->ruri = _ruri_;

    CStr _consumer_("consumer");
    rschdr->consumer = _consumer_;

    CStr _producer_("producer");
    rschdr->producer = _producer_;

    CStr _rid_("rid21");
    rschdr->rid = _rid_;


	unimsg->tAddr.logAddr = _addr_;
    unimsg->msgName = RSC_MESSAGE;
    unimsg->dialogType = DIALOG_MESSAGE;
    unimsg->msgType = RSC_MESSAGE_TYPE;
    unimsg->setTransId();
    unimsg->setCSeq(11);

	unimsg->setCtrlMsgHdr(rschdr);
	unimsg->setMsgBody(rscbody);
	sendMsg(unimsg);

    return TRUE;
}

void TTestLogicalTask::procMsg(std::auto_ptr<TUniNetMsg> msg)
{
	std::cout << "Logicaltask received message from RSCPSA!" << std::endl;
	TUniNetMsg* unimsg=msg.get();
	if(unimsg==NULL) return;

	TRscMsgHdr* rschdr=new TRscMsgHdr();
	TRscMsgBody* rscbody=new TRscMsgBody();

	rschdr=dynamic_cast<TRscMsgHdr*> (unimsg->getCtrlMsgHdr());
	rscbody=dynamic_cast<TRscMsgBody*> (unimsg->getMsgBody());

	std::cout << "Message Header are :[" << rschdr->ruri.c_str() << "][" << rschdr->host.c_str() << "]" << std::endl;
	std::cout << "Message is :" << rscbody->rsc.c_str() << std::endl;

	return;
}
