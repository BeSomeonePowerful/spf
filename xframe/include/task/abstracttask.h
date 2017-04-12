/****************************************************************
* Copyright (c)2011, by BUPT
* All rights reserved.
*      The copyright notice above does not evidence any
*      actual or intended publication of such source code

* FileName：      abstracttask.h
* System:       xframe
* SubSystem：     Kernel_Task
* Author:       Li Jinglin
* Date：        2010.4.4
* Version：      1.0
* Description：
	注意，TAbstractTask是提供给所有应用实现的上层Application的接口。
	TAbstractTask提供独立的process方法供线程调用，使用time limit fifo接收消息。
	
	20150724：李静林
		清理代码，增加taskContainer，支持一个线程实例运行多个task

********************************************************************/

#ifndef _TABSTRACTTASK_H
#define _TABSTRACTTASK_H

#include <strstream>
#include <memory> 

#include "comtypedef.h"
#include "comtypedef_vchar.h"
#include "generalobj.h"
#include "env.h"
#include "info.h"

#include "msgdef.h"
#include "msg.h"
#include "msgdef_com.h"
#include "msghelper_com.h"

using namespace std;

_CLASSDEF(TTaskContainer);
_CLASSDEF(TAbstractTask);
_CLASSDEF(TFRAMEManager);

class TAbstractTask : public TGeneralObject
{
public:
	TAbstractTask();
	virtual ~TAbstractTask();

public:
	//开放给子类的接口，子类可重载使用或直接调用
	
	//===============================================================================================
	//继承的应用可重载一下方法，以实现应用逻辑
	//================================================================================================
	//系统初始化之后加载，输入配置文件中应用扩展配置信息的指针，应用自行读取并保存到任务中
	virtual BOOL onInit(TiXmlElement*	extend) { return TRUE;}
	//Task 任务完成加载，处理消息之前
	virtual BOOL preTaskStart() {return TRUE;}
	//Task 任务关闭之前
	virtual BOOL preTaskShutdown() { return FALSE;}
	//重新加载配置，输入配置文件中应用扩展配置信息的指针，应用自行读取并保存到任务中
	//  cmd:	输入的命令行；extend: 输入的配置文件指针)
	virtual BOOL reloadTaskEnv(CStr& cmd, TiXmlElement* & extend) { return TRUE;}
	//获取Task当前处理状态
	//  cmd:   输入的命令行；st: 输出的状态
	virtual BOOL getStatues(CStr& cmd, CStr& st) {return TRUE;}

	//子类必须派生一个克隆函数，用于生成对象副本
	virtual TAbstractTask* clone()=0;
	//示例：TAbstractTask* newTask=new TXXTask();
	// 或定义一个 _COMPONETDEF(TXXTask)

	//子类如果想自行管理taskinstid，则重载该接口。默认返回task对象自己的instid
	virtual UINT genTaskInst() { return mInstId;}
	
	//=================================================================================================
	//任务消息处理函数
	//    包括消息处理函数和消息发送函数
	
	//procMsg
	//	消息处理函数。子类需要重载之，以实现消息处理功能。
	//输入参数是接收到的msg
	//
	//使用procMsg(TUniNetMsg* msg) 要注意，传入的msg 不能作为消息内容，再发送出去，因为这个指针会被本地释放
	//     如果需要使用转发，则重载procMsg(std::auto_ptr<TMsg>& msg) 是更合适的选择，在转发消息的时候，调用msg.release() 释放自动指针对消息指针的控制权
	virtual void procMsg(TUniNetMsg* msg);
	virtual void procMsg(std::auto_ptr<TUniNetMsg> msg);

	//sendMsg
	// 	发出一条消息。
	//输入参数
	//	应用要构造TMsg msg对象。
	// 	msg对象将会被内核自动销毁，上层应用需要保证每次发出的
	// 	msg是一个新的对象，且不能主动销毁之。
	//使用sendMsg(TUniNetMsg* msg) 要注意，msg一定是要new出来的新指针，不要直接用procMsg 传入的msg 指针，这个传入的指针会自动释放的
	//    如果使用sendMsg(std::auto_ptr<TMsg> msg) ，则不必担心，因为自动指针复制会自行移交指针控制权
	//发送的消息默认填写tmsg相关参数，特别是taskinstid，目前使用的是默认值（1），如需要修改，则可重载该函数
	virtual void  sendMsg(TUniNetMsg* msg);
	virtual void  sendMsg(std::auto_ptr<TUniNetMsg> msg);
	//向归属的container发消息，一般用于task的子线程向task主线程发消息
	void  sendMsgSelf(TMsg* msg);

	void  postEvent(TEventMsg* msg);
	void  postEvent(std::auto_ptr<TEventMsg> msg);

	//定时器操作函数
	//
	//setTimer
	//	设置一个定时器。此方法直接使用。子类无需重载。
	//输入参数:
	//    timerId		为定时器id，指示通过配置文件配置的定时器信息。
	//    timerDelay	为超时时间，单位为毫秒。不携带delay，则从配置文件中自动获取时延。
	//	  para			用户自定义参数，如果没有，无需传入此参数
	//返回值:
	//	timerKey		定时器的唯一标识，后继的操作需通过该定时器标识执行。
	virtual TTimerKey setTimer(TTimeMark timerId, TTimeDelay timerDelay, void* para=NULL);
	virtual TTimerKey setTimer(TTimeMark timerId, void* para=NULL);

