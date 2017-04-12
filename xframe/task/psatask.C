#include "psatask.h"

TPSATask::TPSATask():TAbstractTask()
{
	mWaitAtQueue=FALSE;	//协议栈在select阻塞，不在msgqueue阻塞
	mWaitMS=0;
	mSelectMS.tv_sec = 0;
    mSelectMS.tv_usec = 500000;
	objectType=objtype_Psa;
}

void TPSATask::setMilliSeconds(unsigned long ms)
{
	mSelectMS.tv_sec = (ms/1000);
    mSelectMS.tv_usec = (ms%1000)*1000;
}

