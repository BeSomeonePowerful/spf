
#include "abstracttask.h"
#include "taskcontainer.h"

#include "info.h"
#include "comconst.h"

TAbstractTask::TAbstractTask():TGeneralObject()
{
	objectType=objtype_Task;
	memset(mMsgProcCount,0,sizeof(mMsgProcCount));
	mTimerProcCount=0;
	
	logToSyslog=0;
	syslogTaskID=10;
	
	mContainerId=0;
	mHostId=0;
	mInstId=0;
	mTaskId=0;
}

TAbstractTask::~TAbstractTask()
{

}

BOOL TAbstractTask::init(UINT hostid, UINT containerid, UINT taskid, TTaskContainer* taskctrl)
{
	if(taskctrl==NULL) return FALSE;
	
	mInstId=1;	//默认是第I个对象
	mContainerId=containerid;
	mHostId=hostid;
	mTaskId=taskid;
	mTaskCtrl = taskctrl;
	genPhyAddr(mHostId, mContainerId, mPhyAddr); 

	UniINFO(" Init Task: taskId(logAddr)=%d, hostId/containerid(phyAddr)=%d/%d(%d), instId=%d \n", mTaskId, mHostId, mContainerId, mPhyAddr, mInstId );
	
	TTaskEnv env(mTaskId);
	env.load();

	CStr task_env;
	env.list(task_env);
	
	UniINFO(" Task env: %s", task_env.c_str());

	logType=env.getLogType();
	logLevel=env.getLogLevel();
	logToSyslog=env.logToSyslog();
	syslogTaskID=env.syslogTaskID();
	env.getTaskInfo(objectType, objectName);
	mTME.clear();
	env.getTimerRsc(mTME);
	mDB.clear();
	if(env.getDBRsc(mDB)) mDB.conn();
	
	return onInit(env.extend);
}

void TAbstractTask::sendMsg(TUniNetMsg* msg)
{
	//将消息放入队列，由kernel进行处理
	msg->sender.taskType=objectType;
	msg->sender.taskID=mTaskId;
	msg->sender.instID=mContainerId;
	msg->oAddr.logAddr=mTaskId;
	msg->oAddr.phyAddr=mPhyAddr;
	msg->oAddr.taskInstID=mInstId;

	mTaskCtrl->sendMsg(msg);
}


void  TAbstractTask::sendMsg(std::auto_ptr<TUniNetMsg> msg)
{
	TUniNetMsg* pUniNetMsg = msg.release();	//释放自动指针，发送消息对象
	if(pUniNetMsg) sendMsg(pUniNetMsg);
}

void  TAbstractTask::postEvent(TEventMsg* msg)
{
	if(msg) post(msg);	
}

void  TAbstractTask::postEvent(std::auto_ptr<TEventMsg> msg)
{
	TEventMsg* pEventMsg = msg.release();
	if(pEventMsg) post(pEventMsg);
}

void  TAbstractTask::post(TMsg* msg)
{
	msg->sender.taskType=objectType;
	msg->sender.taskID=mTaskId;
	msg->sender.instID=mContainerId;
	mTaskCtrl->post(msg);
}

void  TAbstractTask::sendMsgSelf(TMsg* msg)
{ 
	mTaskCtrl->add(msg); 
}

TTimerKey TAbstractTask::setTimer(TTimeMark timerId, TTimeDelay timerDelay, void* para)
{
	//判断定时器时延一定要大于0
	if(timerDelay>0)
	return mTaskCtrl->setTimer(timerId, timerDelay, mTaskId, 1, para);
	else
	return 0;
}

TTimerKey TAbstractTask::setTimer(TTimeMark timerId, void* para)
{
	if(timerId>=0 && timerId<__TASK_TIMER_NUM)
	return setTimer(mTME[timerId].timerId, mTME[timerId].timerDelay, para);
	else
	return 0;
}

void TAbstractTask::processTimeout(TTimerKey timerKey, UINT timerIID, void* para)
{
	mTimerProcCount++;
	onTimeOut(timerKey, timerIID, para);
}

