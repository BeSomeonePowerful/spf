#include "abstracttask.h"
#include "framectrl.h"
#include <time.h>

TFRAMEControl::TFRAMEControl( Fifo<TMsg>&	framemngFifo ):mPostFifo(framemngFifo),shutdown(TRUE),loopCount(0)
{
	mFifo = new Fifo<TMsg>();
	mTaskSelector = new TTaskSelector(this);
}

TFRAMEControl::~TFRAMEControl()
{
	if(mFifo!=NULL)
	{
		delete mFifo;
	}
}

TTaskSelector* TFRAMEControl::getTaskSelector()
{
	return mTaskSelector;
}

void  TFRAMEControl::init(UINT appid, int logT, int logL, int recycleTime)
{
	mAppID=appid;
	logType=logT; 
	logLevel=logL; 
	if(recycleTime) mTaskSelector->setRecycleTime(recycleTime);
}

BOOL TFRAMEControl::mainLoop()
{
	loopCount++;
	
	//add by LJL 20160714 同步时间----------------
	mCurrentTime=time(0);
	mTaskSelector->setCurrentTime(mCurrentTime);
	//--------------------------------------------
	
	//用智能指针会将消息释放掉
	TMsg * msg= mFifo->getNext(1000);	//在消息队列最长等待时间=1000ms
	if(msg)
	{
		TUniNetMsg* pUniNetMsg = NULL;
		pUniNetMsg=dynamic_cast<TUniNetMsg*>(msg);
		if(pUniNetMsg)
		{
			if(!shutdown && mTaskSelector->add(pUniNetMsg)) uninetmsgCountS++;
			else
			{
				uninetmsgCountF++;
				delete pUniNetMsg;
			}
			return TRUE;
		}
		TEventMsg* pEventMsg = NULL;
		pEventMsg=dynamic_cast<TEventMsg*>(msg);
		if(pEventMsg)
		{
			UniDEBUG("FrameCtrl: recv eventmsg, eventid=%d, taskid=%d, instid=%d",pEventMsg->eventID, pEventMsg->taskID, pEventMsg->instID);
			//taskid=1 发给framectrl, instid=0&&taskid=0 广播到framectrl 
			if(pEventMsg->instID==0 && (pEventMsg->taskID==__FRAMECONTROL_TASKID || pEventMsg->taskID==__TASKSELECTOR_TASKID || pEventMsg->taskID==0) ) 
			{
				switch(pEventMsg->eventID)
				{
				case KERNAL_APP_INIT:
					onInit(pEventMsg->transID);
					break;
				case KERNAL_APP_HEATBEAT:
					onHeartBeat(pEventMsg->transID);
					break;
				case KERNAL_APP_RELOAD:
					onReload(pEventMsg->transID);
					break;
				case KERNAL_APP_SHUTDOWN:
					onShutdown(pEventMsg->transID);
					break;
				case KERNAL_APP_STATUES:
					onStatues(pEventMsg->transID, pEventMsg->eventInfo);
				default:
					break;
				}
			}
			//只要taskid > 2 就需要发给task
			if(pEventMsg->instID>=0 &&( pEventMsg->taskID>=__TASKSELECTOR_TASKID || pEventMsg->taskID==0))
			{
				mTaskSelector->addEvent(pEventMsg);	//taskid=0,广播的event
			}	
			else 
			{
				delete pEventMsg;	//本地处理完成，释放消息
				pEventMsg=NULL;
			}
			return TRUE;			
		}
		delete msg;
		msg=NULL;
	}
	return TRUE;
}

BOOL TFRAMEControl::post(TMsg* pMsg)
{
	if(pMsg)
	{
		pMsg->sender.taskID=__FRAMECONTROL_TASKID;
		pMsg->sender.taskType=objtype_Kernal;
		pMsg->sender.instID=0;
		mPostFifo.add(pMsg);
		return TRUE;
	}
	else return FALSE;
}

BOOL	TFRAMEControl::onInit(UINT transid)
{
	TEventMsg* msg=new TEventMsg();
	if(msg)
	{
		UniINFO("FrameCtrl: *********** on received init eventmsg, started! ************");
		msg->eventID=KERNAL_APP_INIT;
		msg->transID=transid;
		msg->status=1;	//1=active
		shutdown=FALSE;		//收到init 消息后，kernal 模块启动
		return post(msg);

	}
	return FALSE;
}

BOOL	TFRAMEControl::onHeartBeat(UINT transid)
{
	TEventMsg* msg=new TEventMsg();
	if(msg)
	{
		msg->eventID=KERNAL_APP_HEATBEAT;
		msg->transID=transid;
		msg->status=1;	//1=active
		UniDEBUG("FrameCtrl: on received KERNAL_APP_HEATBEAT eventmsg, transid=%d",transid);
		return post(msg);
	}
	else return FALSE;
}

BOOL	TFRAMEControl::onReload(UINT transid)
{
	TEventMsg* msg=new TEventMsg();
	if(msg)
	{
		msg->eventID=KERNAL_APP_RELOAD;
		msg->transID=transid;
		msg->status=1;	//1=sucess
		UniDEBUG("FrameCtrl: send KERNAL_APP_RELOAD eventmsg resp, transid=%d!",transid);
		return post(msg);
	}return TRUE;

}

BOOL	TFRAMEControl::onShutdown(UINT transid)
{

	shutdown=TRUE;		//收到shutdown 消息后，kernal 模块关闭

	TEventMsg* msg=new TEventMsg();
	if(msg)
	{
		UniINFO("FrameCtrl: *********** stoped! ************");
		msg->eventID=KERNAL_APP_SHUTDOWN;
		msg->transID=transid;
		msg->status=1;	//1=active
		return post(msg);
	}

	return TRUE;

}

BOOL	TFRAMEControl::onStatues(UINT transid, CStr& eventinfo)
{
	TEventMsg* msg=new TEventMsg();
	if(msg)
	{
		msg->eventID=KERNAL_APP_STATUES;
		msg->transID=transid;
		msg->status=1;	//1=get statues
		msg->eventInfo<<"FrameCtrlStatus: loop="<<loopCount<<", procmsg="<<uninetmsgCountS<<", failemsg="<<uninetmsgCountF<<", wait="<<mFifo->size()<<"\r\n";
		mTaskSelector->getStatues(msg->eventInfo);
		UniDEBUG("FrameCtrl: send KERNAL_APP_STATUS eventmsg resp: transid=%d,eventinfo=%s",transid, msg->eventInfo.c_str());	
		return post(msg);
	}
	return TRUE;
}


