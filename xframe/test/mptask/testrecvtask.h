#if !defined(__TESTRECVTASK_HXX)
#define __TESTRECVTASK_HXX 

#include "comserv/comtypedef.h"
#include "task/mptask.h"
#include <map>


class TDialogInfo : public TTaskContext
{
public:
	TMsgAddress org;
	TMsgAddress dst;
	int act;
	BOOL live;

	TTimerKey TM2;

	TDialogInfo() {act=0; live=FALSE;}

};

_CLASSDEF(TestRecvTask);
class TestRecvTask : public TMPTask
{

_MPCOMPONETDEF(TestRecvTask, TDialogInfo)

public:
	TestRecvTask();
	virtual ~TestRecvTask();

	BOOL onInit(TiXmlElement*	extend);

	void procMsg(std::auto_ptr<TUniNetMsg> msg);
	void onTimeOut(TTimerKey timerKey, TTimerPara* para);
	BOOL onContextExpire();

	int mCount;

	TTimerKey TM1;

};

#endif
