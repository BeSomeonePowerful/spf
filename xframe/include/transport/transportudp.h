
#ifndef __TTRANSPORT_UDP_H
#define __TTRANSPORT_UDP_H

#include <iostream>
#include <map>
#include "udpsocket.h"
#include "info.h"
#include "transport.h"
#include "msgdatadef.h"

const int SocketBufferSize=8193;

_CLASSDEF(TBind);
_CLASSDEF(TUDPTrasnport);
_CLASSDEF(TAbstractTask);
_CLASSDEF(TMsgAddress);
class TBind
{
public:
	int		mBindID;
	CStr	mHost;
	CStr	mMappingHost;
	int		mTransportType;		//comtypedef.h TransportUDP ...
	int		mPort;
	int		mIPVersion;			//4:IPv4, 6:IPv6
	CUDPSocket      mSocket;
	CList<TUniNetMsg*> mSendBuffer;
	
public:
	TBind():mBindID(0),mHost("0.0.0.0"),mMappingHost("0.0.0.0"),mTransportType(TransportUDP),mPort(0),mIPVersion(TransportIPv4) {}
	TBind(int bindid, const char* host, int port ):mBindID(bindid),mHost(host),mMappingHost(host),mTransportType(TransportUDP),mPort(port),mIPVersion(TransportIPv4) {}
	TBind(int bindid, const char* host, const char* mappinghost, int port, int transtype, int ipv):mBindID(bindid),mHost(host),mMappingHost(mappinghost), mTransportType(transtype),mPort(port),mIPVersion(ipv) {}
	int&	bindId() {return mBindID;}
	CStr&	sentHost() {return mHost;}
	CStr&	mappingHost() {return mMappingHost;}
	int&	transport() {return  mTransportType;}
	int&	sentPort()  {return mPort;}
	int&	protocolVersion() {return mIPVersion;}
	CUDPSocket& socket() {return mSocket;}

public:	
	BOOL	bind() {return mSocket.openServer(mHost.c_str(), mPort);}
	void	brief(CStr& info) { info<<"id="<<mBindID<<",host="<<mHost<<",port="<<mPort;}
};


class TUDPTrasnport : public TTransport
{
public:
	TUDPTrasnport(TAbstractTask* parent, TMsgAddress &addr);
	virtual ~TUDPTrasnport();

	BOOL addTransport(int bindid, const char* host, int port);
	void Recv(TBind* bind);
	void Send(TBind* bind);
		
public: 
	virtual void onRecvSendMsg(TUniNetMsg* msg);	//缓冲下发消息
	virtual void onBuildFDSet(CFdSet& fdset);		//设置fdset，应用可设置读、写等fd 
	virtual void onCheckFDSet(CFdSet& fdset);		//判断是不是可以发收消息 

private:
	INT				mBindsCount;		//绑定地址数量
	std::map<UINT, TBind*> 	mBinds;		//监听数量不会太多，也就几个，不会影响性能

	CCode	mSockBuffer;				//收发消息缓冲区 		
	UINT	mRecvCount;
	UINT	mSendCount;
	UINT	mSendFaile;
};


#endif

