#include <stdlib.h>
#include "transstate.h"
#include "msghelper_com.h"

TTransState::~TTransState()
{
	if(timer)
	{
		mTask->delResendTimer(timer);
	}
	if(msg)
	{
		delete msg;
	}
}

TS_CODE TTransState::TS_CHANGE(TUniNetMsg* unimsg, TUniNetMsg* &resp)
{
	if(tssend) 	return TSClient_CHANGE(unimsg, resp);
	else return TSServer_CHANGE(unimsg, resp);
}
	
TS_CODE TTransState::TSClient_CHANGE(TUniNetMsg* unimsg, TUniNetMsg* &resp)
{
	if(unimsg==NULL) return TSC_NULL;
	switch(tsstate)
	{
	case TS_INIT:
		tschange=FALSE;
		if(unimsg->msgName==COM_MESSAGE_REQ)
		{
			//第一次收到请求
			//发送消息事务，缓存msg，设置定时器等待响应				
			msg=new TUniNetMsg(*unimsg);
			timer=mTask->setResendTimer(transID.str());
			needrsp=COMMsgHelper::getComMsgPri(unimsg);
			//请求消息，跳转到TA_FORWARD，
			tschange=TRUE;
			tschange_from=tsstate;
			tsstate=TS_FORWARD;
			if(unimsg->hasCSeq()) cseq=unimsg->getCSeq();
			return TSC_FORWARD;
		}
		else
		{
			//直接收到的就是响应，事务还没开始就结束了，直接忽略
			return TSC_NULL;
		}
		break;
	case TS_FORWARD:
		tschange=FALSE;
		if(unimsg->msgName==COM_MESSAGE_RSP)
		{
			//收到最终响应，结束事务定时器，删除保存的请求
			if(timer)
			{
				mTask->delResendTimer(timer);
				timer=0;
			}
			if(msg)
			{
				delete msg;
				msg=NULL;
			}
			tschange=TRUE;
			tschange_from=tsstate;
			tsstate=TS_TERMINATED;
			return TSC_FORWARD;
		}
		else if(unimsg->msgName==COM_MESSAGE_TMP)
		{
			//收到临时响应，发消息收到临时响应，停止定时器，不再重发请求
			if(timer)
			{
				mTask->delResendTimer(timer);
				timer=0;
			}
			if(msg)
			{
				delete msg;
				msg=NULL;
			}
			tschange=TRUE;
			tschange_from=tsstate;
			if(needrsp) tsstate=TS_TEMPORARY;
			else tsstate=TS_TERMINATED;			//如果消息不需要最终响应，则收到临时响应就相当于结束
			return TSC_NULL;	//临时消息不上报
		}
		else
		{
			//再次收到请求，应该是重发的请求；//发消息事务，如果在还没收到响应的时候收到重发请求，则不必下发，状态机自己会重发
			return TSC_NULL;
		}
		break;
	case TS_TEMPORARY:
		tschange=FALSE;
		if(unimsg->msgName==COM_MESSAGE_RSP)
		{
			//在临时响应状态下，收到最终响应，则提供一个临时ACK
			resp=COMMsgHelper::genComMsgResp(unimsg,100);
			tschange=TRUE;
			tschange_from=tsstate;
			tsstate=TS_TERMINATED;
			return TSC_FORWARD;
		}
		else
		{
			//再次收到请求，应该是重发的请求，发消息事务，不必重复下发，状态机自己会重发
			//临时状态下收到临时响应，不管是收消息还是发消息，都抛弃
			return TSC_NULL;
		}		
		break;
	case TS_TERMINATED:
		tschange=FALSE;
		if(unimsg->msgName==COM_MESSAGE_RSP)
		{
			//再次收到响应，应该是重发的最终响应，产生一个PRACK
			resp=COMMsgHelper::genComMsgResp(unimsg,100);
			return TSC_REFUSE;
		}
		else
		{
			//结束状态下，收到任何其他消息，都抛弃
			return TSC_NULL;
		}
		break;
	}
	return TSC_NULL;
}
	
