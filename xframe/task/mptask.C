
#include "mptask.h"
#include "info.h"
#include "env.h"
#include "comconst.h"

#include "msgdef_com.h"
#include "msghelper_com.h"

TMPTask::TMPTask()
{
	mTaskCID=0;
	mContextCount=0;
	
}

TMPTask::~TMPTask()
{
	
}

BOOL TMPTask::internalInit(TiXmlElement*	extend)
{
	TKernalEnv env;
	env.load();
	
	if(!env.getMPTaskInfo(mContextRecycleRate, mContextRefershInterval))
	{
		mContextRecycleRate=10;			//默认每次回收的时候，最多不超过10个
		mContextRefershInterval=600;	//默认上下文超时时间是600秒（10分钟）
	}
	UniINFO(" MPTask env: ContextRecycleRate=%d, ContextRefershInterval=%d", mContextRecycleRate, mContextRefershInterval );
	
	TTimerRsc timerList;
	timerList.clear();
	env.getTimerRsc(timerList);
	//初始化定时器取值
	timerContextRefresh=timerList.getTimerIDbyName("TIMER_MPTASK_CTXREC");
	if(timerContextRefresh>0 && timerContextRefresh<__TASK_TIMER_NUM)
	{
		timerContextRefreshDelay=timerList[timerContextRefresh].timerDelay;
		UniINFO(" MPTask env: ContextRefreshTimerID=%d, Delay=%d", timerContextRefresh, timerContextRefreshDelay );
	}
	else
	{
		timerContextRefresh=1;
		timerContextRefreshDelay=30000;	//默认半分钟一次超时查询，清理超时的对象
		UniINFO(" MPTask env: load error, use default. ContextRefreshTimerID=%d, Delay=%d", timerContextRefresh, timerContextRefreshDelay );
	}
		
	//初始化MPTask
	mTaskCID=0;
	mDefaultTaskCTX=onContextCreate();
	mDefaultTaskCTX->mCreateTag=FALSE;
	
	//context的生存周期设置为系统配置取值
	mTaskCTXList.setLivingTime(mContextRefershInterval);
	
	//启动Context回收定时器
	timerContextRefreshKey=TAbstractTask::setTimer(timerContextRefresh, timerContextRefreshDelay, mTaskCID, NULL );
	if(!timerContextRefreshKey)
	{
		UniERROR("MPTask: init contextReferesh timer error! timerid=%d", timerContextRefresh);
		CStr log;
		log<<"init contextReferesh timer error! timerid="<<timerContextRefresh;
		sysLog("MPTask", LOG_ERR, log);
	}
	
}

BOOL TMPTask::releaseContext(UINT cid)
{
	if(cid && cid!=mTaskCID)
	{
		mTaskCTXList[cid]->mReleaseTag=TRUE;
	}
	else
	{
		mTaskCTXList[mTaskCID]->mReleaseTag=TRUE;
	}
}

void TMPTask::sendMsg(TUniNetMsg* msg)
{
    //将消息放入队列，由kernel进行处理
	msg->oAddr.taskCtxID=getContextID();
	TAbstractTask::sendMsg(msg);	
}


void  TMPTask::sendMsg(std::auto_ptr<TUniNetMsg> msg)
{
	TUniNetMsg* pUniNetMsg = msg.release();	//释放自动指针，发送消息对象
	if(pUniNetMsg) sendMsg(pUniNetMsg);
}

TTimerKey TMPTask::setTimer(TTimerMark timerId, TTimerDelay timerDelay, TTimerPara* para)
{
	//判断定时器时延一定要大于0
	if(timerDelay>0)
		return TAbstractTask::setTimer(timerId, timerDelay, getContextID(), para);
	else
		return 0;
}

TTimerKey TMPTask::setTimer(TTimerMark timerId, TTimerPara* para)
{
	if(timerId>=0 && timerId<__TASK_TIMER_NUM)
		return TAbstractTask::setTimer(mTME[timerId].timerId, mTME[timerId].timerDelay, getContextID(), para);
	else
		return 0;
}

void TMPTask::onTimeOut(TTimerKey timerKey, TTimerPara* para)
{
    UniDEBUG("Task %d Time out !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n", getTaskId());

	//不要删除定时器，超时之后，定时器自动删除了
	//用户也不要自行删除para，定时器消息析构的时候会自动删除
}


