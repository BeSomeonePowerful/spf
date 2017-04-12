#include "./version.h"
#include "sipstackpsa.h"

#include "info.h"
#include "resip/stack/Helper.hxx"
#include "rutil/Logger.hxx"

#include <memory>
#include <string>

using namespace std;
using namespace resip;

#define RESIPROCATE_SUBSYSTEM resip::Subsystem::DUM

SipStackPsa::SipStackPsa() : TAbstractTask() {
	mStack = NULL;
	mIsLooseRoute = true;
	mBindsCount=0;

	stackThread=NULL;

	mRegRefreshInterval=600;
	mCallRefreshInterval=60;

	mEventCSeq=0;

        UniINFO("new a SipStackPsa");
        _SETBUILDINFO;
        UniINFO("%s",getBuildInfo().c_str());	
}

SipStackPsa::~SipStackPsa() {

	if(stackThread) delete stackThread;
	mBinds.clear();
	if (mStack != NULL)
	{
		delete mStack;
		mStack = NULL;
	}

}

const resip::Data& SipStackPsa::name() const
{
	static resip::Data transactionUserName("SipPSA"); 
	return transactionUserName;
}


BOOL SipStackPsa::onInit(TiXmlElement*	extend) 
{
	db = getDB(1);
	
	
	int  id=0;
	CStr tmp1,tmp2;
	TiXmlHandle handle(extend);
	TiXmlElement*	psa=NULL;
	psa=handle.FirstChild("psa").Element();
	if(psa)
	{
		TiXmlHandle psahandle(psa);
		TiXmlElement*	binds=NULL;
		binds=psahandle.FirstChild("binds").Element();
		if(binds)
		{
			TiXmlHandle bindhandle(binds);
			TiXmlElement*	bind=NULL;
			bind=bindhandle.FirstChild("bind").Element();
			while(bind)
			{
				TBinds bindinfo;
				if(!bind->Attribute("bindID", &id)) id=0;	//如果bindID=0，则该绑定不生效
				//if(id>0 && id<__MAX_BINDS_ADDR)
				if(id>0)
				{
					/*
					mBinds[id].sentHost()=bind->Attribute("localIP");
					if(!bind->Attribute("localPort", &mBinds[id].sentPort())) mBinds[id].sentPort()=5060;
					tmp1=bind->Attribute("transportType");
					tmp2=bind->Attribute("networkType");
					if(tmp1=="UDP") mBinds[id].transport()=resip::UDP;
					else if(tmp1=="TCP") mBinds[id].transport()=resip::TCP;
					else if(tmp1=="TLS") mBinds[id].transport()=resip::TLS;
					else mBinds[id].transport()=resip::UDP;
					if(tmp2=="IPv4") mBinds[id].protocolVersion()=resip::V4;
					else if(tmp2=="IPv6") mBinds[id].protocolVersion()=resip::V6;
					else mBinds[id].protocolVersion()=resip::V4;		
					*/
					bindinfo.sentHost()=bind->Attribute("localIP");
					if(!bind->Attribute("localPort", &bindinfo.sentPort())) bindinfo.sentPort()=5060;
					tmp1=bind->Attribute("transportType");
					tmp2=bind->Attribute("networkType");
					bindinfo.transport()=resip::toTransportType(tmp1.c_str());
					if(bindinfo.transport()==resip::UNKNOWN_TRANSPORT) bindinfo.transport()=resip::UDP;
					if(tmp2=="IPv4") bindinfo.protocolVersion()=resip::V4;
					else if(tmp2=="IPv6") bindinfo.protocolVersion()=resip::V6;
					else bindinfo.protocolVersion()=resip::V4;
					
					mBinds.push_back(bindinfo);
					mBindsCount++;
				}
				bind=bind->NextSiblingElement();
			}

			TiXmlElement*	target=NULL;
			target=psahandle.FirstChild("target").Element();
			if(target)
			{
				if(!target->Attribute("regTask", &mRegTask)) mRegTask=1002;
				if(!target->Attribute("procTask", &mTargetTask)) mTargetTask=1001;
				if(!target->Attribute("detecTask", &mDetecTask)) mDetecTask=1003;
			}

			int tmpRegRefreshInterval=0;
			int tmpCallRefreshInterval=0;
			TiXmlElement*	sip=NULL;
			sip=psahandle.FirstChild("sip").Element();
			if(sip)
			{
				if(!sip->Attribute("regRefreshInterval", &tmpRegRefreshInterval)) tmpRegRefreshInterval=DLG_LIVING_TIME;
				if(!sip->Attribute("callRefreshInterval", &tmpCallRefreshInterval)) tmpCallRefreshInterval=DLG_LIVING_TIME;
			}
			else
			{
				tmpRegRefreshInterval=DLG_LIVING_TIME;
				tmpCallRefreshInterval=DLG_LIVING_TIME;
			}

			if(reloadDBEnv())
			{
				if(mRegRefreshInterval<tmpRegRefreshInterval) mRegRefreshInterval=tmpRegRefreshInterval;
				if(mCallRefreshInterval<tmpCallRefreshInterval) mCallRefreshInterval=tmpCallRefreshInterval;
			}
			else
			{
				mRegRefreshInterval=tmpRegRefreshInterval;
				mCallRefreshInterval=tmpCallRefreshInterval;
			}
			
			mDialog.setLivingTime(mRegRefreshInterval>mCallRefreshInterval?mRegRefreshInterval:mCallRefreshInterval);	//默认生存时间修改为注册刷新时间和呼叫刷新时间最大者

            TiXmlElement*   dnscache=NULL;
            TiXmlElement*   host=NULL;
            dnscache=psahandle.FirstChild("dnscache").Element();
            if(dnscache)
            {
                mDNSCache.clear();
                TiXmlHandle handle(dnscache);
                host=handle.FirstChild("host").Element();
                while(host)
                {
                    string name = host->Attribute("name");
                    string ip   = host->Attribute("ip");
                    mDNSCache.insert(std::pair<string, string>(name, ip));
                    host=host->NextSiblingElement();
                }
            }
		}
	}
	UniINFO("Init psasip OK!");
	UniINFO("  regTask=%d, procTask=%d, detecTask=%d",mRegTask,mTargetTask,mDetecTask);
	UniINFO("  rgRefreshInterval=%d, callRefreshInterval=%d", mRegRefreshInterval, mCallRefreshInterval);
	
	mStack= new resip::SipStack;
	if(mStack==NULL)
	{
		UniERROR("Error in create sip stack!!!");
		return FALSE;
	}

	CList<TBinds>::iterator iter;
	for(iter = mBinds.begin(); iter != mBinds.end(); iter++)
	{
		UniINFO("Bind IP=%s, port=%d, transportType=%d, networkType=%d", (*iter).sentHost().c_str(), (*iter).sentPort(), (*iter).transport(), (*iter).protocolVersion());
		mStack->addTransport((*iter).transport(), (*iter).sentPort(),(*iter).protocolVersion(), (*iter).stun(), (*iter).sentHost());
	}

	mLocalRealm="";
/*
	mLocalUri.host()=resip::Data(mBinds[1].sentHost());	//默认先用bind1
	mLocalUri.port()=mBinds[1].sentPort();
	mLocalVia.sentHost()=resip::Data(mBinds[1].sentHost());
	mLocalVia.sentPort()=mBinds[1].sentPort();
*/
	iter = mBinds.begin();	//用第一个添加的绑定地址做默认发送地址
	mLocalUri.host()=mLocalVia.sentHost()=resip::Data((*iter).sentHost());
	mLocalUri.port()=mLocalVia.sentPort()=(*iter).sentPort();
	
	mIsLooseRoute=TRUE;

    UniINFO("PsaId:%d",getTaskId());
	UniINFO("LocalUri:%s:%d",mLocalUri.host().c_str(),mLocalUri.port());
	UniINFO("LocalVia:%s:%d",mLocalVia.sentHost().c_str(),mLocalVia.sentPort());
	UniINFO("LocalRealm:%s",mLocalRealm.c_str());
	UniINFO("Local is Loose Router:%d",mIsLooseRoute);
	UniINFO("TargetTaskId:%d",mTargetTask);
	UniINFO("RegTaskId:%d",mRegTask);
	UniINFO("DetecTaskId:%d",mDetecTask);

	mStack->registerTransactionUser((*this));
	stackThread = new resip::StackThread(*mStack);
	if(stackThread==NULL)
	{
		UniERROR("Error in create sip stack thread!!!");
		return FALSE;
	}
	
	CStr logname;
	logname<<"log/"<<appName<<"_SipStack.log";	
	resip::Log::initialize(resip::Log::File, resip::Log::level(logLevel), Data("SipStack"), logname.c_str());
	mStack->run();		//start stack transport thread, controller thread, dns thread 
	stackThread->run();	//start stack thread

	mInDialogInviteNum=0;
	mOutDialogInviteNum=0;
	mInDialogRegesterNum=0;
	mOutDialogRegesterNum=0;
	mAllNum=0;
	
	mInvite=new COverflow(0);
	mRegester=new COverflow(0);
	mOther=new COverflow(0);

	mRecycleTimerkey=setTimer(TIMER_PSASIP_RECYCLE_DIALOG);	
	mFlowControlTimerkey=setTimer(TIMER_PSASIP_FLOWSTATUS);
}