	//delTimer
	//	根据timerKey删除一个定时器。
	BOOL delTimer(TTimerKey timerKey);

	//onTimeOut
	//	 定时器超时处理函数。
	//输入参数:
	//    timerKey		定时器标识，setTimer的时候返回的标识
	//	  timerIID		定时器设置的TaskInstID
	//	  para			用户自定义参数
	virtual void onTimeOut(TTimerKey timerKey, void* para);
	virtual void onTimeOut(TTimerKey timerKey, UINT timerIID, void* para);
	
	//sysLog
	//   系统日志
	//输入参数：
	//	 id    		应用自定义id，可以用于标识一个对话、序列等
	//	 logLevel	日志等级，包括：LOG_DEBUG，LOG_INFO，LOG_NOTICE，LOG_WARNING，LOG_ERR，LOG_CRIT，LOG_ALERT，LOG_EMERG，在info.h中定义
	//	 logInfo	日志输出信息内容
	//	 logType	日志类型，包括：MSGINFO, COUNTINFO, STATICINFO，应用内容，在本类中定义
	void sysLog(CStr &id, int logLevel,CStr &logInfo, int logType=MSGINFO);
	void sysLog(const char* id, int logLevel, const char* logInfo, int logType=MSGINFO);
	void sysLog(const char* id, int logLevel, CStr &logInfo, int logType=MSGINFO);

	void sysLog(int logLevel, CStr &logInfo, int logType=MSGINFO);
	void sysLog(int logLevel, const char* logInfo, int logType=MSGINFO);
	//////////////////////////////////
	///     成员变量访问接口
	//////////////////////////////////
	UINT getTaskId(){return mTaskId;}
	UINT getHostId(){return mHostId;}
	UINT getInstId() { return mInstId; }
	UINT getPhyAddr() {return mPhyAddr;}
	CStr getTaskName() { return mTaskName; }

	CDB* getDB(int i) {return mDB[i];}
	TTimeMark getTimerIDbyName(char* name) {return mTME.getTimerIDbyName(name);}

	time_t getCurrentTime() {return currentTime;}
	void   setCurrentTime(time_t t) {currentTime=t;}

	CStr& getBuildInfo() {return buildInfo;}
	CStr& getVerInfo() {return verInfo;}
	inline void setBuildInfo(char* baseversion, char* buildversion, char* buildby, char* buildtime)
	{
		getBuildInfo().fCat("  BaseVersion: %s\r\n",baseversion);
		getBuildInfo().fCat("  BuildVersion:%s\r\n",buildversion);
		getBuildInfo().fCat("  BuildBy:     %s\r\n",buildby);
		getBuildInfo().fCat("  BuildTime:   %s\r\n",buildtime);
		getVerInfo().fCat("v%s",baseversion );
	}

private:
	//只有taskContainer和TFRAMEManager才能访问AbstractTask封装的各个内部接口，派生类不能访问
	friend class TTaskContainer;
	friend class TFRAMEManager;
	
	//由taskcontainer模块调用，线程启动前的对象初始化
	BOOL init(UINT hostid, UINT containerid, UINT taskid, TTaskContainer* taskctrl);
	//由taskContainer模块调用
	void process(std::auto_ptr<TUniNetMsg> msg);
	void processTimeout(TTimerKey timerKey, UINT timerIID, void* para);
	//特定事件处理，由taskContainer调用。
	void onStart(TEventMsg* msg);
	void onShutdown(TEventMsg* msg);
	void onReload(TEventMsg* msg, CStr& st);
	void onHeartBeat(TEventMsg* msg);
	void onStatues(TEventMsg* msg, CStr& st);
	
	void post(TMsg* msg);

protected:
	BOOL	mWaitAtQueue;	//是否在消息队列处阻塞，默认=TRUE，读消息队列的时候，如果TRUE，则在消息队列处等待超时；如果=FALSE，则不等待任何超时（应用可控制在哪里等待，比如在socket处）
	int 	mWaitMS;		//如果mWaitAtQueue=TRUE，则等待时间=mWaitMS，默认1000，即1秒

	time_t currentTime;  //当前时间，系统调用procMsg和onTimeOut的时候更新，避免应用内部重新time(0)浪费时间
private:

	CStr buildInfo;
	CStr verInfo;

	UINT  mTaskId;
	UINT  mHostId;
	UINT  mContainerId;
	UINT  mPhyAddr;
	UINT  mInstId;
	CStr  mTaskName;
		
	UINT  mMsgProcCount[DIALOG_MULTICAST+1];	//外部消息计数
	UINT  mTimerProcCount;	//定时器消息计数

	TDBRsc			mDB;		//每个任务默认配置不超过5 个数据库连接，可通过mDB[i] 的方式来访问，i 是配置文件中配置的数据库序号id
	TTimerRsc 		mTME;		//每个任务默认配置不超过30个定时器

	TTaskContainer* 	mTaskCtrl;	//控制器对象
	int		logToSyslog;		//是否发送到sysLog开关
	int		syslogTaskID;		//sysLog 的taskID
	
};

#define _SETBUILDINFO setBuildInfo(BASEVERSION, BUILDVERSION, BUILDBY, BUILDTIME);
#define _COMPONETDEF(TASK) \
public: \
	virtual TAbstractTask* clone() \
	{ \
		TASK * p=NULL;\
		p= new TASK(); \
		if(p!=NULL) return (TAbstractTask*)p;}



#endif
