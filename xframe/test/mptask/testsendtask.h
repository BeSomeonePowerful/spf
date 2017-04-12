#if !defined(__TESTSENDTASK_HXX)
#define __TESTSENDTASK_HXX 

#include "comserv/comtypedef.h"
#include "task/mptask.h"
#include <map>

_CLASSDEF(TSendDialogInfo);
_CLASSDEF(TestSendTask);

class TSendDialogInfo:public TTaskContext
{
public:
	TMsgAddress org;
	TMsgAddress dst;

	TTimerKey mTK2;
	
};


class TestSendTask : public TMPTask
{

_MPCOMPONETDEF(TestSendTask,TSendDialogInfo)

public:
	TestSendTask();
	virtual ~TestSendTask();

	BOOL onInit(TiXmlElement*	extend);

	void procMsg(std::auto_ptr<TUniNetMsg> msg);
	void onTimeOut(TTimerKey timerKey, TTimerPara* para);
	int mCount;
	int mSum;
	TTimerKey mTK1;
};

#endif
