#if !defined(__TESTSELFTASK_HXX)
#define __TESTSELFTASK_HXX 

#include "comserv/comtypedef.h"
#include "task/abstracttask.h"
#include "tcpsocket.h"
#include <map>

_CLASSDEF(TestEpollTask);
class TestEpollTask : public TAbstractTask
{
public:
	TestEpollTask();
	virtual ~TestEpollTask();

	TAbstractTask* clone(){ return (TAbstractTask*)( new TestEpollTask());}
	BOOL onInit(TiXmlElement*	extend);

	void procMsg(std::auto_ptr<TUniNetMsg> msg);
	void onTimeOut(TTimerKey timerKey, void* para);

};

#endif