void TAbstractTask::onTimeOut(TTimerKey timerKey, void* para)
{
	UniDEBUG("Task %d Time out !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n",mTaskId);

	//不要删除定时器，超时之后，定时器自动删除了
	//用户也不要自行删除para，定时器消息析构的时候会自动删除
}

void TAbstractTask::onTimeOut(TTimerKey timerKey, UINT timerIID, void* para)
{
	//如果重载了该接口，需要注意，不带timerIID参数的接口需要在这里调用
	onTimeOut(timerKey, para);
}

BOOL TAbstractTask::delTimer( TTimerKey timerKey)
{
	return mTaskCtrl->delTimer(timerKey);	//用户自定义对象会自动删除
}

void TAbstractTask::onStart(TEventMsg* msg)
{
	BOOL ret=preTaskStart();

	if(msg)
	{
		if(ret)	msg->status=1;	//1=active
		else msg->status=2;		//2=inactive
	}
}

void TAbstractTask::onShutdown(TEventMsg* msg)
{

	BOOL ret=preTaskShutdown();
	if(msg)
	{
		if(ret)	msg->status=1;	//1=shutdown
		else msg->status=2;		//2=still active
	}
}

void TAbstractTask::onReload(TEventMsg* msg, CStr& cmd)
{
	// set logLevel DEBUG     设置信息输出级别为调试
	// set logLevel INFO      设置信息输出级别为信息
	// set logLevel ERROR     设置信息输出级别为错误
	// set logLevel default	  设置信息输出级别为配置文件默认
	// reload all			  重新加载所有配置
	// reload log             重新加载日志输出级别
	// reload db              重新加载数据库配置
	// reload timer           重新加载定时器配置
	TTaskEnv env(mTaskId);
	GeneralThreadEnv* pEnv=NULL;
	pEnv=(GeneralThreadEnv*)threadEnvGet();	//更新线程设置

	CStr scmd[5];
	cmd.split(scmd,5);
/*  //移动到container，这里只重新加载配置文件
	if(scmd[0]=="set")
	{
		if(scmd[1]=="logLevel")
		{
			if(scmd[2]=="DEBUG")
			{
				if(pEnv)
				{
					pEnv->logLevel = Debug;
				}
			}
			else if(scmd[2]=="INFO")
			{
				if(pEnv)
				{
					pEnv->logLevel = Info;
				}
			}
			else if(scmd[2]=="ERROR")
			{
				if(pEnv)
				{
					pEnv->logLevel = Err;
				}
			}
			else if(scmd[2]=="default")
			{
				if(pEnv)
				{
					pEnv->logLevel = logLevel;
				}
			}
		}		
	}
	else */if(scmd[0]=="reload")
	{
		env.load();
		if(scmd[1]=="all" || scmd[1]=="log")
		{
			logType=env.getLogType();
			logLevel=env.getLogLevel();
			if(pEnv)
			{
				pEnv->logLevel = logLevel;
				pEnv->logType = logType;
			}
		}
		if(scmd[1]=="all" || scmd[1]=="db")
		{
			mDB.clear();
			if(env.getDBRsc(mDB))
			mDB.conn();
		}
		if(scmd[1]=="all" || scmd[1]=="timer")
		{
			mTME.clear();
			env.getTimerRsc(mTME);
		}
	}

	BOOL ret=reloadTaskEnv(cmd, env.extend);

	if(msg)
	{
		if(ret) msg->status=1;	//1=reload success
		else msg->status=2; 	//2=reload error
	}
}

void TAbstractTask::onHeartBeat(TEventMsg* msg)
{
	if(msg)
	{
		msg->status=1;	//1=active
	}
}