BOOL SipStackPsa::transExist(resip::Data host, int port, resip::TransportType tt)
{
	CList<TBinds>::iterator iter;
	for(iter = mBinds.begin(); iter != mBinds.end(); iter++)
	{
		if(tt!=(*iter).transport()) continue;
		if(port!=(*iter).sentPort())  continue;
		if(host!=(*iter).sentHost())  continue;
		
		return TRUE;	//有匹配的绑定地址
	}
	return FALSE;	//无匹配的绑定地址
}

BOOL SipStackPsa::reloadDBEnv()
{
	db = getDB(1);
	BOOL ret=FALSE;
	mCallRefreshInterval = DLG_LIVING_TIME;
	cleanSQLStatement();
	sprintf(m_cSQLStatement,"select paravalue from SystemParas where paraname=\'PROXY_DLGSESSION_TIMER\';");
	if(db->execSQL(m_cSQLStatement))
	{
		PTSelectResult selectResult = db->getSelectResult();
		if ((selectResult!=NULL)&&(selectResult->rowNum>0))
		{
			TRow *row = selectResult->pRows;
			if (row != NULL)
			{
				mCallRefreshInterval=atoi(row->arrayField[0].value.stringValue);
				ret=TRUE;
			}
		}
	}
	else
	{
		UniERROR("MYSQLDB ERROR: %s",db->getErrMsg());
	}	
	
	mRegRefreshInterval=DLG_LIVING_TIME;
	cleanSQLStatement();
	sprintf(m_cSQLStatement,"select paravalue from SystemParas where paraname =\'REG_DEFAULT_EXPIRE\';");
	if (db->execSQL(m_cSQLStatement))
	{
		PTSelectResult selectResult = db->getSelectResult();
		if ((selectResult!=NULL)&&(selectResult->rowNum>0))
		{
			TRow *row = selectResult->pRows;
			if(row!=NULL)
			{
				mRegRefreshInterval = atoi(row->arrayField[0].value.stringValue);
				ret = TRUE;
			}
		}
	}
	else
	{
		UniERROR("MYSQLDB ERROR: %s",db->getErrMsg());
	}	
	
	return ret;
}

