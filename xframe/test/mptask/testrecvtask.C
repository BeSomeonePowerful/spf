

#include "testrecvtask.h"

TestRecvTask::TestRecvTask()
{
	mCount=0;
}

TestRecvTask::~TestRecvTask()
{
}
 
BOOL TestRecvTask::onInit(TiXmlElement*	extend)
{
//	UniINFO("Recv Task %d, Inst %d, on Init", getTaskId(), getInstId());
	TM1=setTimer(1);
	return TRUE;
}


void TestRecvTask::procMsg(std::auto_ptr<TUniNetMsg> msg)
{

	TUniNetMsg* unimsg=msg.get();
	if(!unimsg) return;
	
	mCount++;

	if(mCount==5)
	{
		getContext()->live=TRUE;
	}

	getContext()->org=unimsg->oAddr;
	getContext()->dst=unimsg->tAddr;
	getContext()->act++;

	if(getContextID()>0 && getContext()->fresh()) 
	{
		UniDEBUG("Recv Create Context Msg");
		if(!getContext()->live)
		{
			getContext()->TM2=setTimer(2);

			TUniNetMsg* unimsg2=new TUniNetMsg();
			unimsg2->tAddr=getContext()->org;
			unimsg2->dialogType=DIALOG_BEGIN;
			unimsg2->msgName=100;
												
			sendMsg(unimsg2);
		}
	}
	else
	{
		UniDEBUG("Recv in Context Msg");
	}
	UniDEBUG("Recv Task %d, Inst %d, Ctx %d", getTaskId(), getInstId(), getContextID());
	UniDEBUG("	Recv msg from: %d/%d/%d, to: %d/%d/%d", getContext()->org.logAddr, getContext()->org.taskInstID, getContext()->org.taskCtxID, getContext()->dst.logAddr, getContext()->dst.taskInstID, getContext()->dst.taskCtxID);
	UniDEBUG("	Recv msg count %d", getContext()->act);

	
}



void TestRecvTask::onTimeOut(TTimerKey timerKey, TTimerPara* para)
{
	if(TM1==timerKey)
	{
		UniDEBUG("Recver Status: Timer1 fire, ctx num=%d", getContextCount());
		TM1=setTimer(1);
	}
	if(getContext()->TM2==timerKey)
	{
		UniDEBUG("Recver Ctx Timer2 fire: ctx=%d", getContextID());
	}
}


BOOL TestRecvTask::onContextExpire()
{
	if(getContext()->live)
	{
		UniDEBUG("Recver Context %d Expire! set live=FALSE.", getContextID());
		getContext()->live=FALSE;
		return FALSE;
	}
	else return TRUE;
}



