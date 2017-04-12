
#include "taskcontainer.h"
#include "info.h"
#include "comconst.h"
#include "framectrl.h"

#include "msgdef_com.h"
#include "msghelper_com.h"

TTaskContainer::TTaskContainer():TGeneralObject()
{
	mTaskRxFifo = new Fifo<TMsg>();
	if(mTaskRxFifo==NULL)
	{
		UniERROR("ERROR!!!! Init TaskContainer error! taskRxFifo=NULL");
	}
	mTTQ = new TaskTimerQueue(*mTaskRxFifo);
	if(mTTQ==NULL)
	{
		UniERROR("ERROR!!!! Init TaskContainer error! taskTimerQueue=NULL");
	}

	mCheckStepTime=FALSE;

	mHostId=0;
	mContainerId=0;
	//
	mWaitAtQueue=TRUE;
	mWaitMS=1000;

	mMsgProcCount=0;
	mTimerProcCount=0;
	mInnerProcCount=0;
	
	curTime.tv_sec=0;
	curTime.tv_usec=0;
	lastTime.tv_sec=0;
	lastTime.tv_usec=0;
	msgStepTime=0;
	msgStepTime_avg=0;
	timerStepTime=0;
	timerStepTime_avg=0;
	//对100次定时器处理和100次消息处理的时间进行平均
	timerLoop=100;
	msgLoop=100;
	
}

TTaskContainer::~TTaskContainer()
{
	if(mTaskRxFifo!=NULL) 	delete mTaskRxFifo;
	if(mTTQ!=NULL) 			delete mTTQ;
	for(std::map<UINT, TTaskInfo*>::iterator iter=mTaskList.begin();iter!=mTaskList.end();iter++)
	{
		if(iter->second->task) delete iter->second->task;
	}
	mTaskList.clear();
}

BOOL TTaskContainer::init(UINT ctid, UINT hostid, TFRAMEControl* framectrl)
{
	if(framectrl==NULL) return FALSE;
	
	UniINFO("TTaskContainer Init: hostId %d, ctId %d.", hostid, ctid);
	
	mContainerId=ctid;
	mHostId=hostid;
	mFrameCtrl = framectrl;
}

UINT TTaskContainer::genTaskInst(UINT taskid)
{
	std::map<UINT,TTaskInfo*>::iterator it;
	it=mTaskList.find(taskid);
	if(it!=mTaskList.end())
	{
		return it->second->task->genTaskInst();
	}
	return 0;
}


UINT TTaskContainer::registerTask(UINT taskid, TAbstractTask* task)
{
	if(taskid>0 && task!=NULL)
	{
		task->init(mHostId, mContainerId, taskid, this);
		
		TTaskInfo* ti=NULL;
		ti=new TTaskInfo();
		if(!ti) return 0;
		ti->taskId=taskid;
		ti->task=task;
		ti->taskType=task->getObjectType();
		
		std::pair<std::map<UINT, TTaskInfo*>::iterator,bool> ret;
		ret = mTaskList.insert ( std::pair<UINT, TTaskInfo*>(taskid, ti) );
		if (ret.second==false) 
		{
			UniERROR("TaskID %s already existed",taskid);
			return 0;
		}
		return 1;
	}
	return 0;
}


