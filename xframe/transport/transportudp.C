#include <stdlib.h>
#include "transportudp.h"
#include "msgdef_com.h"

TUDPTrasnport::TUDPTrasnport(TAbstractTask* parent, TMsgAddress &addr):TTransport(parent, addr)
{
	mSockBuffer.content=new char[SocketBufferSize];
	if(mSockBuffer.content==NULL) 
	{
		UniERROR("allocate socket buffer error!")
	}
}

TUDPTrasnport::~TUDPTrasnport()
{
	if(mSockBuffer.content!=NULL)
		delete mSockBuffer.content;
}
 
BOOL TUDPTrasnport::addTransport(int bindid, const char* host, int port)
{
	TBind* bind=new TBind(bindid, host, port);
	if(bind)
	{
		BOOL success = bind->socket().openServer(host, port);
    	if(!success)
    	{
        	UniERROR("bind server error! host=%s, port=%d",host, port);
    	}
    	else
    	{
    		UniINFO("bind server! host=%s, port=%d",host, port);
			mBinds.insert ( std::pair<UINT,TBind*>(bindid, bind) );
			return TRUE;
    	}	
	}

	return FALSE;
}

void TUDPTrasnport::onBuildFDSet(CFdSet& fdset)
{
	for(std::map<UINT, TBind*>::iterator iter = mBinds.begin(); iter != mBinds.end(); ++iter)
	{
		fdset.setRead(iter->second->socket().getFd());
		fdset.setWrite(iter->second->socket().getFd());
		fdset.setExcept(iter->second->socket().getFd());
	}
}	

void TUDPTrasnport::onCheckFDSet(CFdSet& fdset)		//判断是不是可以发收消息 
{
	for(std::map<UINT, TBind*>::iterator iter = mBinds.begin(); iter != mBinds.end(); ++iter)
	{
		if(fdset.readyToRead(iter->second->socket().getFd()))
		{
			Recv(iter->second);
		}
		if(fdset.readyToWrite(iter->second->socket().getFd()))
		{
			Send(iter->second);
		}
		if(fdset.hasException(iter->second->socket().getFd()))
		{
			UniERROR("Exception!");
		}
	}
}
		
void TUDPTrasnport::onRecvSendMsg(TUniNetMsg* msg)
{
	if(msg)
	{
		TComMsgHdr* hdr=NULL;
		hdr=dynamic_cast<TComMsgHdr*>(msg->getCtrlMsgHdr());
		if(hdr!=NULL)
		{
			TBind* bind=NULL;
			int bindid=hdr->facility;
			if(bindid>0)
			{
				bind=mBinds[bindid];
				if(bind)
				{
					bind->mSendBuffer.push_back(msg);
				}
			}
			else
			{
				//如果没选择绑定地址，则使用默认地址，即绑定序号最小的那个地址
				bind=mBinds.begin()->second;
				if(bind)
				{
					bind->mSendBuffer.push_back(msg);
				}
			}
		}
	}
}

void TUDPTrasnport::Recv(TBind* bind)
{
	int loop=10;	//一次解除阻塞，最多循环读取10次
	while(loop)
	{
		mSockBuffer.clear();
		struct sockaddr_in client;
		int recvstatus = bind->socket().recvCode(mSockBuffer, client);  //从网络收消息
		if(recvstatus == 1 && mSockBuffer.length >0)
		{
			char* buffer=mSockBuffer.content;
			int   length=mSockBuffer.length;
			TTransMsg tm;
			tm.decode(buffer);
			if(!tm.check())
			{
				UniINFO("recv a illegal msg!");
				continue;
			}
			length-=tm.size();
			if(tm.length>length)
			{
				UniINFO("recv a uncomplete msg!");
				continue;
			}
			
			TUniNetMsg* unimsg=new TUniNetMsg();
			if(unimsg==NULL)
			{
				UniERROR("recv msg faile, out off memory!");
				break;
			}
			TComMsgHdr* hdr=new TComMsgHdr();
			if(hdr==NULL)
			{
				UniERROR("recv msg faile, out off memory!");
				delete unimsg;
				break;
			}
			TComMsgBody* body=new TComMsgBody();
			if(body==NULL)
			{
				UniERROR("recv msg faile, out off memory!");
				delete unimsg;
				delete hdr;
				break;	
			}
			
			unimsg->dialogType=DIALOG_MESSAGE;
			unimsg->msgName=COM_MESSAGE_REQ;
			unimsg->msgType=COM_MESSAGE_TYPE;
			if(tm.cseq) unimsg->setCSeq(tm.cseq);
			if(tm.transid.length()) unimsg->setTransId(tm.transid);
			hdr->status=0;		
			hdr->facility=bind->mBindID;
			hdr->argc=1;
			hdr->argv[0]=client;
            
			// body->info.nCat(buffer, length);
			//直接用body info进行解码
			body->info.decode(buffer);
			unimsg->setCtrlMsgHdr(hdr);
			unimsg->setMsgBody(body);
			
			postMsg(unimsg);
			
			mRecvCount++;			
		}
		else
		{
			break;
		}
                loop--;
	}
}

void TUDPTrasnport::Send(TBind* bind)
{
	while(bind->mSendBuffer.size())  //向网络发消息
	{
		TUniNetMsg* msg=NULL;
		bind->mSendBuffer.pop_front(msg);
		if(msg==NULL) continue;
		
		TComMsgHdr* hdr=NULL;
		hdr=dynamic_cast<TComMsgHdr*>(msg->getCtrlMsgHdr());
		TComMsgBody* body=NULL;
		body=dynamic_cast<TComMsgBody*>(msg->getMsgBody());
		if(hdr==NULL || body==NULL)
		{
			delete msg;
			continue;
		}
		
		mSockBuffer.clear();
		char* buffer=mSockBuffer.content;
		TTransMsg tm;
		tm.length=body->info.size();	//加入消息分节符和消息内容长度
		if(msg->hasCSeq()) tm.cseq=msg->getCSeq();
		if(msg->hasTransId()) tm.transid=msg->getTransId();
		tm.encode(buffer);
		body->info.encode(buffer);
		mSockBuffer.length=tm.size()+tm.length;
			
		// bind->socket().setRemoteSockAddr(hdr->argv[0].c_str());
		int rt=0;
        
                CStr _socket_addr_(hdr->argv[0].c_str());
                TURI _temp_domain_(_socket_addr_);
		rt=bind->socket().sendCode(mSockBuffer, _temp_domain_.host.c_str(), _temp_domain_.port);
                if(rt!=1)
		{
			UniINFO("send msg error! errorcode=%d", rt);
			mSendFaile++;
		}
		else
		{
			mSendCount++;
		}
		delete msg;
	}
}


