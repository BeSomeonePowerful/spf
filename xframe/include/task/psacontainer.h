
#if !defined(_PSACONTAINER_H_)
#define _PSACONTAINER_H_

#include "taskcontainer.h"
#include "fdset.h"

_CLASSDEF(TPSAContainer);

class TPSAContainer : public TTaskContainer
{
public:
	TPSAContainer();
	virtual ~TPSAContainer() {};
	

public:	

	void onBuildFDSet(CFdSet& fdset);		//设置fdset，应用可设置读、写等fd 
	void onCheckFDSet(CFdSet& fdset);		//判断是不是可以发收消息 
	void onResetMilliSeconds();			//可调用setMilliSeconds重置在socket的阻塞时间，如果不重载，默认为setMilliSeconds设置的值
	void setMilliSeconds(unsigned long ms);	//在socket阻塞时间，如果不设置，默认为500ms

	struct timeval mSelectMS; //默认的阻塞时间，默认初试取值500ms

};

#endif

