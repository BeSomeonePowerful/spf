#include "psacontainer.h"
#include "psatask.h"

TPSAContainer::TPSAContainer():TTaskContainer()
{
	mWaitAtQueue=FALSE;	//协议栈在select阻塞，不在msgqueue阻塞
	mWaitMS=0;
	mSelectMS.tv_sec = 0;
	mSelectMS.tv_usec = 500000;
}

void TPSAContainer::onBuildFDSet(CFdSet& fdset)
{
	TPSATask* mTask;
	for( std::map<UINT, TTaskInfo*>::iterator iter=mTaskList.begin(); iter!=mTaskList.end(); iter++)
	{
		//设置fdset，应用可设置读、写等fd 
		mTask=NULL;
		if(iter->second) mTask=(TPSATask*)(iter->second->task);
		if(mTask)
		{
			mTask->onBuildFDSet(fdset);
		}			
	}
}

void TPSAContainer::onCheckFDSet(CFdSet& fdset)
{
	TPSATask* mTask;
	for( std::map<UINT, TTaskInfo*>::iterator iter=mTaskList.begin(); iter!=mTaskList.end(); iter++)
	{
		//判断是不是可以发收消息 
		mTask=NULL;
		if(iter->second) mTask=(TPSATask*)(iter->second->task);
		if(mTask)
		{
			mTask->onCheckFDSet(fdset);
		}			
	}
}

void TPSAContainer::onResetMilliSeconds()
{
	TPSATask* mTask;
	for( std::map<UINT, TTaskInfo*>::iterator iter=mTaskList.begin(); iter!=mTaskList.end(); iter++)
	{
		//重置在socket的阻塞时间，如果不重载，默认为setMilliSeconds设置的值
		mTask=NULL;
		if(iter->second) mTask=(TPSATask*)(iter->second->task);
		if(mTask)
		{
			mTask->onResetMilliSeconds();
		}			
	}			
}

void TPSAContainer::setMilliSeconds(unsigned long ms)
{
	//在socket阻塞时间，如果不设置，默认为500ms
	unsigned long sec, usec;
	sec=(ms/1000);
	usec=(ms%1000)*1000;
	
	//取所有设置的最小值
	if(mSelectMS.tv_sec>sec) mSelectMS.tv_sec=sec;
	if(mSelectMS.tv_usec>usec) mSelectMS.tv_usec=usec;
}

