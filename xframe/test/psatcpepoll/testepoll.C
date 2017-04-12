

#include "testepoll.h"

TestEpollTask::TestEpollTask():TAbstractTask()
{
}

TestEpollTask::~TestEpollTask()
{
}
 
BOOL TestEpollTask::onInit(TiXmlElement*	extend)
{
	socket_test("10.109.247.80",9636);
	printf("333333333\n");
}

void TestEpollTask::procMsg(std::auto_ptr<TUniNetMsg> msg)
{
    printf("recv.........\n");
}

void TestEpollTask::onTimeOut(TTimerKey timerKey, void* para)
{
	printf("time out\n");
}	
