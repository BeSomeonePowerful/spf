#include <sys/select.h>

#include "psathread.h"
#include "info.h"

TPSAThread::TPSAThread(TPSAContainer& task, const char* tname, int tLogType, int tLogLevel, UINT inst)
:mTask(task),TGeneralThread(tname, tLogType, tLogLevel, inst)
{
}

TPSAThread::~TPSAThread()
{

}

void TPSAThread::onstart()
{
	//if(env.logLevel!=mKn.logLevel) env.logLevel=mKn.logLevel;
	//if(env.logType!=mKn.logType) env.logType=mKn.logType;
}

void TPSAThread::process()
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
		}
	}

	catch (exception& e)
	{
		UniERROR("Unhandled exception: %s" ,e.what());
	}
}

