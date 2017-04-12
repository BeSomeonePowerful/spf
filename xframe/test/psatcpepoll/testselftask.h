#if !defined(__TESTSELFTASK_HXX)
#define __TESTSELFTASK_HXX 

#include "comserv/comtypedef.h"
#include "task/abstracttask.h"
#include <map>


	struct LinkInfo
	{
		int linkid;
		int recvcount;
		int topmsg;
	};

	
class LinkStatus
{
	public:
		void printStatus(CStr& info);
		void set(int linkid, CStr& info);
	private:	
		CVector<LinkInfo> mLinks;
};

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
	int mSum;

	CStr mInfoA;
	CStr mInfoB;
	
	LinkStatus mStatusA;
	LinkStatus mStatusB;

	TTimerKey mTK1;
	TTimerKey mTK2;
};

#endif
