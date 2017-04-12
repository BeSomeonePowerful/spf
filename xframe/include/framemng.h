/****************************************************************
* Copyright (c)2011, by BUPT
* All rights reserved.
*      The copyright notice above does not evidence any
*      actual or intended publication of such source code

* FileName:    framectrl.h $
* System:       xframe
* SubSystem:    common
* Author:       zhangzhixiang
* Date：        2011.4.25
* Description:
		XframeManager主要用于task以及psa，framecontrol 的建立，以及各个线程的建立，运行等～～

****************************************************************/
#ifndef _TFRAMEMANAGER_H
#define _TFRAMEMANAGER_H

#include <map>
#include <vector>
#include <cstdio>
#include "defs.h"
#include "comtypedef.h"
#include "msgdef.h"
#include "msg.h"
#include "fifo.h"
#include "clist.h"
#include "unihashtable.h"
#include "env.h"
#include "info.h"
#include "udpsocket.h"

using namespace std;

_CLASSDEF(TFRAMEManager);
_CLASSDEF(TFRAMEControl);
_CLASSDEF(TGeneralThread);
_CLASSDEF(TKernalThread);
_CLASSDEF(TTaskThread);
_CLASSDEF(TPSAThread);
_CLASSDEF(TAbstractTask);
_CLASSDEF(TPSATask);
_CLASSDEF(TTaskContainer);
_CLASSDEF(TPSAContainer);
_CLASSDEF(TaskTimerQueue);
_CLASSDEF(TEnv);

/********************************************/
// this is for xclient
const int maxLen = 10000;

#pragma pack (1)
struct Command
{
	int type;  //0 present for reload,1 present for get status
	int taskId;
	int instId;
	char cmd[100];
	Command():taskId(-1), instId(-1)
	{
		cmd[0] = 0;
	}
};
/********************************************/


//singleton形式的Xframemanager
class TFRAMEManager
{
	//外部调用接口
public:
	TFRAMEManager();
	virtual ~TFRAMEManager();
	static TFRAMEManager * instance();

	//启动之前根据配置文件初始化
	void Init(UINT appID, char* appName);
	//在系统启动之前把应用的任务注册到manager中
	void registerTask(UINT taskID, TAbstractTask* task);
	//系统启动
	void Run();

private:
	//socket for external watch tool
	CUDPSocket socket;

	//消息引擎内核与内核线程
	TFRAMEControl *     framectrl;
	TKernalThread * 	kernalThread;

	int mLogType;
	int mLogLevel;

	CStr 	mSCIP;
	UINT	mSCPort;
	UINT	mSCHB;

	UINT 	mAppID;
	CStr 	mAppName;
	CStr 	mHostIP;
	CStr    mAppInfo;	//应用信息，get sclist 的时候返回，可获得系统信息

	//任务管理、容器管理、启动列表管理
	struct TInstance
	{
		CStr mInstName;		//实例名
		UINT mInstID;		//实例ID（=ThreadID=ContainerID）
		INT	 mInstType;		//实例类型（task或psa）
		TGeneralThread* mInstThread;	//实例线程
		TTaskContainer* mInstContainer;	//实例的task容器
	};

	CList<TProcRsc>					mProcList;	//配置文件中设置的系统启动列表
	std::map<UINT, TInstance*>		mInstList;	//应用实例列表<UINT instID, TInstance*>，每个实例包含一个线程，及对应的一个Container实例
	std::map<UINT, TAbstractTask*>	mTaskObj;	//应用注册的对象实例列表<UINT taskID, TAbstractTask* task>，即所有可用task的资源对象，用于派生新的对象实例

	//内部消息引擎
	Fifo<TMsg> * mFifo; 	//接受应用返回消息的队列
	UINT	mTransID;		//消息事务ID

	//实例
	static TFRAMEManager * _instance;

	//-------------------------------------------------------------------------------------------------
	// ToDo: 线程活动性管理
	TaskTimerQueue* mTTQ;			//内部定时器对象
	TTimerRsc 		mTME;		//每个任务默认配置不超过30个定时器

	BOOL   shutdown;

	//用于设置心跳的间隔，通过管理各个任务的心跳，判断各个任务的活动性，以支持对失败的线程恢复
	TTimeMark		timerIH;		//内部心跳定时器ID，提取定时器ID的字符串为“TIMER_INTERNAL_HEARTBEAT”
	TTimerKey		timerIHKey;		//对应心跳定时器的TimerKey

};

#endif
