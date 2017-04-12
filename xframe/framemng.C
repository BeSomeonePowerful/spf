#include <map>
#include "framemng.h"
#include "framectrl.h"
#include "kernalthread.h"
#include "taskthread.h"
#include "psathread.h"
#include "taskcontainer.h"
#include "psacontainer.h"
#include "taskselector.h"

TFRAMEManager* TFRAMEManager::_instance;
TFRAMEManager::TFRAMEManager()
{

	mFifo=NULL;
	//创建内部控制消息收发队列
	mFifo = new Fifo<TMsg>();

	//创建framectrl
	framectrl=NULL;
	kernalThread=NULL;
	framectrl= new TFRAMEControl(*mFifo);
	kernalThread =new TKernalThread(*framectrl);

	mTTQ = new TaskTimerQueue(*mFifo);

	mTaskObj.clear();
	mInstList.clear();

	mSCIP="127.0.0.1";
	mSCPort=9000;
	mSCHB=10000;

	mTransID=0;

	shutdown=FALSE;

}


TFRAMEManager::~TFRAMEManager()
{
	if(kernalThread) delete kernalThread;
	if(framectrl) delete framectrl;

	for(std::map<UINT, TInstance*>::iterator iter=mInstList.begin();iter!=mInstList.end();iter++)
	{
		if(iter->second->mInstThread) delete iter->second->mInstThread;
		if(iter->second->mInstContainer) delete iter->second->mInstContainer;
	}
	mInstList.clear();
	
	for(std::map<UINT, TAbstractTask*>::iterator iter=mTaskObj.begin();iter!=mTaskObj.end();iter++)
	{
		if(iter->second) delete iter->second;
	}
	mTaskObj.clear();
	
	socket.closeSocket();
}


void TFRAMEManager::registerTask(UINT taskID, TAbstractTask* task)
{
	if(taskID>0 && task!=NULL) mTaskObj.insert(std::pair<UINT, TAbstractTask*>(taskID, task));
}