BOOL TTaskContainer::process()
{
	//TODO:检查定时器的逻辑
	unsigned int nextstep = mTTQ->process();
	if(mWaitAtQueue)
	{
		if(nextstep>mWaitMS) nextstep=mWaitMS;	//定时器返回的下一个定时器时间时间间隔，最长不超过1秒
	}
	else
	{
		nextstep=0;	//如果不在msg queue阻塞，则意味着应用想在别的地方阻塞，比如socket
	}

	TAbstractTask*	mTask;		//当前处理任务
	TTaskInfo*		mTaskInfo;
	std::auto_ptr<TMsg> msg=std::auto_ptr<TMsg>(mTaskRxFifo->getNext(nextstep));	//在消息队列最长等待时间是定时器下一个超时时间间隔
	if(msg.get())
	{
		TUniNetMsg* pUniNetMsg = NULL;
		pUniNetMsg=dynamic_cast<TUniNetMsg*>(msg.get());
		if(pUniNetMsg)
		{
			if(pUniNetMsg->tAddr.logAddr<=0)
			{
				//发送到container的消息一定是携带taskid的				
				return TRUE;
			}

			mTask=NULL;
			mTaskInfo=NULL;
			std::map<UINT,TTaskInfo*>::iterator iter;
			iter=mTaskList.find(pUniNetMsg->tAddr.logAddr);
			if(iter==mTaskList.end())
			{
				//发送到container的消息一定是存在task的				
				return TRUE;
			}
			mTaskInfo=iter->second;
			if(mTaskInfo) 	mTask=mTaskInfo->task;
			else return TRUE;
			
			if(mTask)
			{
				_CHECK_STEPTIME_START();
				
				msg.release();
				mTask->setCurrentTime(time(0));
				mTask->process(std::auto_ptr<TUniNetMsg>(pUniNetMsg));
				
				_CHECK_STEPTIME_PROCMSG(mTaskInfo);
				
				mMsgProcCount++;
			}	
			return TRUE;
		}
		TTimeOutMsg* pTimeOutMsg = NULL;
		pTimeOutMsg=dynamic_cast<TTimeOutMsg*>(msg.get());
		if(pTimeOutMsg)
		{
			mTask=NULL;
			mTaskInfo=NULL;
			std::map<UINT,TTaskInfo*>::iterator iter;
			iter=mTaskList.find(pTimeOutMsg->timerTID);
			if(iter==mTaskList.end())
			{
				//发送到container的消息一定是存在task的				
				return TRUE;
			}
			
			mTaskInfo=iter->second;
			if(mTaskInfo) 	mTask=mTaskInfo->task;
			else return TRUE;
			
			if(mTask)
			{
				_CHECK_STEPTIME_START();

				mTask->setCurrentTime(time(0));
				mTask->processTimeout(pTimeOutMsg->timerKey, pTimeOutMsg->timerIID, pTimeOutMsg->timerPara);
				
				_CHECK_STEPTIME_TIMEOUT(mTaskInfo);
				
				mTimerProcCount++;
			}
			return TRUE;
		}
		TEventMsg* pEventMsg = NULL;
		pEventMsg=dynamic_cast<TEventMsg*>(msg.get());
		if(pEventMsg)
		{
			// taskID=0 广播消息
			// taskID>0 instID=0 对task的广播消息
			// taskID>0 instID>0 定点发送消息
			
			//如果是 set stepcheck on/off 消息，则自行设置，不再传递给task
			if(pEventMsg->eventID == KERNAL_APP_RELOAD)
			{
				CStr scmd[5];
				pEventMsg->eventInfo.split(scmd,5);
				if(scmd[0]=="set")
				{
					if(scmd[1]=="stepcheck")
					{
						if(scmd[2]=="on") mCheckStepTime=TRUE;
						else mCheckStepTime=FALSE;
						TEventMsg* rspmsg=new TEventMsg();
						if(rspmsg)
						{
							genEventRsp(pEventMsg, rspmsg, NULL);
							rspmsg->eventInfo<<"stepcheck=";
							if(mCheckStepTime) rspmsg->eventInfo<<"on";
							else rspmsg->eventInfo<<"off";
							post(rspmsg);
						}
						return TRUE;
					}
					else if(scmd[1]=="loglevel")
					{
						// set logLevel DEBUG     设置信息输出级别为调试
						// set logLevel INFO      设置信息输出级别为信息
						// set logLevel ERROR     设置信息输出级别为错误
						// set logLevel default	  设置信息输出级别为配置文件默认
						GeneralThreadEnv* pEnv=NULL;
						pEnv=(GeneralThreadEnv*)threadEnvGet();	//更新线程设置
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
						else
						{
							//默认都是ERROR
							if(pEnv)
							{
								pEnv->logLevel = Err;
							}
						}
						TEventMsg* rspmsg=new TEventMsg();
						if(rspmsg)
						{
							genEventRsp(pEventMsg, rspmsg, NULL);
							
							rspmsg->eventInfo<<"logLevel=";
							if(scmd[2].empty()) rspmsg->eventInfo<<"ERROR";
							else rspmsg->eventInfo<<scmd[2];
							post(rspmsg);
						}
						return TRUE;
					}
				}				
			}

			
			mTask=NULL;
			mTaskInfo=NULL;
			if(pEventMsg->taskID>0)
			{
				// 如果taskid=0，则是组播
				std::map<UINT,TTaskInfo*>::iterator iter;
				iter=mTaskList.find(pEventMsg->taskID);
				if(iter==mTaskList.end())
				{
					//发送到container的消息一定是存在task的				
					return TRUE;
				}
				mTaskInfo=iter->second;
				if(mTaskInfo) 	mTask=mTaskInfo->task;
				else return TRUE;
				
				if(mTask)
				{
					TEventMsg* rspmsg=new TEventMsg();
					if(rspmsg)
					{
						genEventRsp(pEventMsg, rspmsg, mTaskInfo);
						switch(pEventMsg->eventID)
						{
						case KERNAL_APP_INIT:
							mTask->onStart(rspmsg);
							break;
						case KERNAL_APP_HEATBEAT:
							mTask->onHeartBeat(rspmsg);
							break;
						case KERNAL_APP_RELOAD:
							mTask->onReload(rspmsg, pEventMsg->eventInfo);
							break;
						case KERNAL_APP_SHUTDOWN:
							mTask->onShutdown(rspmsg);
							break;
						case KERNAL_APP_STATUES:
							genStatus(rspmsg, mTaskInfo);
							mTask->onStatues(rspmsg, pEventMsg->eventInfo);
						default:
							break;
						}
						mInnerProcCount++;
						
						post(rspmsg);
					}						
				}
			}
			else if(pEventMsg->taskID==0)
			{
				// 如果taskid=0，则是广播
				for( std::map<UINT, TTaskInfo*>::iterator iter=mTaskList.begin(); iter!=mTaskList.end(); iter++)
				{
					mTask=NULL;
					mTaskInfo=NULL;
					mTaskInfo=iter->second;
					if(mTaskInfo) 	mTask=mTaskInfo->task;
					else continue;
					if(mTask)
					{
						TEventMsg* rspmsg=new TEventMsg();
						if(rspmsg)
						{
							genEventRsp(pEventMsg, rspmsg, mTaskInfo);
							
							switch(pEventMsg->eventID)
							{
							case KERNAL_APP_INIT:
								mTask->onStart(rspmsg);
								UniINFO("TaskContainer: ********* on task init, start ok! *****************");
								break;
							case KERNAL_APP_HEATBEAT:
								mTask->onHeartBeat(rspmsg);
								break;
							case KERNAL_APP_RELOAD:
								mTask->onReload(rspmsg, pEventMsg->eventInfo);
								break;
							case KERNAL_APP_SHUTDOWN:
								mTask->onShutdown(rspmsg);
								break;
							case KERNAL_APP_STATUES:
								genStatus(rspmsg, mTaskInfo);
								mTask->onStatues(rspmsg, pEventMsg->eventInfo);
							default:
								break;
							}
							mInnerProcCount++;
							
							post(rspmsg);
						}
					}
				}
			}    
			return TRUE;
		}
	}

	return TRUE;
}

