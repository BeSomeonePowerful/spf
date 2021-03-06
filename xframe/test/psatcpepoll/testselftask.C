

#include "testselftask.h"

TestSelfTask::TestSelfTask():TAbstractTask()
{
	mCount=1;
	mSum=0;
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
	if(unimsg->msgName!=COM_MESSAGE_REQ) return;
	TComCtrlMsg* ctrl=(TComCtrlMsg*)(unimsg->getCtrlMsgHdr());
	TComMsg* body=(TComMsg*)(unimsg->getMsgBody());
	if(ctrl==NULL || body==NULL) return;
	mCount++;

	if(unimsg->oAddr.logAddr==101 && ctrl->status<20) 
	{
		mStatusA.set(ctrl->status, body->comMsg);	//status 携带的是linkid,comMsg携带的是msgid
	}	
	else if(unimsg->oAddr.logAddr==102 && ctrl->status<20) 
	{
		mStatusB.set(ctrl->status, body->comMsg);	//status 携带的是linkid,comMsg携带的是msgid
	}
	else
	{
		UniERROR("Error Msg from %d, linkid=%d", unimsg->oAddr.logAddr, ctrl->status);
	}
}



void TestSelfTask::onTimeOut(TTimerKey timerKey, void* para)
{
	if(timerKey==mTK2)
	{
		mTK2=setTimer(2);
	//	UniINFO("Send Task %d, Inst %d, Send msg start... time %d", getTaskId(), getInstId(), time(0));
		int count=2;
		while(count)
		{	
			mSum++;
			TUniNetMsg* unimsg=NULL;
			unimsg=new TUniNetMsg();
			unimsg->tAddr.logAddr=101;
			unimsg->dialogType=DIALOG_BEGIN;
			unimsg->msgName=COM_MESSAGE_REQ;
			TComCtrlMsg* ctrl=NULL;
			ctrl=new TComCtrlMsg();
			ctrl->status=0;		//默认发送，linkid=0，协议栈设置为轮询，从6个linkid中轮询一个发送
			TComMsg* body=NULL;
			body=new TComMsg();
			body->comMsg<<mSum;	//消息内容是当前的序号
			unimsg->setCtrlMsgHdr(ctrl);
			unimsg->setMsgBody(body);
			sendMsg(unimsg);

			mSum++;	
			unimsg=NULL;
			ctrl=NULL;
			body=NULL;	
			unimsg=new TUniNetMsg();
			unimsg->tAddr.logAddr=102;
			unimsg->dialogType=DIALOG_BEGIN;
			unimsg->msgName=COM_MESSAGE_REQ;
			ctrl=new TComCtrlMsg();
			ctrl->status=0;		//默认发送，linkid=0，协议栈设置为轮询，从6个linkid中轮询一个发送
			body=new TComMsg();
			body->comMsg<<mSum;	//消息内容是当前的序号
			unimsg->setCtrlMsgHdr(ctrl);
			unimsg->setMsgBody(body);
			sendMsg(unimsg);
			count--;
		}
	}
	else if(timerKey==mTK1)
	{
		mInfoA.clear();
		mInfoA="recv from 101: ";
		mStatusA.printStatus(mInfoA);

		mInfoB.clear();
		mInfoB="recv from 102: ";
		mStatusB.printStatus(mInfoB);

		UniINFO("Send msgcount&msgid=%d, Recv msgcount=%d, Recvmsg(%s) Recvmsg(%s).", mSum, mCount, mInfoA.c_str(), mInfoB.c_str());
		mTK1=setTimer(1);
	}
}

void LinkStatus::printStatus(CStr& info)
{
	for(int i=0;i<mLinks.count();i++)
	{
		info<<"  link="<<mLinks[i].linkid<<", recvmsg("<<mLinks[i].recvcount<<"), topmsgid("<<mLinks[i].topmsg<<")\n";
	}
}

void LinkStatus::set(int linkid, CStr& info)
{
	if(linkid<20)
	{
		mLinks[linkid].linkid=linkid;
		mLinks[linkid].recvcount++;
		mLinks[linkid].topmsg=info.toInt();
	}
}
		