void TAbstractTask::onStatues(TEventMsg* msg, CStr& cmd)
{
	if(msg)
	{
		msg->eventInfo<<"             TaskName( "<<getObjectNameStr()<<" v"<<getBuildInfo()<<"), ";
		msg->eventInfo<<"msgCount: MESSAGE("<<mMsgProcCount[0]<<"),BEGIN("<<mMsgProcCount[1]<<"),CONTINUE("<<mMsgProcCount[2]<<"),END("<<mMsgProcCount[3]<<"),BROADCAST("<<mMsgProcCount[4]<<"),MULTICAST("<<mMsgProcCount[5]<<"),TIMER("<<mTimerProcCount<<")\r\n";
		getStatues(cmd, msg->eventInfo);
		msg->status=1;	//1=get statues
	}
}

void TAbstractTask::process(std::auto_ptr<TUniNetMsg> msg)
{
	TUniNetMsg* pUniNetMsg = NULL;
	pUniNetMsg=msg.get();
	if(pUniNetMsg)
	{
		if(pUniNetMsg->dialogType >= DIALOG_MESSAGE && pUniNetMsg->dialogType <=DIALOG_MULTICAST)
		{
			mMsgProcCount[pUniNetMsg->dialogType]++;
		}
		procMsg(msg);
	}
}

void TAbstractTask::procMsg(TUniNetMsg* msg)
{
	UniDEBUG("Task recieved Msg:%d-->%d\n",msg->oAddr.logAddr,msg->tAddr.logAddr);
}

void TAbstractTask::procMsg(std::auto_ptr<TUniNetMsg> msg)
{
	TUniNetMsg* pUniNetMsg = NULL;
	pUniNetMsg=msg.get();
	if(pUniNetMsg) procMsg(pUniNetMsg);
}

void TAbstractTask::sysLog(CStr &id, int logLevel, CStr &logInfo, int logType)
{
	if(!logToSyslog) return;
	sysLog(id.c_str(), logLevel, logInfo.c_str(), logType);
}

void TAbstractTask::sysLog(const char* id, int logLevel, CStr &logInfo, int logType)
{
	if(!logToSyslog) return;
	sysLog(id, logLevel, logInfo.c_str(), logType);
}

//Added by TianJun, to log user state
void TAbstractTask::sysLog(int logLevel, CStr &logInfo, int logType)
{
	if(!logToSyslog) return;
	sysLog(logLevel, logInfo.c_str(), logType);
}
void TAbstractTask::sysLog(const char* id, int logLevel, const char* logInfo, int logType)
{
	if(!logToSyslog) return;

	CStr log;
	if(logLevel>=LOG_EMERG && logLevel<=LOG_DEBUG)
	{
		log<<cLogLevel[logLevel];
	}

	log<<getObjectNameStr()<<" ";
	log<<mTaskId<<" ";
	log<<mInstId<<"/";
	log<<"id="<<id<<"/";
	log<<logInfo;
	
	TUniNetMsg* tMsg = new TUniNetMsg();

	COMMsgHelper::addMsgBody(tMsg, log);
	COMMsgHelper::addCtrlMsgHdr(tMsg, 0, logLevel, logType);
	
	tMsg->tAddr.logAddr = syslogTaskID;
	tMsg->dialogType = DIALOG_MESSAGE;
	sendMsg(tMsg);
}

//Added by TianJun on 2016.06.20, to log user state
void TAbstractTask::sysLog(int logLevel, const char* logInfo, int logType)
{
	if(!logToSyslog) return;

	CStr log;
	if(logLevel>=LOG_EMERG && logLevel<=LOG_DEBUG)
	{
		log<<cLogLevel[logLevel];
	}
	
	log<<getObjectNameStr()<<" ";
	log<<mTaskId<<" ";
	log<<mInstId<<"/";
	log<<logInfo;
	
	TUniNetMsg* tMsg = new TUniNetMsg();

	COMMsgHelper::addMsgBody(tMsg, log);
	COMMsgHelper::addCtrlMsgHdr(tMsg, 0, logLevel, logType);
	
	tMsg->tAddr.logAddr = syslogTaskID;
	tMsg->dialogType = DIALOG_MESSAGE;
	sendMsg(tMsg);
}
