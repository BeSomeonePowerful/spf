#if !defined(__TESTSENDTASK_HXX)
#define __TESTSENDTASK_HXX 

#include "udpsocket.h"
#include "clist.h"
#include "comserv/comtypedef.h"
#include "task/psatask.h"


_CLASSDEF(TestPsaTask);
class TestPsaTask : public TPSATask
{
public:
	TestPsaTask();
	virtual ~TestPsaTask();

	TAbstractTask* clone(){ return (TAbstractTask*)( new TestPsaTask());}
	BOOL onInit(TiXmlElement*	extend);

	void procMsg(std::auto_ptr<TUniNetMsg> msg);
	void onTimeOut(TTimerKey timerKey, void* para);


	virtual void onBuildFDSet(CFdSet& fdset);		//设置fdset，应用可设置读、写等fd 
	virtual void onCheckFDSet(CFdSet& fdset);		//判断是不是可以发收消息 
	void Recv();        //从网络收消息
	void Send();        //向网络发消息
	
	int mRecvCount;
	int mSendCount;
	
	TTimerKey mTK1;

	CUDPSocket socket;
	int localport;
	int remoteport;
	
	CList<int> mSendBuffer;
};

#endif
