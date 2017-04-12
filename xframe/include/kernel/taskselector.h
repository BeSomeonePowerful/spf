/****************************************************************
* Copyright (c)2011, by BUPT
* All rights reserved.
*      The copyright notice above does not evidence any
*      actual or intended publication of such source code

* FileName:    taskselector.h $
* System:       xframe
* SubSystem:    common
* Author:       Li Jinglin
* Date：        2010.4.4
* Description:
		用于管理task，包括task的加载、注册、消息分发处理等。
		
		20150522：增加对task的负荷分担判断，当某一个task排队的消息太多的时候，
				减小向该task的消息分发，以避免平均分担导致的超时和队列溢出
		20150723：在一个inst里面可以管理多个task，保证简单的task可以共享同一个线程		  
				
****************************************************************/
#if !defined(TaskSelector_H)
#define TaskSelector_H

#include <stdio.h>

#include "comtypedef.h"
#include "unihashtable.h"
#include "clist.h"
#include "cexpirebuffer.h"
#include "fifotimelimit.h"
#include "msgdef.h"
#include "taskcontainer.h"

_CLASSDEF(TAbstractTask);
_CLASSDEF(TTaskSelector);
_CLASSDEF(TTaskContainer);
_CLASSDEF(TFRAMEControl);

const UINT BalanceCountNumber=100;	//task进行负荷分担的时候，每个实例负荷消息数量

class TTaskSelector
{
public:
	//new selector的时候，要输入FrameCtrl的Task sendTo Fifo	
	TTaskSelector(TFRAMEControl* fc);	
	~TTaskSelector();
	void init(UINT appid);

	BOOL	add(TUniNetMsg* msg);	//发送消息
	BOOL	add(std::auto_ptr<TUniNetMsg> msg);
	BOOL	addEvent(TEventMsg* msg); 
	
	//注册容器
	//tcid=taskcontainer的标识
	BOOL	registerContainer(UINT tcid, TTaskContainer* ptc);
	//注册任务
	//携带ContainerID，task归属在指定Container上
	BOOL	registerTask(UINT taskid, UINT tcid, TAbstractTask* task);

	//ToDo：以后是否需要实现再说
	//	  void 	shutdownTask(UINT taskid);
	//	  void 	shutdownTContainer(UINT tcid);
	//    void 	unregisterTask(UINT taskid);
	//	  void 	unregisterTContainer(UINT tcid);

	INT 	getActiveTaskCounter() { return activeTaskCounter;}
	INT 	getRegisteredTaskAmount() { return registeredTaskAmount;}

	void	getStatues(CStr& status);

	//add by LJL 20160714 刷新时间
	void	setCurrentTime(time_t& t) { mCurrentTime=t;}
	void	setRecycleTime(int ct);
	
private:
	BOOL	sendMsgToTask(TUniNetMsg* msg);
	BOOL	multicastMsgToTask(TUniNetMsg* msg);
	BOOL	broadcastMsgToTask(TUniNetMsg* msg);


	//TaskContainer的实例状态
	struct TaskContainerState
	{
		TaskContainerState() {};
		TaskContainerState(UINT _tcid, TTaskContainer* ptc) : tcid(_tcid), waitCount(0), tc(ptc) {};
		UINT			tcid;
		TTaskContainer*	tc;
		UINT			waitCount;	//等待数量；所在TaskContainer的队列的排队数量（定期更新）
	};
	
	//Task的实例状态。每个inst包含归属的container
	struct InstState
	{
		InstState() {};
		InstState(UINT _tcid, TaskContainerState* ptc) : tcid(_tcid), count(0), weight(0),shuttingDown(FALSE), tcs(ptc) {};
		UINT			weight;		//权重，0=正常分担；1=不分担
		//	超过等待数量平均值 100，即超过一个轮询周期内的理论负载，权值=1
		UINT			count;		//向该inst发送消息的数量
		BOOL 			shuttingDown;	//是否停止处理消息
		UINT			tcid;		//taskcontainer的标识
		TaskContainerState*	tcs;
		BOOL operator==(const InstState& rhs) { return tcid == rhs.tcid; }
	};

	//task的信息，每个taskid包含inst的列表
	struct TaskState
	{
		TaskState(){};
		TaskState(UINT tid, CList<InstState*>* ts):taskid(tid),inst(ts), instpos(0), count(0), balanceCount(0){};
		TObjectType	taskType;	//task类型
		UINT 		taskid;	//taskid
		UINT 		instpos;	//当前选择的inst在inst列表中的位置
		UINT		count;	//处理消息计数，当处理消息数量超过balanceCount，则重新进行权重计算
		//modify by LJL 20150522
		UINT		balanceCount;	//Task的Inst数量*100，即每个Inst平均被轮询100次才重新计算一次balance权重，避免重复轮询			
		CList<InstState*>* 	inst;
		
		BOOL operator==(const TaskState& rhs) { return taskid == rhs.taskid; }
	};
	//taskid 在uninet消息中，是 logAddr
	//tcid 在uninet消息中，是 phyAddr中的threadid，用parsePhyAddr解析，相当于ContainerID
	//如果taskid>0, tcid=0
	//    由于task 有多个实例 ，因此该方法返回的是一个按照负荷分担选择的实例结果
	//    在标记某个inst 处于 shutdown 状态的时候，则不返回该实例，保证shutdown 的实例不会再处理新的对话请求
	//如果taskid>0, tcid>0
	//    已经明确tcid 的请求，一般是后继请求，还能选择标记为shutdown的实例，保证实例友好的结束
	TTaskSelector::TaskContainerState*	getTask(UINT taskid, UINT tcid);
	
	void balanceCheck(TaskState* ts);

	CHashTable<UINT, TaskContainerState*> mContainerList;	//TaskContainer的实例列表，主键为containerid，用于TaskSelector寻址
	CHashTable<UINT, TaskState*> 		 mTaskList;  		//按照TaskId 管理的任务列表，主键为TaskID，内容为TaskState的内容数组
	
	TFRAMEControl* 	mfc;		//framectrl的指针
	UINT      		mAppID;		//应用ID
	
	//add by LJL 20160714 增加对session的处理-----------------------------
	struct TaskKey
	{
		UINT tcid;	//task container id
		UINT tiid;	//task instant id
	};
	
	CExpireBuffer<UINT, TaskKey>    mIntDialog;	//以整数为key的dialog
	CExpireBuffer<CStr, TaskKey>    mStrDialog;	//以str为key的dialog
	
	time_t    mCurrentTime;					//当前时间，每次刷新的时候设置
	//--------------------------------------------------------------------	

	INT  	activeTaskCounter;		//活动的任务数量
	INT  	activeInstCounter;		//活动的实例数量
	INT  	registeredTaskAmount;	//注册的任务数量
	INT  	registeredInstAmount;	//注册的实例数量

	INT  	sendMsgToTaskCount;
	INT  	multicastMsgCount;
	INT 	broadcastMsgCount;
};

#endif