TS_CODE TTransState::TSServer_CHANGE(TUniNetMsg* unimsg, TUniNetMsg* &resp)
{
	if(unimsg==NULL) return TSC_NULL;
	switch(tsstate)
	{
	case TS_INIT:
		tschange=FALSE;
		if(unimsg->msgName==COM_MESSAGE_REQ)
		{
			//接收消息事务，不需要创建任何定时器，也不需要缓冲消息
			//请求消息，跳转到TA_FORWARD，
			needrsp=COMMsgHelper::getComMsgPri(unimsg);
			if(unimsg->hasCSeq()) cseq=unimsg->getCSeq();
			tschange=TRUE;
			tschange_from=tsstate;
			if(needrsp) tsstate=TS_FORWARD;
			else
			{
				//不需要最终响应，则直接返回临时响应
				tsstate=TS_TERMINATED;
				resp=COMMsgHelper::genComMsgResp(unimsg, 100);
			}			
			return TSC_FORWARD;
		}
		else
		{
			//直接收到的就是响应，事务还没开始就结束了，直接忽略
			return TSC_NULL;
		}
		break;
	case TS_FORWARD:
		tschange=FALSE;
		if(unimsg->msgName==COM_MESSAGE_REQ)
		{
			//再次收到请求，应该是重发的请求
			//收消息事务，在还没产生最终响应的时候，收到重发的请求，则产生临时响应
			tschange=TRUE;
			tschange_from=tsstate;
			tsstate=TS_TEMPORARY;
			resp=COMMsgHelper::genComMsgResp(unimsg, 100);
			return TSC_REFUSE;
		}
		else if(unimsg->msgName==COM_MESSAGE_RSP)
		{
			//收消息，等到最终响应，保存，以待重发
			if(msg) delete msg;
			msg=new TUniNetMsg(*unimsg);
			//收消息没有设置定时器，不需要结束定时器
			tschange=TRUE;
			tschange_from=tsstate;
			tsstate=TS_TERMINATED;
			return TSC_FORWARD;
		}
		else
		{
			return TSC_NULL;
		}
		break;
	case TS_TEMPORARY:
		tschange=FALSE;
		if(unimsg->msgName==COM_MESSAGE_RSP)
		{
			//收消息，等到最终响应，保存，以待重发
			if(msg) delete msg;
			msg=new TUniNetMsg(*unimsg);
			//发送过临时响应，说明请求没及时处理，最终响应需要确认是否发到
			timer=mTask->setResendTimer(transID.str());
			
			tschange=TRUE;
			tschange_from=tsstate;
			tsstate=TS_TERMINATED;
			return TSC_FORWARD;
		}
		else if(unimsg->msgName==COM_MESSAGE_REQ)
		{
			//收消息事务，在还没产生最终响应的时候，收到重发的请求，则产生临时响应
			resp=COMMsgHelper::genComMsgResp(unimsg,100);
			return TSC_REFUSE;
		}
		else
		{
			return TSC_NULL;
		}
		break;
	case TS_TERMINATED:
		tschange=FALSE;
		if(unimsg->msgName==COM_MESSAGE_REQ)
		{
			//再次收到请求，应该是重发的请求,则再次发送最终响应
			if(msg) 
			{
				resp=new TUniNetMsg(*msg);
				return TSC_REFUSE;
			}
			else
			{
				if(!needrsp)
				{
					//不需要最终响应，则重发临时响应
					resp=COMMsgHelper::genComMsgResp(unimsg,100);
					return TSC_REFUSE;
				}
				else
					return TSC_NULL;
			}
		}
		else if(unimsg->msgName==COM_MESSAGE_TMP)
		{
			//结束状态下，收到临时响应，说明最终响应确认发到了
			if(timer) mTask->delResendTimer(timer);
			timer=0;
			return TSC_NULL;
		}
		break;
	}
	return TSC_NULL;
}

TS_CODE	TTransState::TS_CHANGE(TTimerKey key, TUniNetMsg* &resend)
{
	if(key>0 && timer==key)
	{
		//定时器超时了，是本状态机设置的定时器
		switch(tsstate)
		{
			case TS_FORWARD:
				//一直没收到回复
				if(tssend)
				{
					//发送消息事务，重发缓存的msg，设置定时器等待响应				
					if(msg) resend=new TUniNetMsg(*msg);
					timer=mTask->setResendTimer(transID.str());
					return TSC_RESEND;
				}			
				break;
			case TS_TERMINATED:
				if(!tssend)
				{
					//发送消息事务缓存的响应，设置定时器等待确认
					if(msg) resend=new TUniNetMsg(*msg);
					timer=mTask->setResendTimer(transID.str());
					return TSC_RESEND;
				}
			case TS_INIT:
			case TS_TEMPORARY:
				//这几个状态不应该重设超时
				return TSC_NULL;		
		}
	}
	return TSC_NULL;
}

TTransMachine::~TTransMachine()
{
	mTSState.clear();
}

TS_CODE	TTransMachine::processTM(BOOL fromwire, TUniNetMsg* msg, TUniNetMsg* &resp)
{
	//从网络收到消息
	if(msg==NULL) return TSC_NULL;
	
	TTransState* state=NULL;
	if(msg->hasTransId())
	{
		CStr transid(msg->getTransId());
		if(!mTSState.find(transid, state))
		{
			if(msg->msgName==COM_MESSAGE_REQ)
			{
				state=new TTransState(!fromwire, transid, this);
				if(state) mTSState.push(transid, state);
			}
		}
		if(state) return state->TS_CHANGE(msg, resp);
	}
	return TSC_NULL;
}

TS_CODE	TTransMachine::processTM(TTimerKey tk, void* para, TUniNetMsg* &resend)
{
	CStr transid;
	transid.nTransfer((char*)para);
	//定时器超时
	if(tk>0)
	{
		TTransState* state=NULL;
		if(transid.size())
		{
			if(mTSState.find(transid, state))
			{
				if(state)
				{
					return state->TS_CHANGE(tk, resend);
				}
			}
		}
	}
	return TSC_NULL;
}
	
TS_CODE	TTransMachine::processTM(TUniNetMsg* &resp,const time_t currenttime)
{
	TTransState* state=NULL;
	while(mTSState.pop_front_old(state, currenttime))
	{
		//超时，判断状态机状态
		if(state->state()>TS_INIT && state->state()<TS_TERMINATED)
		{
			//没有收到最终响应
			if(state->isSend())
			{
				//发消息状态机，产生超时响应
				if(state->unimsg()!=NULL)
				{
					resp=COMMsgHelper::genComMsgResp(state->unimsg(),408);
					delete state;
					return TSC_TIMEOUT;
				}
			}
		}
		
		//正常结束状态机
		delete state;
		state=NULL;	
	}
	return TSC_NULL;
}
