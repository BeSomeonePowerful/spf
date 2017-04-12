

#include "testsendtask.h"

TestSendTask::TestSendTask()
{
	mCount=10;
	mSum=0;
}

TestSendTask::~TestSendTask()
{
}
 
BOOL TestSendTask::onInit(TiXmlElement*	extend)
{
	mTK1=setTimer(1);

        int count=mCount;
		while(count)
		{   
		       TUniNetMsg* unimsg=new TUniNetMsg();
		       unimsg->tAddr.logAddr=1001;
		       unimsg->dialogType=DIALOG_BEGIN;
		       unimsg->msgName=100;
		           
		       sendMsg(unimsg);
		       count--;
		       mSum++;
		}   

	return TRUE;
}


void TestSendTask::procMsg(std::auto_ptr<TUniNetMsg> msg)
{
	TUniNetMsg* unimsg=msg.get();
	if(getContext()->fresh())
	{
		getContext()->org=unimsg->oAddr;
		getContext()->dst=unimsg->tAddr;
		getContext()->mTK2=setTimer(2);
	}

}



void TestSendTask::onTimeOut(TTimerKey timerKey, TTimerPara* para)
{

	if(timerKey==getContext()->mTK2)
	{
		getContext()->mTK2=setTimer(2);
		TUniNetMsg* unimsg=new TUniNetMsg();
		unimsg->tAddr=getContext()->org;
		unimsg->dialogType=DIALOG_CONTINUE;
		unimsg->msgName=100;
	
		sendMsg(unimsg);
		mSum++;

		UniINFO("Sender Send Refresh Msg to Task %d, Inst %d, Ctx %d.", getContext()->org.logAddr, getContext()->org.taskInstID, getContext()->org.taskCtxID);
	}
	else if(timerKey==mTK1)
	{
		UniINFO("Sender Status: Context count:%d, Send msg count %d.", getContextCount(), mSum);
		mTK1=setTimer(1);
	}
}

