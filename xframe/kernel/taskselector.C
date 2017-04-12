/****************************************************************
* Copyright (c)2011, by BUPT
* All rights reserved.
*      The copyright notice above does not evidence any
*      actual or intended publication of such source code

* FileName:    taskselector.cpp $
* System:       xframe
* SubSystem:    common
* Author:       zhang zhixiang
* Date：        2011.4.14
* Description:
		用于管理task，包括task的加载、注册、消息分发处理等。

****************************************************************/

#include "taskselector.h"
#include "abstracttask.h"


TTaskSelector::TTaskSelector(TFRAMEControl* fc):mfc(fc)
{
}
TTaskSelector::~TTaskSelector()
{
	//关闭的时候，哈希表中保存的container指针由framemng自己释放，这里不要删除，否则会出问题
	mContainerList.clear();

	TaskState* ts=NULL;
	mTaskList.reset();
	while(mTaskList.getNext(ts))
	{
		if(ts->inst)
		{
			InstState* is=NULL;
			ts->inst->reset();
			while(ts->inst->next(is))
			{
				delete is;
				is=NULL;
			}
			ts->inst->clear();
			delete ts;
			ts=NULL;
		}		
	}	
	mTaskList.clear();
	
	mIntDialog.clear();
	mStrDialog.clear();
}

void TTaskSelector::init(UINT appid)
{
	mAppID=appid;
	
	mContainerList.clear();
	mTaskList.clear();
	mIntDialog.clear();
	mStrDialog.clear();
	
	activeTaskCounter=0;
	activeInstCounter=0;
	registeredTaskAmount=0;
	registeredInstAmount=0;
	
	sendMsgToTaskCount=0; //发送到Task的消息个数？用途 by ZZQ
	multicastMsgCount=0; //多波消息数量？用途？by ZZQ
	broadcastMsgCount =0;
}


void TTaskSelector::setRecycleTime(int ct) 
{ 
	mIntDialog.setLivingTime(ct); 
	mStrDialog.setLivingTime(ct);
}


BOOL TTaskSelector::registerContainer(UINT tcid, TTaskContainer* ptc)
{
	if(tcid==0 || ptc==NULL) return FALSE;
	if(mContainerList.containsKey(tcid)) return FALSE;
	TaskContainerState* ptcs=NULL;
	ptcs=new TaskContainerState(tcid, ptc);
	if(ptcs==NULL) return FALSE;
	//ptcs->tc->init(tcid, mAppID, mfc);	//初始化
	mContainerList.put(tcid, ptcs);
	registeredInstAmount++;
	return TRUE;
}

BOOL TTaskSelector::registerTask(UINT taskid, UINT tcid, TAbstractTask* task)
{
	if(taskid<=0 || tcid<=0 || task==NULL) return FALSE;

	//将新增加的task添加到Container中
	TaskContainerState* ptcs=NULL;
	if(!mContainerList.get(tcid, ptcs)) return FALSE;
	if(ptcs==NULL) return FALSE;
	if(!ptcs->tc->registerTask(taskid, task)) return FALSE;

	//将新增加的task添加到Task列表中
	InstState* is=NULL;
	is=new InstState(tcid, ptcs);
	if(is==NULL) return FALSE;
	if(!mTaskList.containsKey(taskid))
	{
		CList<InstState*>* il=NULL;
		il=new CList<InstState*>();
		if(il==NULL) return FALSE;
		il->push_back(is);
		TaskState* ts=NULL;
		ts=new TaskState(taskid, il);
		if(ts==NULL) return FALSE;	
		mTaskList.put(taskid, ts);
		registeredTaskAmount++;			
	}
	else
	{
		TaskState* ts=NULL;
		if(mTaskList.get(taskid,ts))
		{
			if(ts!=NULL)
			{
				int lsize=0;
				lsize=ts->inst->push_back(is);
				ts->balanceCount=lsize*BalanceCountNumber;					
			}
			else return FALSE;
		}
		else
		return FALSE;
	}

	return TRUE;	
}		