void TTaskContainer::genStatus(TEventMsg* rspmsg, TTaskInfo* mTaskInfo)
{
	rspmsg->eventInfo<<"Status: HostID="<<mHostId<<", ContainerID="<<mContainerId<<", TaskID="<<mTaskInfo->taskId<<"\r\n";
	rspmsg->eventInfo<<" InstStatus: ProcMsgCount("<<mMsgProcCount<<"), ProcTimerMsgCount("<<mTimerProcCount<<"), ProcEventMsgCount("<<mInnerProcCount<<")\r\n";
	if(mCheckStepTime)
	{
		rspmsg->eventInfo<<"             AvgStepTime(UniMsg="<<msgStepTime_avg<<"ms; TimerMsg="<<timerStepTime_avg<<"ms)\r\n";
		rspmsg->eventInfo<<" TaskStatus: AvgStepTime(UniMsg="<<mTaskInfo->msgStepTime_avg<<"ms; TimerMsg="<<mTaskInfo->timerStepTime_avg<<"ms)\r\n";
	}
	else
	{
		rspmsg->eventInfo<<" TaskStatus:\r\n";
	}
}

void TTaskContainer::genEventRsp(TEventMsg* pEventMsg, TEventMsg* rspmsg, TTaskInfo* mTaskInfo)
{
	if(mTaskInfo!=NULL)
	{
		rspmsg->sender.taskType=mTaskInfo->taskType;
		rspmsg->sender.taskID=mTaskInfo->taskId;
	}
	else
	{
		rspmsg->sender.taskType=objtype_Kernal;
		rspmsg->sender.taskID=__TASKCONTAINER_TASKID;
	}
	rspmsg->sender.instID=mContainerId;
	rspmsg->eventID=pEventMsg->eventID;
	rspmsg->status=1;
	rspmsg->transID=pEventMsg->transID;
	rspmsg->taskID=pEventMsg->sender.taskID;
	rspmsg->instID=pEventMsg->sender.instID;	
}

void TTaskContainer::sendMsg(TUniNetMsg* msg)
{
	mFrameCtrl->add(msg);
}

void TTaskContainer::post(TMsg* msg)
{
	mFrameCtrl->postTo(msg);
}

TTimerKey TTaskContainer::setTimer(TTimeMark timerId, TTimeDelay timerDelay, UINT timerTID, UINT timerIID, void* para)
{
	//判断定时器时延一定要大于0
	if(timerDelay>0)
	return mTTQ->add(timerId, timerDelay, timerTID, timerIID, para);
	else
	return 0;
}

BOOL TTaskContainer::delTimer( TTimerKey timerKey)
{
	mTTQ->remove(timerKey);	//用户自定义对象会自动删除
	return TRUE;
}
