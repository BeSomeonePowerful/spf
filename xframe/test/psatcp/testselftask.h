#if !defined(__TESTSELFTASK_HXX)
#define __TESTSELFTASK_HXX 

#include "comserv/comtypedef.h"
#include "task/abstracttask.h"
#include <map>

using namespace std;

struct LinkInfo
{
	int linkid;
	int recvcount;
	int topmsg;
	LinkInfo() { linkid=0; recvcount=0; topmsg=0;}
};
	
class LinkStatus
{
	public:
		LinkStatus() {};
		void printStatus(CStr& info);
		void set(int linkid, CStr& info);
	private:	
		std::map<int, LinkInfo> mLinks;
};

_CLASSDEF(TestSelfTask);
class TestSelfTask : public TAbstractTask
{
public:
	TestSelfTask();
	virtual ~TestSelfTask();

	TAbstractTask* clone(){ return (TAbstractTask*)( new TestSelfTask());}
	virtual BOOL onInit(TiXmlElement*	extend);

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