/*
//todo: 以后增加
void TTaskSelector::shutdownTContainer(UINT tcid)
{
	TaskContainerState* is=NULL;
	if(mContainerList.get(tcid,is))
	{
		if(is!=NULL)
		{
			is->shuttingDown=true;
		}
	}
}


void TTaskSelector::shutdownTask(UINT taskid)
{
	TaskState* ts=NULL;
	InstState* is=NULL;
	if(mTaskList.get(taskid,ts))
	{
		if(ts!=NULL)
		{
			for(CList<InstState*>::iterator iter=ts->inst->begin(); iter!=ts->inst->end(); iter++)
			{
				(*iter)->shuttingDown=true;
			}
		}
	}
}

void TTaskSelector::unregisterInst(UINT instid)
{
	InstState* is=NULL;
	TaskState* ts=NULL;
	UINT taskid=0;

	if(mContainerList.get(instid,is))
	{
		if(is!=NULL)
		{
			taskid=is->taskid;
			if(mTaskList.get(taskid, ts))
			{
				if(ts!=NULL)
				{
					ts->inst->remove(is);
					ts->balanceCount=ts->inst->size()*BalanceCountNumber;
				}
			}
			mContainerList.remove(instid);
			registeredInstAmount--;
			delete is;
			is=NULL;
		}
	}
}

void TTaskSelector::unregisterTask(UINT taskid)
{
	UINT instid=0;
	TaskState* ts=NULL;
	InstState* is=NULL;

	if(mTaskList.get(taskid, ts))
	{
		if(ts!=NULL)
		{
			
			for(CList<InstState*>::iterator iter=ts->inst->begin(); iter!=ts->inst->end(); iter++)
			{
				is=(*iter);
				if(is!=NULL)
				{
					mContainerList.remove(is->instid);
					delete is;
					is=NULL;
					registeredInstAmount--;
				}
			}
			mTaskList.remove(taskid);
			delete ts;
			ts=NULL;
			registeredTaskAmount--;
		}
	}
}
*/

TTaskSelector::TaskContainerState* TTaskSelector::getTask(UINT taskid, UINT tcid)
{
	TaskState* ts=NULL;
	InstState* is=NULL;
	TaskContainerState* tcs=NULL;

	if(taskid>0 && tcid>0 && mContainerList.get(tcid, tcs))
	{
		return tcs; 
	}
	if(taskid>0 && tcid==0 && mTaskList.get(taskid,ts))
	{
		if(ts==NULL)
		{
			UniINFO("TaskSelector: taskid error, taskid=%d", taskid);
			return NULL;
		}
		if(ts->inst->count() <= 0)
		{
			UniINFO("TaskSelector: task has no inst container, taskid=%d", taskid);
		}
		else if(ts->inst->count()==1)
		{
			//only one inst, return it
			if(ts->inst->front(is))
				return is->tcs;
			else
				UniINFO("TaskSelector: the task has a container, but get container error, taskid=%d", taskid);

		}
		else if(ts->inst->reset_pos(ts->instpos))	//当前指向节点
		{
			for(int i=0; i<ts->inst->count(); i++)
			{
				if(ts->inst->next_d(is) && is!=NULL)
				{
					ts->instpos++;	//如果选择task ，则采用负荷分担方式，instpos 记录了最新的inst 选择
					if(ts->instpos>=ts->inst->count()) ts->instpos=0;

					if(!is->weight && !is->shuttingDown) //未被阻塞，可分担
					{
						//负荷分担的时候，标记为shutdown 的实例就不选择了
						ts->count++;
						is->count++;
						if(ts->count > ts->balanceCount)
						{
							balanceCheck(ts);
							ts->count=0;
						}
						return is->tcs;
					}
				}
			}
		}
	}
	return NULL;
}



void TTaskSelector::balanceCheck(TaskState* ts)
{
	InstState*  is=NULL;
	UINT	sum=0;
	if(ts)
	{
		//计算所有inst当前的等待队列长度
		for(CList<InstState*>::iterator iter=ts->inst->begin(); iter!=ts->inst->end(); iter++)
		{	
			is=(*iter);
			if(is!=NULL)
			{
				is->tcs->waitCount=is->tcs->tc->waitCount();
				sum+=is->tcs->waitCount;
			}
		}
		if(ts->inst->size()==1) return;
		//根据当前排队状态，重新设置是否分发的权重
		sum=sum/ts->inst->size();
		for(CList<InstState*>::iterator iter2=ts->inst->begin(); iter2!=ts->inst->end(); iter2++)
		{
			is=(*iter2);
			if(is!=NULL  && is->tcs->waitCount > (sum+BalanceCountNumber)) is->weight=1;	//如果等待队列大于队列长度均值+阈值，则不分发
			else is->weight=0;			
		}
	}
}