BOOL SipStackPsa::reloadTaskEnv(CStr& cmd, TiXmlElement* & extend)
{
	CStr scmd[5];
	cmd.split(scmd,5);
	if(scmd[0]=="set")
	{
		if(scmd[1]=="logLevel")
		{
			CStr logname;
			logname<<"log/"<<appName<<"_SipStack.log";
			if(scmd[2]=="DEBUG")
			{
				resip::Log::initialize(resip::Log::File, resip::Log::Debug, Data("SipStack"), logname.c_str());
			}
			else if(scmd[2]=="INFO")
			{
				resip::Log::initialize(resip::Log::File, resip::Log::Info, Data("SipStack"), logname.c_str());
			}
			else if(scmd[2]=="ERROR")
			{
				resip::Log::initialize(resip::Log::File, resip::Log::Err, Data("SipStack"), logname.c_str());
			}
			else if(scmd[2]=="default")
			{
				resip::Log::initialize(resip::Log::File, resip::Log::level(logLevel), Data("SipStack"), logname.c_str());
			}
		}
	}
	else if(scmd[0]=="reload")
	{
		TiXmlHandle handle(extend);
		TiXmlElement*	psa=NULL;
		psa=handle.FirstChild("psa").Element();
		if(psa)
		{
			if(scmd[1]=="all" || scmd[1]=="task")
			{
				int tmpRegRefreshInterval=0;
				int tmpCallRefreshInterval=0;
				TiXmlHandle psahandle(psa);
				TiXmlElement*	sip=NULL;
				sip=psahandle.FirstChild("sip").Element();
				if(sip)
				{
					if(!sip->Attribute("rgRefreshInterval", &tmpRegRefreshInterval)) tmpRegRefreshInterval=DLG_LIVING_TIME;
					if(!sip->Attribute("callRefreshInterval", &tmpCallRefreshInterval)) tmpCallRefreshInterval=DLG_LIVING_TIME;
				}
				else
				{
					tmpRegRefreshInterval=DLG_LIVING_TIME;
					tmpCallRefreshInterval=DLG_LIVING_TIME;
				}

				if(reloadDBEnv())
				{
					if(mRegRefreshInterval<tmpRegRefreshInterval) mRegRefreshInterval=tmpRegRefreshInterval;
					if(mCallRefreshInterval<tmpCallRefreshInterval) mCallRefreshInterval=tmpCallRefreshInterval;
				}
				else
				{
					mRegRefreshInterval=tmpRegRefreshInterval;
					mCallRefreshInterval=tmpCallRefreshInterval;
				}
				
				mDialog.setLivingTime(mRegRefreshInterval>mCallRefreshInterval?mRegRefreshInterval:mCallRefreshInterval);	//默认生存时间修改为注册刷新时间和呼叫刷新时间最大者
			}
			if(scmd[1]=="all" || scmd[1]=="dnscache")
			{
				TiXmlHandle psahandle(psa);
				TiXmlElement*	dnscache=NULL;
				TiXmlElement*	host=NULL;
				dnscache=psahandle.FirstChild("dnscache").Element();
				if(dnscache)
				{
					mDNSCache.clear();
					TiXmlHandle handle(dnscache);
					host=handle.FirstChild("host").Element();
					while(host)
					{
						string name = host->Attribute("name");
						string ip   = host->Attribute("ip");
						mDNSCache.insert(std::pair<string,string>(name, ip));
						
						host=host->NextSiblingElement();
					}
				}
			}				
		}
	}
}


