#if !defined(__TESTSENDTASK_HXX)
#define __TESTSENDTASK_HXX 

#include "socket.h"
#include "clist.h"
#include "comserv/comtypedef.h"
#include "task/psatask.h"
#include "msg/msgdef_com.h"

_CLASSDEF(CTestLink);
_CLASSDEF(TestPsaTask);
class CTestLink : public CTcpConnection
{
public:
	CTestLink(TestPsaTask* task);
	~CTestLink();
	void sendToLink(CStr& msg);	//发送到指定的link
	
	virtual int needSend() { return 1; }	//设置是否需要发送消息
    virtual void processSend(); 		 	//向网络发消息
    virtual int processRecv(); 		 		//从网络收消息
	virtual void printState(CStr& str);
	
	int mSendCount, mSendCountR, mSendProc;
	int mRecvCount, mRecvProc;
private:
	TestPsaTask* mTask;	
	CList<CStr> mSendBuffer;	//发消息缓存
	CStr mRecvBuffer;	//接受数据缓冲，TCP的话，接收到的消息可能是有粘包的，需要定界和拆开
	int  mClient;		//是不是客户端连接
};

class TestPsaTask : public TPSATask
{
public:
	TestPsaTask();
	virtual ~TestPsaTask();

	TAbstractTask* clone(){ return (TAbstractTask*)( new TestPsaTask());}
	BOOL onInit(TiXmlElement*	extend);

	void procMsg(std::auto_ptr<TUniNetMsg> msg);
	void onTimeOut(TTimerKey timerKey, void* para);
	
	void sendToTask(int linkid, CStr& msg);	//发送到kernal

	virtual void onBuildFDSet(CFdSet& fdset);		//设置fdset，应用可设置读、写等fd 
	virtual void onCheckFDSet(CFdSet& fdset);		//判断是不是可以发收消息 
	
	TTimerKey mTK1;
	TTimerKey mTK2;

	int localport;		//做Server的本地监听端口
	int remoteport;		//做Client的远端连接端口
	
	CTcpListener listener;
	CTcpConnectionManager links;
	
	int linknum;
	int clientnum;
	CList<int> mLinkList;
};

#endif