void TMPTask::onTimeOut(TTimerKey timerKey, UINT timerCid, TTimerPara* para)
{
	if(timerContextRefreshKey==timerKey)
	{
	//Context的定时器刷新
		UINT i=mContextRecycleRate;
		while(mTaskCTXList.front_old(mTaskCID, mTaskCTX, currentTime))
		{
			if(mTaskCTX->mReleaseTag || onContextExpire())
			{
				//已经标记为被销毁，直接删除即可
				//没有标记被销毁，但超时提示校验返回TRUE，该CTX被销毁
				mTaskCTXList.pop_front();
				delete mTaskCTX;	//这个是指针，要单独释放
				mTaskCID=0;
				mTaskCTX=mDefaultTaskCTX;
				mContextCount--;
			}
			else
			{
				//返回FALSE，不销毁该CTX，相当于做了一次刷新
				mTaskCTXList.update(mTaskCID, currentTime);
			}
			i--;
			if(!i) break;
		}
		if(i==mContextRecycleRate)
		{
			//如果i==mContextRecycleRate，代表周期内没有超时的节点，可延迟下一个查询周期，减小定时器超时检查数量
			timerContextRefreshKey=setTimer(timerContextRefresh, timerContextRefreshDelay*10, 0 );
		}
		else
		{
			timerContextRefreshKey=setTimer(timerContextRefresh, timerContextRefreshDelay, 0 );
		}
		return;
	}
	//不是MPTask系统定时器超时
	//根据timerCid查找当前的Context
	if(timerCid>0)
	{
		mTaskCTX=mTaskCTXList[timerCid];
		if(mTaskCTX!=NULL)
		{
			mTaskCID=mTaskCTX->mCID;
			onTimeOut(timerKey, para);
			
			//如果超时处理后，CTX标识为被释放，则立即清理，不等超时
			if(mTaskCTX->mReleaseTag)
			{
				mTaskCTXList.pop(mTaskCID);
				delete mTaskCTX;	//这个是指针，要单独释放
				mTaskCID=0;
				mTaskCTX=mDefaultTaskCTX;
				mContextCount--;				
			}
			else
			{
				mTaskCTXList.update(mTaskCID, currentTime);
			}
		}
		else
		{
		//	如果没找到匹配的Ctx，则可能已经释放了，后期清理
			if(para!=NULL)
			{		
				delete para;
			}
		}
	}
	else
	{
		//timerCid=0, 是默认任务设置的定时器（如在初始化的时候，设置的定时器）
		mTaskCTX=mDefaultTaskCTX;
		mTaskCID=0;
		onTimeOut(timerKey, para);
	}
	return;
}

void TMPTask::procMsg(TUniNetMsg* msg)
{
    UniDEBUG("Task recieved Msg\n");
    UniDEBUG("%d---->%d\n",msg->oAddr.logAddr,msg->tAddr.logAddr);
}

void TMPTask::procMsg(std::auto_ptr<TUniNetMsg> msg)
{
    TUniNetMsg* pUniNetMsg = msg.get();
    if(pUniNetMsg) procMsg(pUniNetMsg);
}

void TMPTask::procMsg(std::auto_ptr<TUniNetMsg> msg, UINT taskCID)
{
	TUniNetMsg* pUniNetMsg = msg.get();
	if(pUniNetMsg==NULL) return;
	
	//根据taskCID查找当前的Context
	if(taskCID>0)
	{
		mTaskCTX=mTaskCTXList[taskCID];
		if(mTaskCTX!=NULL)
		{
			mTaskCID=mTaskCTX->mCID;
		}
		else
		{
			//没找到指定的cid，且cid>0，则应该是失效的消息，用默认ctx处理
			mTaskCID=0;
			mTaskCTX=mDefaultTaskCTX;
		}
	}
    else
	{
		//如果taskCID=0，dialogType=DIALOG_BEGIN, 则代表需创建新的CTX
		if(pUniNetMsg->dialogType==DIALOG_BEGIN)
		{
			mMaxTaskCID++;
			if(mMaxTaskCID==0) mMaxTaskCID=1;	//应用使用的CID总是从1开始
			int findit=50;
			//注意：判断mMaxTaskCID不冲突，如果当前并发的数量远小于 2^32，这个循环不会有几次。但如果并发数量极大，则这么做会导致处理性能下降（如何解决以后再研究）
			while(mTaskCTXList.find(mMaxTaskCID))
			{
				mMaxTaskCID++;
				if(mMaxTaskCID==0) mMaxTaskCID=1;
				findit--;
				if(!findit){ break;}	//不会死循环下去
			}
			if(findit)	//有可分配的ContextID
			{
				TTaskContext* p=onContextCreate();
				if(p!=NULL)
				{
					mTaskCTX=p;
					mTaskCID=mMaxTaskCID;
					mTaskCTX->mCID=mTaskCID;
					mTaskCTXList.push(mTaskCID,mTaskCTX);
					mContextCount++;

					pUniNetMsg->tAddr.taskCtxID=mTaskCID;
					//创建了新的上下文
				}
				else
				{
					UniERROR("MPTask: new Context Error!");
					sysLog("MPTask", LOG_ERR, "new Context Error!");
					return;
				}
			}
			else
			{
				//没有可分配的cid，使用默认cid
				//在这样的情况下，应用应该拒绝请求，或使用默认处理策略
				mTaskCTX=mDefaultTaskCTX;
				mTaskCID=0;
			}
		}
		else
		{
			//其他对话类型的消息，不创建CTX，使用默认Ctx处理
			mTaskCID=0;
			mTaskCTX=mDefaultTaskCTX;
		}
	}
	
	procMsg(msg);

	if(mTaskCID>0)
	{
		//Refresh Context List
		mTaskCTXList.update(mTaskCID, currentTime);

		if(mTaskCTX->mCreateTag) mTaskCTX->mCreateTag=FALSE;
	
	//如果超时处理后，CTX标识为被释放，则立即清理，不等超时
		if(mTaskCTX->mReleaseTag)
		{
			mTaskCTXList.pop(mTaskCID);
			delete mTaskCTX;	//这个是指针，要单独释放
			mTaskCID=0;
			mTaskCTX=mDefaultTaskCTX;
			mContextCount--;				
		}
	}
}