void SipStackPsa::procMsg(std::auto_ptr<TUniNetMsg> msg)
{
//Psa 线程循环

	UINT port=0;
	CStr psalog;

	TUniNetMsg* unimsg=msg.get();
	if(!unimsg) return;
	
	if(unimsg->msgType==EVENT_MESSAGE_TYPE)
	{
		TEventMsgHdr* eventhdr=NULL;
		eventhdr=dynamic_cast<TEventMsgHdr*>(unimsg->getCtrlMsgHdr());
		if(!eventhdr) return;

		if(unimsg->sender.taskID==0)
		{
			CStr sevent;
			TEventMsgHelper::brief(unimsg, sevent);
			UniDEBUG("  Received event msg from wire...{%s}.", sevent.c_str());
			//从网络上收到通知消息, 判断应该给谁发送
			//ToDo  .. 从配置文件配置连接中断通知发送给谁
			//eventhdr->observer="需按照配置文件填写";
			eventhdr->observer="regserver";	//send to regserver
			eventhdr->trigger="psasp";	//sender
			eventhdr->cseq=++mEventCSeq;	//seq++
			unimsg->tAddr.logAddr=mRegTask;
			sendMsg(msg);
		}
	}
	else if(unimsg->msgType==SIP_MESSAGE_TYPE || unimsg->msgType==1)	//1代表所有的应用消息
	{

		TSipMsg* sipbody=NULL;
		sipbody=dynamic_cast<TSipMsg*>(unimsg->getMsgBody());
		if(!sipbody) return;
		TSipCtrlMsg* sipctrl=NULL;
		sipctrl=dynamic_cast<TSipCtrlMsg*>(unimsg->getCtrlMsgHdr());
		if(!sipctrl) return;

		mDid=sipctrl->dialogid;
		
		resip::SipMessage* sipmsg=sipbody->sipMsg;
		if(sipmsg==NULL) return;

		if(unimsg->sender.taskID==0)
		{
			UniDEBUG("  Received a msg from wire....");
			//psalog<<"Received a msg from wire:";
			//从网络上收到消息, 先进行消息检查
			if(!checkSipMessage(sipmsg)) return;

			psalog<<"msg from wire:"<<SIPMsgHelper::brief(sipmsg).c_str();
			UniDEBUG("Received a msg from wire:%s", psalog.c_str());
			
			//判断消息是否是对话内消息
			CSIPPsaDialogInfo dialoginfo;
			if(mDialog.QueryDialogTab(sipctrl->dialogid, dialoginfo))
			{
				unimsg->tAddr=dialoginfo.mAddr;
				
				if(unimsg->tAddr.logAddr==0)
				{
				//app do not response the dialog init request yet
					if(sipctrl->method == SIP_REGISTER)
					{
						unimsg->tAddr.logAddr=mRegTask;             
						mInDialogRegesterNum++;
						//mRegester->checkOverflow(currentTime);
					}
					else if(sipctrl->method == SIP_OPTIONS)
					{
						unimsg->tAddr.logAddr=mDetecTask;
						mOtherNum++;
					}
					else
					{
						//默认都送到INVITE task
						unimsg->tAddr.logAddr=mTargetTask;
						mInDialogInviteNum++;
						//mInvite->checkOverflow(currentTime);
					}
					mAllNum++;
				}
				unimsg->dialogType=DIALOG_CONTINUE;
				if(sipctrl->dialogid != dialoginfo.mDialogID_F ) sipctrl->dialogid=dialoginfo.mDialogID_F;

				mDialog.UpdateDialogExpire(dialoginfo.mDialogID_F, currentTime); 

				UniDEBUG("  Received in dialog msg: did=%s; send to task:%d,%d,%d", sipctrl->dialogid.c_str(), unimsg->tAddr.logAddr, unimsg->tAddr.phyAddr, unimsg->tAddr.taskInstID);
				sysLog(mDid,LOG_INFO,psalog,MSGINFO);
				sendMsg(msg);
				return;
			}
			else
			{
				//Dialog 外消息，判断是否应该创建Dialog

				if(unimsg->msgName==SIP_MESSAGE_RSP
					|| sipctrl->method == SIP_ACK 
					|| sipctrl->method == SIP_INFO 
					|| sipctrl->method == SIP_BYE 
					|| sipctrl->method == SIP_CANCEL
					|| sipctrl->method == SIP_PRACK
					|| sipctrl->method == SIP_UPDATE
					|| sipctrl->method == SIP_UNKNOWN)
				{
					//状态码大于0，是响应，不可能对话外，按照直接将消息转发走
					//是请求，但不可能是对话外的请求，则按照宽松路由规则转发

					sipbody->release();	//释放TUninetMsg 的sip 指针
					
					//把错误输出修改为ERROR级别，提示更多信息，正常情况下不应该出现
					UniERROR("  Received Unexpected msg: did=%s, send to stack..., %s", sipctrl->dialogid.c_str(), SIPMsgHelper::brief(sipmsg).c_str());
					sysLog(mDid,LOG_INFO,psalog,MSGINFO);
					sendSipMessage(sipmsg);
					return;	
				}

				//是创建对话请求

				CStr did_B;
				did_B=SIPMsgHelper::computeDialogId(sipmsg->header(h_To).uri().user().c_str(), sipmsg->header(h_From).uri().user().c_str(), sipmsg->header(h_CallID).value().c_str());
				mDialog.InsertDialogTab(sipctrl->dialogid, did_B);

				if(sipctrl->method == SIP_REGISTER)
				{
					unimsg->tAddr.logAddr=mRegTask;				//注册请求发送给注册处理任务
					if(!mRegester->checkOverflow(currentTime))
					{
						//todo 如果被流控，需要返回拒绝响应
						return OverFlowMakeResp(sipmsg);
					}
					mOutDialogRegesterNum++;
				}
				else if(sipctrl->method == SIP_OPTIONS)
				{
					unimsg->tAddr.logAddr=mDetecTask;
					if(!mOther->checkOverflow(currentTime))
					{
						//todo 如果被流控，需要返回拒绝响应
						return OverFlowMakeResp(sipmsg);
					}
					mOtherNum++;
				}
				else if(sipctrl->method == SIP_INVITE)
				{
					unimsg->tAddr.logAddr=mTargetTask;
					if(!mInvite->checkOverflow(currentTime))
					{
						//todo 如果被流控，需要返回拒绝响应
						return OverFlowMakeResp(sipmsg);
					}
					mOutDialogInviteNum++;
				}
				else
				{
					unimsg->tAddr.logAddr=mTargetTask;
					if(!mOther->checkOverflow(currentTime))
					{
						//todo 如果被流控，需要返回拒绝响应
						return OverFlowMakeResp(sipmsg);
					}
					mOtherNum++;
				}
				
				mAllNum++;
				unimsg->dialogType=DIALOG_BEGIN;

				UniDEBUG("  Received out dialog msg: did=%s, send to task:%d,%d,%d", sipctrl->dialogid.c_str(),unimsg->tAddr.logAddr, unimsg->tAddr.phyAddr, unimsg->tAddr.taskInstID);
				sendMsg(msg);
				return;
			}
		}
		else
		{
			//UniDEBUG("Rceived a msg from task:%s",SIPMsgHelper::SIPMsgHelper::brief(sipmsg).c_str());
			
			psalog<<"msg from task:"<<SIPMsgHelper::brief(sipmsg).c_str();
			UniDEBUG(psalog.c_str());
			//从内部任务收到消息
			
			CSIPPsaDialogInfo dialoginfo;
			if(sipctrl->dialogid.empty())
			{
				mDid=SIPMsgHelper::computeDialogId(sipmsg->header(h_From).uri().user().c_str(), sipmsg->header(h_To).uri().user().c_str(), sipmsg->header(h_CallID).value().c_str());
				sipctrl->dialogid=mDid;
			}
			
			if(mDialog.QueryDialogTab(mDid, dialoginfo))
			{
				//Dialog内消息

				//如果对话信息中的logAddr 和instID 没有初始化，或与当前消息的源地址不同，则根据消息中的信息更新
				if(!(dialoginfo.mAddr==unimsg->oAddr))
				{
					//dialoginfo.mAddr=unimsg->oAddr;
					mDialog.UpdateDialogAddr(mDid, unimsg->oAddr);
					
				}
				// 应用下发消息就不更新了
				//mDialog.UpdateDialogExpire(dialoginfo.mDialogID_F, currentTime);	//用ID_F 更新，少一次检索时间
				UniDEBUG("  Received in dialog msg: did=%s, send to stack...", sipctrl->dialogid.c_str());
			}
			else
			{
				//modify by LJL. 2014.8.7. support out dialog NOTIFY.
				//Dialog 外消息，判断是否应该创建Dialog
				if(unimsg->msgName!=SIP_MESSAGE_RSP 
					&& sipctrl->method != SIP_RESPONSE
					&& sipctrl->method != SIP_ACK 
					&& sipctrl->method != SIP_INFO 
					&& sipctrl->method != SIP_BYE 
					&& sipctrl->method != SIP_CANCEL
					&& sipctrl->method != SIP_PRACK
					&& sipctrl->method != SIP_UPDATE
					&& sipctrl->method != SIP_UNKNOWN)
				{
					CStr did_B;
					did_B=SIPMsgHelper::computeDialogId(sipmsg->header(h_To).uri().user().c_str(), sipmsg->header(h_From).uri().user().c_str(), sipmsg->header(h_CallID).value().c_str());
					mDialog.InsertDialogTab(mDid, did_B, unimsg->oAddr);
				}

				//是响应，或是请求，但不是对话外的请求，直接下发
				UniDEBUG("  Received out dialog msg: did=%s, send to stack...", sipctrl->dialogid.c_str());
			}
			
			sysLog(mDid,LOG_INFO,psalog,MSGINFO);
			sipbody->release();	//释放对sip消息指针的控制权
			sendSipMessage(sipmsg);
		
			return;
		}	
	}
}


