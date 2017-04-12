#include <stdlib.h>
#include "testpsatask.h"

TestPsaTask::TestPsaTask():TPSATask()
{
	localport=9000;
	remoteport=9001;
	linknum=0;
	clientnum=0;
}

TestPsaTask::~TestPsaTask()
{
}
 
BOOL TestPsaTask::onInit(TiXmlElement*	extend)
{
	TiXmlHandle handle(extend);
	TiXmlElement*	task=NULL;
	task=handle.FirstChild("task").Element();
	if(task)
	{
		if(!task->Attribute("localport", &localport)) localport=9000;
		if(!task->Attribute("remoteport", &remoteport)) remoteport=9001;
	}
	
	UniINFO("LocalPort=%d, RemotePort=%d...", localport,remoteport );

	//绑定服务端口
	if(!listener.open(NULL, localport))
    {
       	UniERROR("bind server error! port=%d",localport);
		return FALSE;
    }
    else
    {
    	UniINFO("bind server! port=%d",localport);
    }
	
	//状态输出定时器
	mTK1=setTimer(1);
	//客户端延迟一段时间建立连接
	mTK2=setTimer(2);

	return TRUE;
}


void TestPsaTask::procMsg(std::auto_ptr<TUniNetMsg> msg)
{
	TUniNetMsg* unimsg=msg.get();
	if(unimsg!=NULL)
	{
		if(unimsg->oAddr.logAddr==1001 && unimsg->msgName==COM_MESSAGE_REQ 
			&& unimsg->hasCtrlMsgHdr() && unimsg->hasMsgBody())
		{
			TComCtrlMsg* ctrl=(TComCtrlMsg*)(unimsg->ctrlMsgHdr);
			TComMsg* body=(TComMsg*)(unimsg->msgBody);
			CTestLink* link=NULL;
			int linkid=0;
			if(ctrl->status==0)	//如果没有指定linkid，则轮询link
			{
				mLinkList.next_d(linkid);
			}
			else
			{
				linkid=ctrl->status;
			}
			link=(CTestLink*)(links.findByLinkId(linkid));		//status代替linkid
			if(link!=NULL)
			{
				 link->sendToLink(body->comMsg);	//comMsg代替消息内容
			}
		}
	}	
}



void TestPsaTask::onTimeOut(TTimerKey timerKey, void* para)
{
	if(timerKey==mTK1)
	{
		CStr info;
		links.printState(info);
		UniDEBUG("%s", info.c_str());
		mTK1=setTimer(1);
	}
	if(timerKey==mTK2)
	{
		if(clientnum<3)
		{
			CTestLink* link=new CTestLink(this);
			if(link->connect(NULL, remoteport, NULL, 0))
			{
				linknum++;
				link->linkId()=linknum;			//设置链路id
				mLinkList.push_back(linknum);	//链路id，添加到内部保存的列表，准备做负荷分担用
				links.addConnection(link);		//添加链路对象到链路管理
				UniINFO("connect server! serverport=%d, linkid=%d",remoteport, link->linkId());
				clientnum++;
			}
			else
			{
				UniDEBUG("connect server error! serverport=%d",remoteport);
			}
			mTK2=setTimer(2);
		}
	}
}

void TestPsaTask::onBuildFDSet(CFdSet& fdset)
{
	listener.buildFdSet(fdset);
    links.buildFdSet(fdset);
}	

void TestPsaTask::onCheckFDSet(CFdSet& fdset)		//判断是不是可以发收消息 
{
	 if(listener.accept(fdset))
      {
         CTestLink* link = new CTestLink(this);
         listener.dupConnection(link);
		 linknum++;
         link->linkId() = linknum;	//设置链路id
         links.addConnection(link);	//添加链路对象到链路管理
		 mLinkList.push_back(linknum);//链路id，添加到内部保存的列表，准备做负荷分担用
		 UniINFO("accept client connect! serverport=%d, linkid=%d", localport, link->linkId());
      }
	  if(!links.procFdSet(fdset))
	  {
		//UniINFO("")  //
	  }
}
		
		
void TestPsaTask::sendToTask(int linkid, CStr& msg)
{		
	TUniNetMsg* unimsg=new TUniNetMsg();
	unimsg->tAddr.logAddr=1001;
	unimsg->dialogType=DIALOG_BEGIN;
	unimsg->msgName=COM_MESSAGE_REQ;
	TComCtrlMsg* ctrl=new TComCtrlMsg();
	ctrl->status=linkid;
	TComMsg* body=new TComMsg();
	body->comMsg=msg;
	unimsg->setCtrlMsgHdr(ctrl);
	unimsg->setMsgBody(body);
	sendMsg(unimsg);
}

CTestLink::CTestLink(TestPsaTask* task)
{
	mTask=task;
	mSendCount=0;
	mSendCountR=0;
	mRecvCount=0;
	mSendProc=0;
}

CTestLink::~CTestLink()
{
	
}

void CTestLink::sendToLink(CStr& msg)
{
	if(!mSendBuffer.push_back(msg))
	{
		UniERROR("add to send buffer error!");
	}
	UniDEBUG("send msg (%s) queue count=%d", msg.c_str(), mSendBuffer.count());
	mSendCount++;
}


int CTestLink::processRecv()
{
	CCode code;
	mRecvProc++;
    	code.content = NULL;
	char mSockBuffer[100];
	memset(mSockBuffer, 0, sizeof(mSockBuffer));
    	code.content = mSockBuffer;
    	code.length = 0;
    	int recvstatus = receive(code);  //从网络收消息
	//UniDEBUG("recv msg (%s), len=%d",code.content, code.length);
	if(recvstatus == 1 && code.length >0)
	{
		mRecvBuffer.nCat(mSockBuffer,code.length);		//先加入到缓冲区
		//UniDEBUG("recv buffer (%s), len=%d",mRecvBuffer.c_str(), mRecvBuffer.length());
		CStr result;
		do{
			result="";
			mRecvBuffer.cut(";",result);		//使用\0当定界符
			if(result.length())
			{
				//UniDEBUG("recv msg (%s), len=%d", result.c_str(), result.length());
				mTask->sendToTask(mLinkId, result);
				mRecvCount++;
			}
		}while(result.length());
		return 1;	
	}
	return 0;
}

void CTestLink::processSend()
{
	char mBuffer[100];
	int  i;
	CStr msg="";
//	UniDEBUG("Ready to send, buffer queue=%d", mSendBuffer.count());
	while(mSendBuffer.count())
	{
		mSendBuffer.pop_front(msg);  //向网络发消息
	
//		UniDEBUG("Ready to send:%s", msg.c_str());

			memset(mBuffer, 0, sizeof(mBuffer));
			msg<<";";
			memcpy(mBuffer,msg.c_str(),msg.length());
			CCode code;
			code.content=mBuffer;
			code.length = strlen(mBuffer);	//加一个"\0"做定界符
			send(code);
			mSendCountR++;
		msg="";
	}
	mSendProc++;
}

void CTestLink::printState(CStr& str)
{
	CTcpConnection::printState(str);
	str<<"LinkID="<<linkId()<<"; RecvCount="<<mRecvCount<<"/(proc="<<mRecvProc<<"); SendCount="<<mSendCount<<"; SendCountR="<<mSendCountR<<"/(proc="<<mSendProc<<")\n";
}

