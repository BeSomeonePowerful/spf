

#include "testselftask.h"

TestSelfTask::TestSelfTask():TAbstractTask()
{
	mCount=1000;
	mSum=0;
	mRecvCountB=0;
	mRecvCountA=0;
	mRecvTopA=0;
	mRecvTopB=0;
}

TestSelfTask::~TestSelfTask()
{
}
 
BOOL TestSelfTask::onInit(TiXmlElement*	extend)
{
	UniINFO("Send Task %d, Inst %d, on Init, count=%d/T2...", getTaskId(), getInstId(), mCount);
	mTK1=setTimer(1);
	mTK2=setTimer(2);
	return TRUE;
}


void TestSelfTask::procMsg(std::auto_ptr<TUniNetMsg> msg)
{
        TUniNetMsg* unimsg=msg.get();
        if(!unimsg) return;

		if(unimsg->oAddr.logAddr==101) 
		{
			mInfoA.clear();
			mInfoA="101 msgcount="; 
			mRecvCountA++; 
			mInfoA<<mRecvCountA; 
			mInfoA<<"/msgid=";
			if(mRecvTopA<unimsg->msgName) mRecvTopA=unimsg->msgName;
			mInfoA<<mRecvTopA;
		}
		else if(unimsg->oAddr.logAddr==102) 
		{
			mInfoB.clear();
			mInfoB="102 msgcount="; 
			mRecvCountB++; 
			mInfoB<<mRecvCountB; 
			mInfoB<<"/msgid=";
			if(mRecvTopB<unimsg->msgName) mRecvTopB=unimsg->msgName;
			mInfoB<<mRecvTopB;
		}
}



void TestSelfTask::onTimeOut(TTimerKey timerKey, void* para)
{
	if(timerKey==mTK2)
	{
		mTK2=setTimer(2);
	//	UniINFO("Send Task %d, Inst %d, Send msg start... time %d", getTaskId(), getInstId(), time(0));
		int count=mCount;
		while(count)
		{	
			mSum++;
			TUniNetMsg* unimsg=new TUniNetMsg();
			unimsg->tAddr.logAddr=101;
			unimsg->dialogType=DIALOG_BEGIN;
			unimsg->msgName=mSum;
			sendMsg(unimsg);
			
			unimsg=new TUniNetMsg();
			unimsg->tAddr.logAddr=102;
			unimsg->dialogType=DIALOG_BEGIN;
			unimsg->msgName=mSum;
			sendMsg(unimsg);
			count--;
		}
	}
	else if(timerKey==mTK1)
	{
		UniINFO("Send msgcount&msgid=%d, Recvmsg(%s) Recvmsg(%s).", mSum, mInfoA.c_str(), mInfoB.c_str());
		mTK1=setTimer(1);
	}
}