void TFRAMEManager::Init(UINT appID, char* appName)
{
	TKernalEnv  env;
	env.load();

	mLogType=env.getLogType();
	mLogLevel=env.getLogLevel();
	mAppID=env.getHostID();
	if(appID>0) mAppID=appID;	//以应用传输进来的ID 为准
	mAppName=env.getAppName();
	if(appName) mAppName=appName;	//以应用传输进来的名字 为准

	env.getSCInfo(mSCIP, mSCPort, mSCHB);
	env.getProcList(mProcList);
	env.getTimerRsc(mTME);

	CStr kernal_env;
	env.list(kernal_env);
	UniINFO("FrameMng: Kernal env: \n %s ", kernal_env.c_str());

	framectrl->init(mAppID, mLogType, mLogLevel,env.getRecycleTime());

	TInstance*		ins=NULL;
	TTaskContainer* tc=NULL;
	TTaskThread*	tt=NULL;
	TPSAContainer* 	pc=NULL;
	TPSAThread*		pt=NULL;
	TAbstractTask*	obj=NULL;
	TAbstractTask*	task=NULL;
	
	UINT mContainerID=1;
	mAppInfo<<"\r\n"<<mAppName<<"(AppID/HostID="<<mAppID<<", FrameMngID="<<__FRAMEMNG_TASKID<<"/FrameCtrlID="<<__FRAMECONTROL_TASKID<<"/TaskSelectorID="<<__TASKSELECTOR_TASKID<<")";
	
	/*
	CallServer(AppID/HostID=1, FrameMngID=1/FrameCtrlID=2/TaskSelectorID=3)
		Proc：Proxy(Type=1,ThreadNum=3)
		Task Proxytask(taskid=1001, tasktype=2, taskdll=proxytask.so)
		Task Registertask(taskid=1002, tasktype=2, taskdll=registertask.so)
		----------
		InstID=1, 2, 3
		
	*/
	
	for(CList<TProcRsc>::iterator iter=mProcList.begin(); iter!=mProcList.end(); iter++ )
	{
		mAppInfo<<"\r\n ----------\r\n Proc："<<(*iter).instName<<"(Type="<<(*iter).instType<<", ThreadNum="<<(*iter).threadNum<<")\r\n";
		for(CList<TTaskRsc>::iterator itertask=(*iter).taskList.begin(); itertask!=(*iter).taskList.end(); itertask++ )	//按照启动队列中设置的task数量初始化任务
		{
			//ToDo:以后的自动加载，应该写在这里
			std::map<UINT, TAbstractTask*>::iterator  it;
			it=mTaskObj.find((*itertask).taskID);
			if(it!=mTaskObj.end())
			{
				obj=it->second;
				if(obj!=NULL)	//确认任务列表中的对象应该是已经注册的对象
				{
					mAppInfo<<"   Task "<<(*itertask).taskName<<"(taskid="<<(*itertask).taskID<<", tasktype="<< (*itertask).taskType<<", taskdll="<<(*itertask).taskDll<<")\r\n";
				}
			}
		}

		mAppInfo<<"   InstID=";
		for(int i=0;i<(*iter).threadNum;i++)	//按照启动队列中设置的线程数量初始化线程
		{
			ins=NULL;
			ins=new TInstance();
			if(!ins)
			{
				UniERROR("FrameMng: new task container instance error!");
				exit(0);
			}
			if((*iter).instType>=objtype_Task && (*iter).instType<objtype_Psa )
			{
				ins->mInstID=mContainerID;
				ins->mInstType=objtype_Task;
				ins->mInstName=(*iter).instName.c_str();

				tc=NULL;
				tc=new TTaskContainer();
				if(!tc)
				{
					UniERROR("FrameMng: new task container error!");
					exit(0);
				}
				UniINFO("FrameMng: init container, containerid=%d, hostid=%d", mContainerID, mAppID);
				tc->init(mContainerID, mAppID, framectrl);
				ins->mInstContainer=tc;
				
				if(!framectrl->getTaskSelector()->registerContainer(mContainerID, tc))	//注册到taskselector
				{
					UniERROR("FrameMng: reg task container %s:%d error!",ins->mInstName.c_str(), ins->mInstID);
					exit(0);
				}
				
				tt=NULL;
				tt=new TTaskThread(*tc, (*iter).instName.c_str(),mLogType, mLogLevel, mContainerID);	//初始化的时候，每一个线程的日志类型与等级与kernal设置保持一致，启动后如需调整，可发送控制指令
				if(!tt)
				{
					UniERROR("FrameMng: new task container thread ERROR!");
					exit(0);
				}
				
				ins->mInstThread=tt;

				mAppInfo<<ins->mInstID<<",";
				
			}
			else if((*iter).instType>=objtype_Psa && (*iter).instType<objtype_Extended)
			{
				ins->mInstID=mContainerID;
				ins->mInstType=objtype_Psa;
				ins->mInstName=(*iter).instName.c_str();

				pc=NULL;
				pc=new TPSAContainer();
				if(!pc)
				{
					UniERROR("FrameMng: new psa container error!");
					exit(0);
				}
				UniINFO("FrameMng: init container, containerid=%d, hostid=%d", mContainerID, mAppID);
				pc->init(mContainerID, mAppID, framectrl);
				ins->mInstContainer=pc;
				
				if(!framectrl->getTaskSelector()->registerContainer(mContainerID, pc))	//注册到taskselector
				{
					UniERROR("FrameMng: reg task container %s:%d error!",ins->mInstName.c_str(), ins->mInstID);
					exit(0);
				}
				
				pt=NULL;
				pt=new TPSAThread(*pc, (*iter).instName.c_str(),mLogType, mLogLevel, mContainerID);	//初始化的时候，每一个线程的日志类型与等级与kernal设置保持一致，启动后如需调整，可发送控制指令
				if(!pt)
				{
					UniERROR("FrameMng: new psa container thread ERROR!");
					exit(0);
				}
				
				ins->mInstThread=pt;
				
				mAppInfo<<ins->mInstID<<",";
				
			}
			else
			{
				UniERROR("FrameMng: invalid container type!")
				exit(0);
			}
			
			std::pair<std::map<UINT, TInstance*>::iterator,bool> ret;
			ret=mInstList.insert(std::pair<UINT, TInstance*>(mContainerID, ins));
			if (ret.second==false) 
			{
				UniERROR("FrameMng: insert container inst %s(%d) error!", ins->mInstName.c_str(), ins->mInstID);
				exit(0);
			}

			//按照启动队列中设置的task初始化container
			for(CList<TTaskRsc>::iterator itertask=(*iter).taskList.begin(); itertask!=(*iter).taskList.end(); itertask++ )	
			{
				std::map<UINT, TAbstractTask*>::iterator  it;
				it=mTaskObj.find((*itertask).taskID);
				if(it!=mTaskObj.end())
				{
					obj=it->second;
					if(obj!=NULL)
					{
						task=obj->clone();	//通过clone 获得复制的task 指针
						UniINFO("FrameMng: init task taskid=%d, hostid=%d, containerid=%d",(*itertask).taskID, mAppID, mContainerID); 
						//task->init(mAppID, mContainerID, (*itertask).taskID, ins->mInstContainer); //初始化task
						if(!framectrl->getTaskSelector()->registerTask((*itertask).taskID, mContainerID, task))
						{
							UniERROR("FrameMng: register task %d to container %s:%d error!", (*itertask).taskID, ins->mInstName.c_str(), ins->mInstID);
							exit(0);
						}
					}
				}
			}
			
			mContainerID++;	//实例ID+1
			if(mContainerID>__MAX_TASKCONTAINERID)
			{
				UniERROR("FrameMng: register container over flow! must lower then %d", __MAX_TASKCONTAINERID);
				exit(0);
			}
		}
	}

	UniINFO("FrameMng: Init task list ok!\r\n");
	UniINFO("%s",mAppInfo.c_str());

	//init socket
	BOOL success = socket.openServer(NULL, mSCPort);	//使用scPORT监听所有地址上的端口
	if(!success)
	{
		UniERROR("FrameMng: Bind command server error! port=%d", mSCPort);
	}
	else
	{
		UniINFO("FrameMng: Bind command server! port=%d", mSCPort);
	}

}