BOOL SipStackPsa::checkSipMessage(resip::SipMessage* sipmsg)
{
	UINT port;
	resip::TransportType  transtype;

	//syslog add by zh
	CStr psalog;

	if (sipmsg->isRequest())
	{
	/*	协议栈会处理from tag缺失问题
		if (!sipmsg->header(h_From).exists(p_tag))
		{
		//Must Exist From Tag
			//UniERROR("From tag missing, make 400 response!! CallID=%s",sipmsg->header(h_CallID).value().c_str());
			psalog<<"From tag missing, make 400 response CallID="<<sipmsg->header(h_CallID).value().c_str();
			UniERROR(psalog);
			std::auto_ptr<SipMessage> response(Helper::makeResponse(*sipmsg, 400));
			mStack->send(response, this);
				
			sysLog(mDid,LOG_WARNING,psalog,MSGINFO);
			
			return FALSE;
		}
		*/
		// The TU selector already checks the URI scheme for us (Sect 16.3, Step 2)

		// check the MaxForwards isn't too low
		if (!sipmsg->exists(h_MaxForwards) || sipmsg->header(h_MaxForwards).value() > 70)
		{			// .bwc. Add Max-Forwards header if not found.
			sipmsg->header(h_MaxForwards).value()=70;
		}			// .bwc. Unacceptable values for Max-Forwards			
		else if(sipmsg->header(h_MaxForwards).value() <=1)
		{
			//UniERROR("MaxForward=0, make 483 response!! CallID=%s",sipmsg->header(h_CallID).value().c_str());
			std::auto_ptr<SipMessage> response(Helper::makeResponse(*sipmsg, 483));	
			psalog<<"MaxForward=0, make 483 response CallID="<<sipmsg->header(h_CallID).value().c_str();
			UniERROR(psalog.c_str());
			mStack->send(response, this);
			sysLog(mDid,LOG_WARNING,psalog,MSGINFO);

			return FALSE;
		}

		//Check the Self-Loop			
		port=sipmsg->header(h_Vias).front().sentPort();
		transtype=resip::toTransportType(sipmsg->header(h_Vias).front().transport());
		if(port==0) port=5060;
		if(transtype==resip::UNKNOWN_TRANSPORT) transtype=resip::UDP;
		if(transExist(sipmsg->header(h_Vias).front().sentHost(), port, transtype ))
		//if(sipmsg->header(h_Vias).front().sentHost()==mLocalUri.host() && port==mLocalUri.port())
		{
			//UniERROR("Critical ERROR detected!!!!!!!!Self-Loop happened!!! top via=self, make 482 response!! CallID=%s",sipmsg->header(h_CallID).value().c_str()); 
			std::auto_ptr<SipMessage> response(Helper::makeResponse(*sipmsg, 482));
			psalog<<"Self-Loop happened top via=self, make 482 response CallID="<<sipmsg->header(h_CallID).value().c_str();
			UniERROR(psalog.c_str());
			mStack->send(response, this);			
			
			sysLog(mDid,LOG_WARNING,psalog,MSGINFO);
	
			return FALSE; 			
		}			

/*	//modify by LJL. 2015-05-27 不再检查toproute和requesturi是不是指向本机，支持终端或其他proxy强制将消息转发给本机
	//本机可根据Route指示或RequestURI指示转发消息
		//Check the Top Route
		if(sipmsg->exists(h_Routes)&&(!sipmsg->header(h_Routes).empty()))
		{
			port=sipmsg->header(h_Routes).front().uri().port();
			if(port==0) port=5060;
			if(sipmsg->header(h_Routes).front().uri().exists(p_transport))
			{
				transtype=resip::toTransportType(sipmsg->header(h_Routes).front().uri().param(p_transport));
			}
			else
			{
				transtype=resip::UDP;
			}
			//if(sipmsg->header(h_Routes).front().uri().host() != mLocalUri.host() || port != mLocalUri.port())
			if(!transExist(sipmsg->header(h_Routes).front().uri().host(), port, transtype ))
			{
				//if top route != LocalHost and RequestURI != LocalHost					
				// prev host is a strick Route host					
				port=sipmsg->header(h_RequestLine).uri().port();					
				if(port==0) port=5060;	
				if(sipmsg->header(h_RequestLine).uri().exists(p_transport))
				{
					transtype=resip::toTransportType(sipmsg->header(h_RequestLine).uri().param(p_transport));
				}
				else
				{
					transtype=resip::UDP;
				}				
				//if( sipmsg->header(h_RequestLine).uri().host()!= mLocalUri.host() || port!=mLocalUri.port() )
				if(transExist(sipmsg->header(h_RequestLine).uri().host(), port, transtype ))
				{
					UniERROR("Critical ERROR detected!!!!!!!! Need Loose Route!!! request uri=self, make 403 response!! CallID=%s, TRoute=%s:%p",sipmsg->header(h_CallID).value().c_str(),sipmsg->header(h_Routes).front().uri().host().c_str(),sipmsg->header(h_Routes).front().uri().port());
					std::auto_ptr<SipMessage> response(Helper::makeResponse(*sipmsg, 403));	
					psalog<<"Need Loose Route.request uri=self, make 403 response CallID="<<sipmsg->header(h_CallID).value().c_str();
					psalog<<",TRoute="<<sipmsg->header(h_Routes).front().uri().host().c_str()<<sipmsg->header(h_Routes).front().uri().port();
					mStack->send(response, this);

					sysLog(mDid,LOG_WARNING,psalog,MSGINFO);

					return FALSE; 
				}
			}
		}
*/
		if(sipmsg->header(h_RequestLine).getMethod()==resip::CANCEL)
		{
			//UniDEBUG("Received CANCEL, make 200 response!!");
		   	std::auto_ptr<SipMessage> response(Helper::makeResponse(*sipmsg, 200));	
			psalog<<"Received CANCEL, make 200 response"<<sipmsg->header(h_CallID).value().c_str();
		   	UniDEBUG(psalog.c_str());
			mStack->send(response, this);
			
			sysLog(mDid,LOG_WARNING,psalog,MSGINFO);
		}
	}
	else
	{
		port=sipmsg->header(h_Vias).front().sentPort();
		transtype=resip::toTransportType(sipmsg->header(h_Vias).front().transport());
		if(port==0) port=5060;
		if(transtype==resip::UNKNOWN_TRANSPORT) transtype=resip::UDP;
		if(!transExist(sipmsg->header(h_Vias).front().sentHost(), port, transtype ))
//		if (sipmsg->header(h_Vias).front().sentHost() != mLocalUri.host() || port != mLocalUri.port())
		{
				//UniERROR("Critical ERROR detected!!!!!!!! Need Loose Route!!! top via is not me!! CallID=%s,TVia=%s:%d",sipmsg->header(h_CallID).value().c_str(),sipmsg->header(h_Vias).front().sentHost().c_str(),port,sipmsg->header(h_Vias).front().transport());
				//UniERROR(sipmsg->SIPMsgHelper::brief());
				//UniERROR("Top Via: "<<sipmsg->header(h_Vias).front()<<" is NOT my Host!\n");
				psalog<<"Need Loose Route. top via is not me, make 403 response CallID="<<sipmsg->header(h_CallID).value().c_str();
                psalog<<",TVia="<<sipmsg->header(h_Vias).front().sentHost().c_str()<<":"<<sipmsg->header(h_Vias).front().sentPort()<<"/"<<sipmsg->header(h_Vias).front().transport().c_str();
                UniERROR(psalog.c_str());
				sysLog(mDid,LOG_WARNING,psalog,MSGINFO);

				return FALSE;
			}
	}

	return TRUE;
}


