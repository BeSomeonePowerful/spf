#include <sys/select.h>

#include "transportthread.h"
#include "transport.h"
#include "info.h"

TTransportThread::TTransportThread(TTransport& task, const char* tname, int tLogType, int tLogLevel, UINT inst)
:mTask(task),TGeneralThread(tname, tLogType, tLogLevel, inst)
{
}

TTransportThread::~TTransportThread()
{

}

void TTransportThread::onstart()
{
	
}

void TTransportThread::process()
{
	int result=0;
	CFdSet fdset;
	
	fdset.reset();
	
	try
	{
		while(1)
		{
			mTask.onBuildFDSet(fdset);
			mTask.onResetMilliSeconds();
			
			result=fdset.select(mTask.mSelectMS);
			if(result>0)
			{
				//监测到有解阻事件，调用检查
				mTask.onCheckFDSet(fdset);
			}
			if(!mTask.process()) return;	//完成收发处理后，调用一次主循环; 返回FALSE，则退出线程
			
			fdset.reset();
                        tosleep(1);//to sleep 1 microsecond
		}
	}

	catch (exception& e)
	{
		UniERROR("Unhandled exception: %s" ,e.what());
	}
}