TFRAMEManager * TFRAMEManager::instance()
{
	if(!_instance)
	{
		_instance = new TFRAMEManager();
	}
	return _instance;
}

void TFRAMEManager::Run()
{
	TEventMsg* msg=NULL;

	UniINFO("FrameMng: Starting ......");


	//先启动framectrl
	kernalThread->run();
	kernalThread->detach();

	for(std::map<UINT, TInstance*>::iterator iter=mInstList.begin(); iter!=mInstList.end(); iter++ )
	{
		if(iter->second!=NULL)
		{
			iter->second->mInstThread->run();
			iter->second->mInstThread->detach();
		}
	}

	//将初始化消息先推送到各个实例
	mTransID++;
	msg=new TEventMsg();
	msg->eventID=KERNAL_APP_INIT;
	msg->transID=mTransID;
	msg->status=0;
	msg->sender.taskType=0;
	msg->sender.taskID=0;
	msg->sender.instID=0;
	framectrl->add(msg);

	//延迟1秒后，再按顺序启动各个任务
	sleep(1);
	
	UniINFO("FrameMng: ****************  Start OK!  **************************")

	char mSockBuffer[maxLen];

	CCode code;
	code.content = NULL;
	while(!shutdown)
	{

		UINT64 nextstep = mTTQ->process();
		if(nextstep>1000) nextstep=1000;	//定时器返回的下一个定时器时间时间间隔，最长不超过1秒

		std::auto_ptr<TMsg> msg=std::auto_ptr<TMsg>(mFifo->getNext(nextstep));	//在消息队列最长等待时间是定时器下一个超时时间间隔
		if(msg.get())
		{
			TEventMsg* pEventMsg = NULL;
			pEventMsg=dynamic_cast<TEventMsg*>(msg.get());
			if(pEventMsg)
			{
				CCode scode;
				if(pEventMsg->status == 0) //status==0 means request, 网络发给应用的消息
				{
					CStr scmd[5];
					pEventMsg->eventInfo.split(scmd,5);
					if(pEventMsg->eventID == KERNAL_APP_STATUES && scmd[0]=="sclist")
					{
						scode.content = (char *)mAppInfo.c_str();
						scode.length = strlen(scode.content);
						socket.sendCode(scode);
						UniINFO("\r\nmng command:sclist %s", mAppInfo.c_str());
					}
					else
					{
						UniDEBUG("FrameMng: recv eventmsg eventid=%d, taskid=%d, instid=%d, evinfo=%s", pEventMsg->eventID, pEventMsg->taskID, pEventMsg->instID, pEventMsg->eventInfo.c_str());
						//event消息的分发也由framectrl执行
						msg.release();    //释放消息自动指针
						framectrl->add(pEventMsg);
					}
				} 
				else if(pEventMsg->status>0)  // status>0 means response，send to network
				{
					CStr fstr;
					switch(pEventMsg->eventID)
					{
					case KERNAL_APP_INIT:
						if(pEventMsg->status==1)
						{
							UniINFO("FrameMng: init task ok: taskid=%d, instid=%d",pEventMsg->sender.taskID,  pEventMsg->sender.instID);
						}
						else if(pEventMsg->status==2)
						{
							UniERROR("FrameMng: init task error: taskid=%d, instid=%d",pEventMsg->sender.taskID,  pEventMsg->sender.instID);
						}
						break;
					case KERNAL_APP_HEATBEAT:
						//ToDo: 心跳结果的处理
						break;
					case KERNAL_APP_RELOAD:
						if(pEventMsg->status == 1)
						{
							fstr.fCat("taskid: %d, instid: %d env reload ok", pEventMsg->sender.taskID, pEventMsg->sender.instID);
						}
						else
						{
							fstr.fCat("taskid: %d, instid: %d env reload failed", pEventMsg->sender.taskID, pEventMsg->sender.instID);
						}
						fstr<<pEventMsg->eventInfo;
						scode.content = fstr.c_str();
						scode.length = fstr.length();
						socket.sendCode(scode);
						UniDEBUG("FrameMng: send event msg rsp: %s",scode.content);
						break;
					case KERNAL_APP_SHUTDOWN:
						//ToDo: 关闭
						break;
					case KERNAL_APP_STATUES:
						scode.content = pEventMsg->eventInfo.c_str();
						scode.length = strlen(scode.content);
						socket.sendCode(scode);
						UniDEBUG("FrameMng: send status of taskid:%d, instid:%d, %s", pEventMsg->sender.taskID, pEventMsg->sender.instID, scode.content);
						break;
					default:
						break;
					}
				}
			}
		}

		//check if recieved new command
		memset(mSockBuffer, 0, sizeof(mSockBuffer));
		code.content = mSockBuffer;
		code.length = 0;
		int recvstatus = socket.recvCode(code);
		if(recvstatus == 1 &&code.length >0)
		{
			Command cmd;
			char *token;
			const char *delim=" \t";
			vector<string> str;
			for(token = strsep(&code.content, delim); token != NULL; token = strsep(&code.content, delim))
			{
				if(strcmp(token,""))
				{
					str.push_back(token);
				}
			}
			cmd.type=atoi(str[0].c_str());
			cmd.taskId=atoi(str[1].c_str());
			cmd.instId=atoi(str[2].c_str());
			string string;
			for(int o=3;o<str.size();o++)	//字符串拆分之后，再用“ ”重新拼装
			{
				string = string + str[o];
				if(o!= (str.size()-1))
				string += " ";
			}

			TEventMsg* cmdmsg=NULL;
			cmdmsg=new TEventMsg();
			cmdmsg->sender.taskType=objtype_Kernal;
			cmdmsg->sender.taskID=0;
			cmdmsg->sender.instID=0;

			mTransID++;
			cmdmsg->transID=mTransID;
			cmdmsg->status=0;
			cmdmsg->eventInfo += string.c_str();
			switch(cmd.type)
			{
			case 0:
				cmdmsg->eventID= KERNAL_APP_RELOAD;
				break;
			case 1:
			default:
				cmdmsg->eventID= KERNAL_APP_STATUES;
				break;
			}
			cmdmsg->taskID=cmd.taskId;
			cmdmsg->instID=cmd.instId;
			UniDEBUG("FrameMng: recv eventmsg from net, eventid=%d, status=%d, taskid=%d, instid=%d, evinfo=%s", cmdmsg->eventID, cmdmsg->status, cmdmsg->taskID, cmdmsg->instID, cmdmsg->eventInfo.c_str());
			mFifo->add(cmdmsg);
		}
	}
}
