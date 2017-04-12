#include "abstracttask.h"
#include "transport.h"
#include "info.h"

TTransMsg::TTransMsg() 
{ 
	split[0]=0; 
	split[1]=0xFF; 
	split[2]=0; 
	cseq=0;
	length=0; 
}

void TTransMsg::encode(CHAR* &buf)
{
	memcpy( buf, split, 3 );
	buf+=3;
	ENCODE_INT(buf, cseq);
	transid.encode(buf);
	ENCODE_INT(buf, length);
}

void TTransMsg::decode(CHAR* &buf)
{
	memcpy(split, buf, 3);
	buf+=3;
	DECODE_INT(cseq, buf);
	transid.decode(buf);
	DECODE_INT(length, buf);
}

int TTransMsg::size()
{
	return 3+sizeof(INT)+transid.size()+sizeof(INT);
}

BOOL TTransMsg::check()
{
    if(split[0]==0 && ((0xFF) ^ split[1]) != 0 /* split[1] == 0xFF */ && split[2]==0 && length>0)
    {
        return TRUE;
    }
	else
    {
        return FALSE;
    }
}
	
TTransport::TTransport(TAbstractTask* parent, TMsgAddress &addr):mParent(parent),mParentAddr(addr)
{
	mSelectMS.tv_sec = 0; 
	mSelectMS.tv_usec = 5000;
		
	mFifo=new Fifo<TMsg>();
	shutdown=FALSE;
}
	
TTransport::~TTransport()
{
	if(mFifo!=NULL) delete mFifo;
}

void TTransport::setMilliSeconds(unsigned long ms)	//在socket阻塞时间，如果不设置，默认为5ms
{ 
	mSelectMS.tv_sec = (ms/1000);
	mSelectMS.tv_usec = (ms%1000)*1000;
}

void  TTransport::postMsg(TUniNetMsg* msg)
{
	//将收到的消息发送给parent
	//源端将全部设置为0
	msg->sender.taskID=0;
	msg->sender.instID=0;
	msg->sender.taskType=0;
	msg->oAddr.logAddr=0;
	msg->oAddr.phyAddr=0;
	msg->oAddr.taskInstID=0;
	msg->tAddr=mParentAddr;
	mParent->sendMsgSelf(msg);
}

void  TTransport::postEvent(TEventMsg* msg)
{
	msg->sender.taskID=0;
	msg->sender.instID=0;
	msg->sender.taskType=0;
	mParent->sendMsgSelf(msg);
}

BOOL TTransport::process()
{
	//如果已经设置为关闭，且发送缓冲已经清空，则向父进程发送确认，并返回
	if(shutdown)
	{
		postEvent(shutdownmsg);
		return FALSE;
	}
	
	//一次轮询20个消息（数量可以修改）
	int loop=20;
	while(loop && mFifo->messageAvailable())
	{
		--loop;
		TMsg* msg=mFifo->getNext(0);
		if(msg)
		{
			TUniNetMsg* pUniNetMsg = NULL;
			pUniNetMsg=dynamic_cast<TUniNetMsg*>(msg);
			if(pUniNetMsg)
			{
				onRecvSendMsg(pUniNetMsg);
				continue;
			}
			TEventMsg* pEventMsg = NULL;
			pEventMsg=dynamic_cast<TEventMsg*>(msg);
			if(pEventMsg)
			{
				switch(pEventMsg->eventID)
				{
					case KERNAL_APP_RELOAD:
						pEventMsg->eventInfo="transport reload";
						postEvent(pEventMsg);
						break;
					case KERNAL_APP_SHUTDOWN:
						shutdown=TRUE;
						pEventMsg->eventInfo="transport shutdown";
						shutdownmsg=pEventMsg;
						break;
					default:
						delete pEventMsg;
						break;
				}
			}			
		}
	}
	
	return TRUE;
}