void SipStackPsa::sendSipMessage(resip::SipMessage* sipmsg)
{
	UINT port;
	resip::TransportType  transtype;

	//下发消息之前进行检查
	if(sipmsg->isResponse())
	{
	//如果是响应，则检查via
		if(sipmsg->header(h_Vias).empty()) return;
		port=sipmsg->header(h_Vias).front().sentPort();
		transtype=resip::toTransportType(sipmsg->header(h_Vias).front().transport());
		if(port==0) port=5060;
		if(transtype==resip::UNKNOWN_TRANSPORT) transtype=resip::UDP;
		if(transExist(sipmsg->header(h_Vias).front().sentHost(), port, transtype ))
		{
			sipmsg->header(h_Vias).pop_front();
			if(sipmsg->header(h_Vias).empty()) return;
		}
		
		std::auto_ptr<SipMessage> msg(sipmsg);
		mStack->send(msg, this);
		return;	
	}

	//如果是请求，则要检查route、via 
	//Check and Remove Top Route	
	if(sipmsg->exists(h_Routes)&&(!sipmsg->header(h_Routes).empty())) 
	{
		//remove Top Route if it == LocalHost		
		port=sipmsg->header(h_Routes).front().uri().port();
		if(port==0) port=5060;
		if(sipmsg->header(h_Routes).front().uri().exists(p_transport))  transtype=resip::toTransportType(sipmsg->header(h_Routes).front().uri().param(p_transport));
		else	transtype=resip::UDP;
		if(transExist(sipmsg->header(h_Routes).front().uri().host(), port, transtype ))
		{
			sipmsg->header(h_Routes).pop_front();
			//add by LJL 2014.5.4
			//可能存在双RR的情况（承载类型不同），判断删除route两次，以避免可能存在的发送给自己导致loop的错误
			if(!sipmsg->header(h_Routes).empty())
			{
				port=sipmsg->header(h_Routes).front().uri().port();
				if(port==0) port=5060;
				if(sipmsg->header(h_Routes).front().uri().exists(p_transport))  transtype=resip::toTransportType(sipmsg->header(h_Routes).front().uri().param(p_transport));
				else	transtype=resip::UDP;
				if(transExist(sipmsg->header(h_Routes).front().uri().host(), port, transtype ))
				{
					sipmsg->header(h_Routes).pop_front();
				}	
			}
		}

		//check if next hop is strick Route 	
		if(!sipmsg->header(h_Routes).empty())
		{
			if(!sipmsg->header(h_Routes).front().uri().exists(p_lr))
			{	
				//no lr, is Strick Route;	
				//value the URI of RequestURI with Top Route URI;
				sipmsg->header(h_RequestLine).uri().host()=sipmsg->header(h_Routes).front().uri().host();
				sipmsg->header(h_RequestLine).uri().port()=sipmsg->header(h_Routes).front().uri().port();
				if(sipmsg->header(h_Routes).front().uri().exists(p_transport))
				{
                    sipmsg->header(h_RequestLine).uri().param(p_transport)=sipmsg->header(h_Routes).front().uri().param(p_transport);
                }
				sipmsg->header(h_Routes).pop_front();	
			}
		}
			
	}

	//Check Force Target	
	if (!sipmsg->hasForceTarget())	
	{		
		if(sipmsg->header(h_Routes).empty())
		{
			sipmsg->setForceTarget(sipmsg->header(h_RequestLine).uri());
		}
		else
		{
			sipmsg->setForceTarget(sipmsg->header(h_Routes).front().uri());		
			//remove routes in CANCEL, after setForceTarget	
			if(sipmsg->header(h_RequestLine).getMethod()==resip::CANCEL)	
			{		 
				sipmsg->remove(h_Routes);	
			}
		}	
	}
	
	//add by LJL 20160329 如果forceTarget的host不是ip地址，则认为可能是域名，尝试用本地缓存
	if(!isValidIPv4(sipmsg->getForceTarget().host().c_str()))
	{
		string ip=mDNSCache[string(sipmsg->getForceTarget().host().c_str())];
		if(ip.length())
		{
            resip::Uri tg=sipmsg->getForceTarget();
            tg.host()=resip::Data(ip.c_str());
            sipmsg->setForceTarget(tg);
		}
		//如果没有缓存，则会给协议栈，去dns解析	
	}
	
/* 协议栈不知道该加那个地址做via，需应用根据类型自己决定
	//push local via	
	//remove old vias in CANCEL 
	if(!sipmsg->header(h_Vias).empty())	
	{	
		//do not remove CANCEL vias, the top via tid must add by INVITE and CANCEL proc themself!!!!
		//psa do not know the relationship of INVITEs and CANCELs !!!!
			port=sipmsg->header(h_Vias).front().sentPort();		
			if( port==0 ) port = 5060;		
			if( sipmsg->header(h_Vias).front().sentHost() != mLocalUri.host() || port != mLocalUri.port()) 	
			{
				resip::Via via;
				via.sentHost()=mLocalVia.sentHost();
				via.sentPort()=mLocalVia.sentPort();
				sipmsg->header(h_Vias).push_front(via);
			}
	}
*/
	if(sipmsg->header(h_MaxForwards).value()>0) sipmsg->header(h_MaxForwards).value()--;

	//LogDEBUG(<<"Send a sip message: " << *sipmsg );

	std::auto_ptr<SipMessage> msg(sipmsg);
	mStack->send(msg, this);
	return;

}

