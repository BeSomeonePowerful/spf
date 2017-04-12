
#ifndef __TTRANSPORT_H
#define __TTRANSPORT_H

#include <iostream>
#include "msgdef_com.h"
#include "fifo.h"
#include "fdset.h"

_CLASSDEF(TAbstractTask);
_CLASSDEF(TMsgAddress);
_CLASSDEF(TTransMsg);
_CLASSDEF(TTransport);

class TTransMsg
{
public:
	CHAR split[3];	//消息定界符
	CStr transid;	//消息事务id
	INT  cseq;		//消息序号
	INT  length; 	//不包括本消息头的消息内容长度
public:
	TTransMsg();
	void encode(CHAR* &buf);
	void decode(CHAR* &buf);
	int size();		//本消息头长度
	BOOL check();	//检查消息头是不是正确
};

class TTransport
{
public:
	TTransport(TAbstractTask* parent, TMsgAddress &addr);
	virtual ~TTransport();
	
	void add(TMsg* msg) { mFifo->add(msg); }
			
public: 
	virtual void onRecvSendMsg(TUniNetMsg* msg)=0;	//从kernal收到下发的消息，需进行缓冲排队，以等待发送
	virtual void onBuildFDSet(CFdSet& fdset)=0;		//设置fdset，应用可设置读、写等fd 
	virtual void onCheckFDSet(CFdSet& fdset)=0;		//判断是不是可以发收消息 
	virtual void onResetMilliSeconds() {}			//重置在socket的阻塞时间，如果不重载，默认为setMilliSeconds设置的值
	virtual void setMilliSeconds(unsigned long ms);	//在socket阻塞时间，如果不设置，默认为5ms

protected:
	void  postMsg(TUniNetMsg* msg);	//将收到的消息发送给parent
	void  postEvent(TEventMsg* msg);//将构造的事件发送给parent
	
private:
	friend class TTransportThread;
	BOOL process();		//仅供TTransportThread调用

	struct timeval mSelectMS; //默认的阻塞时间，默认初试取值500ms
	
	TAbstractTask* mParent;
	TMsgAddress    mParentAddr;

	BOOL shutdown;
	TEventMsg* shutdownmsg;
	
	Fifo<TMsg>*   mFifo;
};


#endif