BOOL TTaskSelector::addEvent(TEventMsg* msg)
{
	BOOL found=FALSE;
	if(msg->instID>0)
	{
		TaskContainerState* inst=NULL;
		if(mContainerList.get(msg->instID, inst))
		{
			if(inst) //找到了inst
			{
				if(inst->tc)
				{
					inst->tc->add(msg);
					return TRUE;
				}
			}
		}
	}
	else if(msg->taskID>0)
	{
		TaskState* task=NULL;
		if(mTaskList.get(msg->taskID, task))
		{
			if(task)	//找到了task，再看是不是需要遍历
			{
				TEventMsg* newMsg;
				
				//找具体的inst
				for(CList<InstState*>::iterator iter=task->inst->begin(); iter!=task->inst->end(); iter++)
				{
					if((msg->instID==0 || (*iter)->tcid == msg->instID) && (*iter)->tcs!=NULL)
					{
						newMsg=NULL;
						newMsg=(TEventMsg*)msg->clone();
						if(newMsg)
						{
							newMsg->taskID=msg->taskID;
							newMsg->instID=(*iter)->tcid;
							(*iter)->tcs->tc->add(newMsg);
							found=TRUE;
						}						
					}
				}				
			}
		}
	}
	else
	{
		//taskid,instid都为0，广播
		mContainerList.reset();
		TaskContainerState* tcs=NULL;
		TEventMsg* newMsg;
		while(mContainerList.getNext(tcs))
		{
			if(tcs)
			{
				newMsg=NULL;
				newMsg=(TEventMsg*)msg->clone();
				if(newMsg)
				{
					tcs->tc->add(newMsg);
					found=TRUE;
				}
			}
		}
	}
	
	if(found)
	{
		if(msg) delete msg;
		msg=NULL;
		return TRUE;
	}
	else
	{
		msg->status=2;	//失败
		msg->eventInfo="taskid or instid not found!";
		mfc->post(msg);
		return FALSE;
	}
}

BOOL TTaskSelector::add(TUniNetMsg* msg)
{
	switch(msg->msgType)
	{
	case DIALOG_BROADCAST:
		return broadcastMsgToTask(msg);
		break;
	case DIALOG_MULTICAST:
		return multicastMsgToTask(msg);
		break;
	default:
		return sendMsgToTask(msg);
	}
	return TRUE;
}


BOOL TTaskSelector:: add(std::auto_ptr<TUniNetMsg> msg)
{
	TUniNetMsg* pUniNetMsg = msg.release();
	if(pUniNetMsg) return add(pUniNetMsg);
	return FALSE;
}

BOOL TTaskSelector::sendMsgToTask(TUniNetMsg* msg)
{
	if(msg==NULL) return FALSE;
	if(!mTaskList.containsKey(msg->tAddr.logAddr)) //Task没被注册
	{
		return FALSE;
	}

	UINT taskid=msg->tAddr.logAddr;
	UINT hostid=0;
	UINT containerid=0;
	if(msg->tAddr.phyAddr>0)
	{
		//消息中携带具体的实例ID，则意味着消息对端知道具体目标，该消息应该不是创建对话消息，不受shutdown的影响
		//消息中未携带具体的实例ID，则需要负荷分担
		parsePhyAddr(msg->tAddr.phyAddr, hostid, containerid);
		msg->tAddr.phyAddr=containerid;			//删掉了hostid，只留下了containerid
	}
	
	
	//add by LJL 20160714 添加对Session的处理------------------------------------------------------------
	if(msg->dialogType!=DIALOG_BEGIN && containerid==0 && msg->hasCtrlMsgHdr() && msg->ctrlMsgHdr->hasSessionId())
	{
		//如果是对话内消息，且目标instid=0，且有控制消息头，且控制消息头中设置了sessionid，即需要先判断一下是不是存在
		UINT sid=0;
		UINT instid=0;
		TaskKey tk;
		sid=msg->ctrlMsgHdr->getSessionId();
		if(sid>0)
		{
			//sid是整数
			if(mIntDialog.find(sid, tk))
			{
				//如果找到映射，则更新msg消息头
				if(tk.tcid>0 && tk.tiid>0)
				{
					msg->tAddr.phyAddr=tk.tcid;			//删掉了hostid，只留下了containerid
					msg->tAddr.taskInstID=tk.tiid;
					mIntDialog.update(sid, mCurrentTime);
				}	
			}
		}
		else
		{
			//sid是字符串
			CStr ssid;
			if(msg->ctrlMsgHdr->getSessionId(ssid))
			{
				//如果找到映射，则更新msg消息头
				if(!ssid.empty())
				{
					if(mStrDialog.find(ssid, tk))
					{
						//如果找到映射，则更新msg消息头
						if(tk.tcid>0 && tk.tiid>0)
						{
							msg->tAddr.phyAddr=tk.tcid;			//删掉了hostid，只留下了containerid
							msg->tAddr.taskInstID=tk.tiid;
							mStrDialog.update(ssid, mCurrentTime);
						}	
					}
				}					
			}
		}
		//清理超时的buffer
		mIntDialog.pop_old(mCurrentTime);
		mStrDialog.pop_old(mCurrentTime);
	}
	//---------------------------------------------------------------------------------------------------
	
	TaskContainerState* item=NULL; 
	//消息中携带具体的实例ID，则意味着消息对端知道具体目标，该消息应该不是创建对话消息，不受shutdown的影响
	//消息中未携带具体的实例ID，则需要负荷分担
	
	item=getTask(taskid, containerid);
	if(item)
	{
		//如果是创建实例消息，则需要指定的container预先创建处理对象
		if(msg->dialogType==DIALOG_BEGIN && msg->tAddr.taskInstID==0)
		{
			//如果是创建任务消息，则调用genTask
			UINT taskinstid=0;
			taskinstid=item->tc->genTaskInst(taskid);
			msg->tAddr.taskInstID=taskinstid;
			if(!containerid) msg->tAddr.phyAddr=item->tcid;
			
			//add by LJL 20160714 如果有sessionid，则需要保存一下 ------------------------------------------------
			if(msg->hasCtrlMsgHdr() && msg->ctrlMsgHdr->hasSessionId())
			{
				TaskKey tk;
				tk.tcid=item->tcid;
				tk.tiid=taskinstid;
				
				UINT sid=0;
				sid=msg->ctrlMsgHdr->getSessionId();
				if(sid>0)
				{
					//sid是整数
					mIntDialog.push(sid, tk);				
				}
				else
				{
					//sid是字符串
					CStr ssid;
					if(msg->ctrlMsgHdr->getSessionId(ssid))
					{
						//如果找到映射，则更新msg消息头
						if(!ssid.empty()) mStrDialog.push(ssid, tk);									
					}
				}
			}		
			//------------------------------------------------------------------------------------------------------
		}
		item->tc->add(msg);  //找到Task,只需调用Task的add 方法即可。。
		sendMsgToTaskCount++;
		return TRUE;
	}

	return FALSE;
}