void SipStackPsa::closeResources() {
	UniINFO("close mStack.");
    mStack->shutdown();
    UniINFO("ok!");
    UniINFO("close stackThread.");
//	stackThread->detach();
	UniINFO("ok!");
	UniINFO("close transportThread.");
	//transportThread->detach();
	UniINFO("ok!");
	UniINFO("close recvTransportThread.");
	//recvTransportThread->detach();
	UniINFO("ok!");
} 

void SipStackPsa::printState(CStr* cStr) {
	
	if(cStr==NULL) return;

	*cStr<<" TaskInfo:\r\n";
	*cStr<<"   Local addr=";
	*cStr<<mLocalUri.host().c_str();
	*cStr<<":";
	*cStr<<mLocalUri.port();
	*cStr<<", loose router=";
	*cStr<<mIsLooseRoute;
	*cStr<<"\r\n";
	*cStr<<"  Target task: regTask=";
	*cStr<<mRegTask;
	*cStr<<"  Detec task: detecTask=";
	*cStr<<mDetecTask;
	*cStr<<", proxyTask=";
	*cStr<<mTargetTask;
	*cStr<<"\r\n";
	CList<TBinds>::iterator iter;
	for(iter = mBinds.begin(); iter != mBinds.end(); iter++)
	{
		*cStr<<"  Bind addr="<<(*iter).sentHost().c_str()<<":"<<(*iter).sentPort();
		*cStr<<",transport="<<(*iter).transport();
		*cStr<<",pto="<<(*iter).protocolVersion()<<"\r\n";
	}
	/*
	for(int i=1;i<=mBindsCount;i++)
	{
		*cStr<<"  Bind addr=";
		*cStr<<mBinds[i].sentHost().c_str();
		*cStr<<":";
		*cStr<<mBinds[i].sentPort();
		*cStr<<",transport=";
		*cStr<<mBinds[i].transport();
		*cStr<<",version=";
		*cStr<<mBinds[i].protocolVersion();
		*cStr<<"\r\n";
	}*/
}



void SipStackPsa::postToTransactionUser(resip::Message* msg, resip::TimeLimitFifo<Message>::DepthUsage usage)
{
	post(msg);
}

