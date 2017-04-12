#include "info.h"
#include "taskthread.h"

TTaskThread::TTaskThread(TTaskContainer& task, const char* tname,int tLogType, int tLogLevel, UINT inst)
:mTask(task),TGeneralThread(tname, tLogType, tLogLevel, inst)
{
}

TTaskThread::~TTaskThread()
{

}

void
TTaskThread::onstart()
{
	//if(env.logLevel!=mTask.logLevel) env.logLevel=mTask.logLevel;
	//if(env.logType!=mTask.logType) env.logType=mTask.logType;
}

void
TTaskThread::process()
{
	int i;
	try
	{
		i=10;
		while(i)
		{
			//Èç¹ûreturn FALSE£¬exit thread
			if(!mTask.process()) return;
			i--;
		}
		tosleep(1);//to sleep 1 microseconds
	}

	catch (exception& e)
	{
		UniERROR("Unhandled exception: %s" ,e.what());
	}
}

