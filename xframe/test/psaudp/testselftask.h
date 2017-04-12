#if !defined(__TESTSELFTASK_HXX)
#define __TESTSELFTASK_HXX 

#include "comserv/comtypedef.h"
#include "task/abstracttask.h"
#include <map>

_CLASSDEF(TestSelfTask);
class TestSelfTask : public TAbstractTask
{
public:
	TestSelfTask();
	virtual ~TestSelfTask();

	TAbstractTask* clone(){ return (TAbstractTask*)( new TestSelfTask());}
	BOOL onInit(TiXmlElement*	extend);

	void procMsg(std::auto_ptr<TUniNetMsg> msg);
	void onTimeOut(TTimerKey timerKey, void* para);
	int mCount;
	int mRecvCountA;
	UINT mRecvTopA;
	int mRecvCountB;
	UINT mRecvTopB;
	int mSum;
	CStr mInfoA;
	CStr mInfoB;

	TTimerKey mTK1;
	TTimerKey mTK2;
};

#endif