void SipStackPsa::post(Message* msg)
{
//Sip协议栈上报消息，封装成UniNetMsg 写到消息队列
//这个函数是SipStack 线程调用的，不能访问SipStackPsa 中的参数
	
	SipMessage* sipmsg=dynamic_cast<SipMessage*>(msg);
	if(sipmsg)
	{
		//UniDEBUG("Received a sip message: ");
		DebugLog( << " Received sip msg:" << *sipmsg << "\n");
		TUniNetMsg* unimsg=NULL;
		TSipCtrlMsg* ctrlmsg=NULL;
		TSipMsg* msgbody=NULL;
		unimsg=new TUniNetMsg();
		if(unimsg)
		{
			unimsg->sender.taskID=0;
			unimsg->sender.taskType=0;
			unimsg->sender.instID=0;
			if(sipmsg->isRequest()) unimsg->msgName=SIP_MESSAGE_REQ;
			else unimsg->msgName=SIP_MESSAGE_RSP;
			unimsg->msgType=SIP_MESSAGE_TYPE;

			SIPMsgHelper::addCtrlMsgHdr(unimsg,sipmsg);
			SIPMsgHelper::addMsgBody(unimsg, sipmsg);

			sendMsgSelf((TMsg*)(unimsg));
		}
		return;
	}
	//是连接中断消息
	resip::ConnectionTerminated* term = dynamic_cast<resip::ConnectionTerminated*>(msg);
    if (term)
    {
		DebugLog( << " Received connection terminated msg:" << *term << "\n");
 		TUniNetMsg* unimsg=NULL;
		TEventMsgHdr* eventhdr=NULL;
		TEventMsgBody* body=NULL;
		unimsg=new TUniNetMsg();
		if(unimsg)
		{
			unimsg->sender.taskID=0;
			unimsg->sender.taskType=0;
			unimsg->sender.instID=0;		
			unimsg->msgName=EVENT_MESSAGE_REQ;
			unimsg->msgType=EVENT_MESSAGE_TYPE;
			unimsg->dialogType=DIALOG_MESSAGE;
			
			eventhdr=new TEventMsgHdr();
			if(eventhdr)
			{
				eventhdr->status=0;	//Request=0

				unimsg->setCtrlMsgHdr(eventhdr);
			}
			body=new TEventMsgBody();
			if(body)
			{
				body->ev="conbroken";	//connection broken
				body->einfo="conid=";	//conid=xxx
				ULONG conid=term->getFlowKey();
				body->einfo<<conid;
				unimsg->setMsgBody(body);
			}
			sendMsgSelf((TMsg*)(unimsg));
		}
        delete term;
       return;
    }
	
	
	DebugLog( << " Received a message, but is not a sip/ConnectionTerminated message!");
	
}

void SipStackPsa::onTimeOut(TTimerKey timerKey, void* para)
{
	if(mRecycleTimerkey==timerKey)
	{
		mDialog.CleanExpireDialog();
		mRecycleTimerkey=setTimer(TIMER_PSASIP_RECYCLE_DIALOG);
	}
	else if(mFlowControlTimerkey==timerKey)
	{
		OverFlow();		
		mFlowControlTimerkey=setTimer(TIMER_PSASIP_FLOWSTATUS);
	}

}

void SipStackPsa::OverFlow()
{
		CStr overflowlog;
		TTimeString Curtime,INVITE_max_caps_time,REG_max_caps_time,Other_max_caps_time;
		
		ttToTimeString(currentTime,Curtime);
		//Overflow count INVITE:总数(建立对话/对话内) REGISTER:总数(建立对话/对话内) 所有消息总数
		overflowlog<<Curtime<<"/";
		overflowlog<<"Overflow count "<<"/";
		overflowlog<<"I:"<<"/"<<mOutDialogInviteNum+mInDialogInviteNum<<"/"<<mOutDialogInviteNum<<"/"<<mInDialogInviteNum<<"/";
		overflowlog<<"R:"<<"/"<<mInDialogRegesterNum+mOutDialogRegesterNum<<"/"<<mOutDialogRegesterNum<<"/"<<mInDialogRegesterNum<<"/";
		overflowlog<<"A:"<<"/"<<mAllNum<<"/";
		
		//清空计数器
		mInDialogInviteNum=0;
		mOutDialogInviteNum=0;
		mInDialogRegesterNum=0;
		mOutDialogRegesterNum=0;
		mAllNum=0;
				
		mRegester->getFlowInfo(mRegesterInfo);
		mInvite->getFlowInfo(mInviteInfo);
		mOther->getFlowInfo(mOtherInfo);
		
		ttToTimeString(mRegesterInfo.max_caps_time,REG_max_caps_time);
		ttToTimeString(mInviteInfo.max_caps_time,INVITE_max_caps_time);
		ttToTimeString(mOtherInfo.max_caps_time,Other_max_caps_time);
		
		//Overflow status INVITEcaps:最大话务量(出现时间) REGISTERcaps:最大话务量(出现时间) 其他消息caps:最大话务量（出现时间）
		overflowlog<<"Overflow status "<<"/";
		overflowlog<<"Imc:"<<"/"<<mInviteInfo.max_caps<<"/"<<(mInviteInfo.max_caps==0?" ":INVITE_max_caps_time)<<"/";    
		overflowlog<<"Rmc:"<<"/"<<mRegesterInfo.max_caps<<"/"<<(mRegesterInfo.max_caps==0?" ":REG_max_caps_time)<<"/";
		overflowlog<<"Omc"<<"/"<<mOtherInfo.max_caps<<"/"<<(mOtherInfo.max_caps==0?" ":Other_max_caps_time)<<"/";
		
		UniINFO(overflowlog.c_str());
		sysLog("ofinfo",LOG_INFO,overflowlog,STATICINFO);
}

void SipStackPsa::OverFlowMakeResp(resip::SipMessage* sipmsg)
{
	CStr psalog;
	std::auto_ptr<SipMessage> response(Helper::makeResponse(*sipmsg, 503, "overflow, try again later"));
	psalog<<"overflow, make 503 response CallID="<<sipmsg->header(h_CallID).value().c_str();
	UniINFO(psalog.c_str());
	mStack->send(response, this);			
			
	sysLog(mDid,LOG_WARNING,psalog,MSGINFO);
	
}