//多播
BOOL TTaskSelector::multicastMsgToTask(TUniNetMsg* msg)
{
	if(msg==NULL) return FALSE;
	if(!mTaskList.containsKey(msg->tAddr.logAddr)) //Task没被注册
	{
		delete msg;
		return FALSE;
	}
	TaskState* ts=NULL;
	InstState* is=NULL;
	TUniNetMsg* newMsg=NULL;
	
	if(mTaskList.get(msg->tAddr.logAddr, ts))
	{
		for(CList<InstState*>::iterator iter=ts->inst->begin(); iter!=ts->inst->end(); iter++)
		{
			is=*iter;
			if(is!=NULL && is->tcid>0)
			{
				newMsg=NULL;
				newMsg=(TUniNetMsg*)msg->clone();
				if(newMsg)
				{
					newMsg->tAddr.phyAddr=is->tcid;
					is->tcs->tc->add(newMsg);
					is->count++;
				}
			}
		}
		multicastMsgCount++;
	}
	delete msg;
	return TRUE;
}

//广播
BOOL TTaskSelector::broadcastMsgToTask(TUniNetMsg* msg)
{
	if(msg==NULL) return FALSE;

	TUniNetMsg* newMsg;
	TaskContainerState* is=NULL;
	mContainerList.reset();
	while(mContainerList.getNext(is))
	{
		if(is)
		{
			newMsg=NULL;
			newMsg=(TUniNetMsg*)msg->clone();
			if(newMsg)
			{
				newMsg->tAddr.phyAddr=is->tcid;
				is->tc->add(newMsg);
			}
		}                                
	}
	broadcastMsgCount++;
	delete msg;
	return TRUE;
}



void TTaskSelector::getStatues(CStr& status)
{
	TaskState* ts=NULL;
	InstState* is=NULL;
	TaskContainerState* tcs=NULL;

	status<<"TaskInfo:"<<"(regtask="<<registeredTaskAmount<<",reginst="<<registeredInstAmount<<",procmsg("<<sendMsgToTaskCount<<"/"<<multicastMsgCount<<"/"<<broadcastMsgCount<<")\r\n";
	status<<"ContainerList:\r\n";
	mContainerList.reset();
	while(mContainerList.getNext(tcs))
	{
		if(tcs!=NULL)
		{
			status<<"  ContainerID="<<tcs->tcid<<"(waitCount="<<tcs->waitCount<<")\r\n";
		}
	}
	status<<"TaskList:\r\n";
	mTaskList.reset();
	while(mTaskList.getNext(ts))
	{
		if(ts!=NULL)
		{
			status<<" TaskID="<<ts->taskid<<"(balanceCount="<<ts->balanceCount<<",count="<<ts->count<<")\r\n";
			for(CList<InstState*>::iterator iter=ts->inst->begin(); iter!=ts->inst->end(); iter++)
			{
				is=*iter;
				if(is!=NULL) status<<"    InstID="<<is->tcid<<"(count="<<is->count<<",weight="<<is->weight<<")\r\n";
			}
		}
	}
}
