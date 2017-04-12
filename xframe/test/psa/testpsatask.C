#include <stdlib.h>
#include "testpsatask.h"

TestPsaTask::TestPsaTask():TPSATask()
{
	localport=9000;
	remoteport=9001;
	mRecvCount=0;
	mSendCount=0;
}

TestPsaTask::~TestPsaTask()
{
}
 
BOOL TestPsaTask::onInit(TiXmlElement*	extend)
{
	TiXmlHandle handle(extend);
	TiXmlElement*	task=NULL;
	task=handle.FirstChild("task").Element();
	if(task)
	{
		if(!task->Attribute("localport", &localport)) localport=9000;
		if(!task->Attribute("remoteport", &remoteport)) remoteport=9001;		
	}
	
	UniINFO("LocalPort=%d, RemotePort=%d...", localport,remoteport );
	mTK1=setTimer(1);

	BOOL success = socket.openServer(NULL, localport);
    	if(!success)
    	{
        	UniERROR("bind server error! port=%d",localport);
    	}
    	else
    	{
    		UniINFO("bind server! port=%d",localport);
    	}

	return TRUE;
}


void TestPsaTask::procMsg(std::auto_ptr<TUniNetMsg> msg)
{
	TUniNetMsg* unimsg=msg.get();
	if(unimsg!=NULL)
	{
		if(unimsg->oAddr.logAddr==1001) mSendBuffer.push_back(unimsg->msgName);
	}
	
}



void TestPsaTask::onTimeOut(TTimerKey timerKey, void* para)
{
	if(timerKey==mTK1)
	{
		UniINFO("Recv %d and Send %d.", mRecvCount, mSendCount);
		mTK1=setTimer(1);
	}
}

void TestPsaTask::onBuildFDSet(CFdSet& fdset)
{
	fdset.setRead(socket.getFd());
	fdset.setWrite(socket.getFd());
	fdset.setExcept(socket.getFd());
}	

void TestPsaTask::onCheckFDSet(CFdSet& fdset)		//判断是不是可以发收消息 
{
	if(fdset.readyToRead(socket.getFd()))
	{
		Recv();
	}
	if(fdset.readyToWrite(socket.getFd()))
	{
		Send();
	}
	if(fdset.hasException(socket.getFd()))
	{
		UniERROR("Exception!");
	}
}
		
	
void TestPsaTask::Recv()
{
	CCode code;
    code.content = NULL;
	char mSockBuffer[100];
	memset(mSockBuffer, 0, sizeof(mSockBuffer));
    code.content = mSockBuffer;
    code.length = 0;
    int recvstatus = socket.recvCode(code);  //从网络收消息
	if(recvstatus == 1 &&code.length >0)
	{
		CStr loginfo=mSockBuffer;
		TUniNetMsg* unimsg=new TUniNetMsg();
		unimsg->tAddr.logAddr=1001;
		unimsg->dialogType=DIALOG_BEGIN;
		unimsg->msgName=loginfo.toInt();
		sendMsg(unimsg);
		mRecvCount++;
	}
}

void TestPsaTask::Send()
{
	char mBuffer[100];
	int  i;
	while(mSendBuffer.size())  //向网络发消息
	{
		i=0;
		mSendBuffer.pop_front(i);
		memset(mBuffer, 0, sizeof(mBuffer));
		sprintf(mBuffer,"%d",i);
		CCode code;
		code.content=mBuffer;
		code.length = strlen(mBuffer);
        	socket.sendCode(code,"127.0.0.1", remoteport);
		mSendCount++;
	}
}


