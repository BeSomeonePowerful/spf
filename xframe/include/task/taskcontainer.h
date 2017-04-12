/****************************************************************
* Copyright (c)2011, by BUPT
* All rights reserved.
*      The copyright notice above does not evidence any
*      actual or intended publication of such source code

* FileName:     taskcontainer.h $
* System:       xframe
* SubSystem:    kernal/task
* Author:       Li Jinglin
* Date：        2015.5.0
* Description:
		应用容器

****************************************************************/

#if !defined(_TASKCONTAINER_H_)
#define _TASKCONTAINER_H_

#include <strstream>

#include "comtypedef.h"
#include "comtypedef_vchar.h"
#include "fifo.h"
#include "framectrl.h"
#include "generalobj.h"
#include "msgdef.h"
#include "msg.h"
#include "fifosorted.h"
#include "env.h"
#include "info.h"
#include "timerqueue.h"
#include "msgdef_com.h"
#include "msghelper_com.h"
#include "abstracttask.h"

_CLASSDEF(TFRAMEControl);
_CLASSDEF(TTaskContainer);

class TTaskContainer : public TGeneralObject
{
public:
	TTaskContainer();
	virtual ~TTaskContainer();

public:	

	//由framemng模块调用，线程启动前的对象初始化
	BOOL init(UINT ctid, UINT hostid, TFRAMEControl* framectrl);
	
	//由taskSelector模块调用，系统初始化的时候，注册Container中的task对象
	//如果已经注册过，返回FALSE
	BOOL registerTask(UINT taskid, TAbstractTask* task);

	//由线程管理模块调用，线程的主循环
	BOOL  process();

	//由FrameCtrl 模块调用，向任务发送消息（处于多线程并发考虑，不做其他检查）
	void  add(TMsg* msg) { mTaskRxFifo->add(msg); }
	
	//由AbstractTask调用
	//向kernal发消息
	void      sendMsg(TUniNetMsg* msg);
	//向kernal发event
	void      post(TMsg* msg);
	//设置和取消定时器
	TTimerKey setTimer(TTimeMark timerId, TTimeDelay timerDelay, UINT timerTID, UINT timerIID, void* para);
	BOOL	  delTimer( TTimerKey timerKey);
	
	//返回本container的id
	UINT getContainerId() { return mContainerId; }
	
	//产生新的InstID
	UINT genTaskInst(UINT taskid);
	
	//当前队列中的等待数量
	UINT  waitCount() {mTaskRxFifo->size();}
	
protected:

	UINT  mMsgProcCount;
	UINT  mTimerProcCount;  
	UINT  mInnerProcCount;

	//当前主机id与线程实例id
	UINT  mHostId;
	UINT  mContainerId;
	BOOL  mCheckStepTime;	//是否检查步进时间，默认关闭，否则会消耗时间
	
	struct TTaskInfo
	{
		int  taskType;
		UINT taskId;
		TAbstractTask* task;
		
		//跟踪调试的时候使用，显示task执行时间
		int    msgStepTime;
		int	   msgStepTime_avg;
		int    timerStepTime;
		int	   timerStepTime_avg;
		int    msgLoop;
		int    timerLoop;
		TTaskInfo()
		{
			taskType=0;
			taskId=0;
			task=NULL;		
			msgStepTime=0;
			msgStepTime_avg=0;
			timerStepTime=0;
			timerStepTime_avg=0;
			msgLoop=100;
			timerLoop=100;		
		}
	};
	
	std::map<UINT, TTaskInfo*>  mTaskList;	//处理任务列表
	
	Fifo<TMsg>*	mTaskRxFifo; 	//发送到Task的消息队列缓冲
	BOOL  		mWaitAtQueue;	//是否在消息队列处阻塞，默认=TRUE，读消息队列的时候，如果TRUE，则在消息队列处等待超时；如果=FALSE，则不等待任何超时（应用可控制在哪里等待，比如在socket处）
	int	 		mWaitMS;		//如果mWaitAtQueue=TRUE，则等待时间=mWaitMS，默认1000，即1秒
	
	TaskTimerQueue* mTTQ;		//内部定时器对象
	
	TFRAMEControl* 	mFrameCtrl;	//控制器对象
	
	//跟踪调试的时候使用，显示task执行时间
	struct timeval curTime;
	struct timeval lastTime;
	int    msgStepTime;
	int	   msgStepTime_avg;
	int    timerStepTime;
	int	   timerStepTime_avg;
	int    msgLoop;
	int    timerLoop;

	void genStatus(TEventMsg* rspmsg, TTaskInfo* mTaskInfo);
	void genEventRsp(TEventMsg* reqmsg, TEventMsg* rspmsg, TTaskInfo* mTaskInfo=NULL);
};



#define _CHECK_STEPTIME_START() \
	if(mCheckStepTime)\
	{\
		gettimeofday(&lastTime, NULL);\
	}

#define _CHECK_STEPTIME_PROCMSG(mTaskInfo) \
	if(mCheckStepTime)\
	{\
		UINT steptime;\
		gettimeofday(&curTime, NULL);\
		steptime=(((curTime.tv_sec-lastTime.tv_sec)*1000000+curTime.tv_usec)-lastTime.tv_usec);\
		msgStepTime+=steptime;\
		msgLoop--;\
		if(!msgLoop)\
		{\
			msgLoop=100;\
			msgStepTime_avg=msgStepTime/msgLoop;\
			msgStepTime=0;\
		}\
		mTaskInfo->msgStepTime+=steptime;\
		mTaskInfo->msgLoop--;\
		if(!mTaskInfo->msgLoop)\
		{\
			mTaskInfo->msgLoop=100;\
			mTaskInfo->msgStepTime_avg=mTaskInfo->msgStepTime/mTaskInfo->msgLoop;\
			mTaskInfo->msgStepTime=0;\
		}\
	}

#define _CHECK_STEPTIME_TIMEOUT(mTaskInfo) \
	if(mCheckStepTime)\
	{\
		UINT steptime;\
		gettimeofday(&curTime, NULL);\
		steptime=(((curTime.tv_sec-lastTime.tv_sec)*1000000+curTime.tv_usec)-lastTime.tv_usec);\
		timerStepTime+=steptime;\
		timerLoop--;\
		if(!timerLoop)\
		{\
			timerLoop=100;\
			timerStepTime_avg=timerStepTime/timerLoop;\
			timerStepTime=0;\
		}\
		mTaskInfo->timerStepTime+=steptime;\
		mTaskInfo->timerLoop--;\
		if(!mTaskInfo->timerLoop)\
		{\
			mTaskInfo->timerLoop=100;\
			mTaskInfo->timerStepTime_avg=mTaskInfo->timerStepTime/mTaskInfo->timerLoop;\
			mTaskInfo->timerStepTime=0;\
		}\
	}				

#endif

