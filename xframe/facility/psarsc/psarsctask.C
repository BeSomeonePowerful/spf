#include <stdlib.h>
#include "psarsctask.h"


TPsaRscTask::TPsaRscTask():TAbstractTask()
{
	mMaintain=0;
	mCSeq=0;
	mTaskDefault=0;
}

TPsaRscTask::~TPsaRscTask()
{
	mBindList.clear();
	mDNSCache.clear();
	if(mTransportThread) delete mTransportThread;
	if(mTransport) delete mTransport;
}

BOOL TPsaRscTask::reloadDNSCache(TiXmlElement* & extend)
{
	TiXmlHandle handle(extend);
	TiXmlElement*	psa=NULL;
	if(psa)
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
	return TRUE;
}

BOOL TPsaRscTask::reloadOtherCfg(TiXmlElement* & extend)
{
	TiXmlHandle handle(extend);
	TiXmlElement*	psa=NULL;
	if(psa)
	{
		TiXmlHandle psahandle(psa);
		TiXmlElement*	sendtimeout=NULL;
		sendtimeout=psahandle.FirstChild("send").Element();
		if(sendtimeout)
		{
			INT sendTimeOutSec;
			if(!sendtimeout->Attribute("timeout", &sendTimeOutSec)) sendTimeOutSec=DEFAULT_MSG_SEND_TIMEOUT; //默认发消息超时时间为5秒
			tminit(sendTimeOutSec);
		}
	}
	return TRUE;
}


BOOL TPsaRscTask::reloadTaskEnv(CStr& cmd, TiXmlElement* & extend)
{
	CStr scmd[5];
	cmd.split(scmd,5);
	if(scmd[0]=="reload")
	{
			if(scmd[1]=="dnscache")
			{
				return reloadDNSCache(extend);
			}
			else if(scmd[1]=="all")
			{
				if(reloadDNSCache(extend) && reloadOtherCfg(extend))
				{
					return TRUE;
				}
				else return FALSE;
			}
	}
}

BOOL TPsaRscTask::onInit(TiXmlElement*	extend)
{
	CStr objname=getObjectNameStr();
	objname+="-udp";
	TMsgAddress addr;
	addr.logAddr=getTaskId();
	addr.phyAddr=getPhyAddr();
	addr.taskInstID=getInstId();
	mTransport=new TUDPTrasnport(this, addr);
	mTransportThread=new TTransportThread(*mTransport, objname.c_str());

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
				SBind sb;
				if(!bind->Attribute("bindID", &sb.bindId)) sb.bindId=0;	//如果bindID=0，则该绑定不生效
				if(sb.bindId>0)
				{
					sb.host=bind->Attribute("localIP");
					if(!bind->Attribute("localPort", &sb.port)) sb.port=DEFAULT_BIND_PORT;
					if(!isValidIPv4(sb.host.c_str()))
					{
						UniERROR("bind address error: host=%s",sb.host.c_str());
						continue;
					}
					std::pair<std::map<UINT, SBind>::iterator, bool> rt=mBindList.insert(std::pair<UINT,SBind>(sb.bindId, sb));
					if(!rt.second)
					{
						UniERROR("bind id duplicate: id=%",sb.bindId);
						continue;
					}
					if(!mTransport->addTransport(sb.bindId, sb.host.c_str(), sb.port))
					{
						mBindList.erase(sb.bindId);
						continue;
					}
				}
				bind=bind->NextSiblingElement();
			}
		}
		TiXmlElement*	targets=NULL;
		targets=psahandle.FirstChild("targets").Element();
		if(targets)
		{
			if(!targets->Attribute("default", &mTaskDefault)) mTaskDefault=7;
			TiXmlHandle bindhandle(targets);
			TiXmlElement*	target=NULL;
			target=bindhandle.FirstChild("target").Element();
			while(target)
			{
				STask st;
				if(!target->Attribute("taskid", &st.taskid)) st.taskid=mTaskDefault;
				st.regx=target->Attribute("regx");
				if(!mTaskMapping.push_back(st))
				{
					UniERROR("target list insert error: id=%", st.taskid);
					continue;
				}
			}
		}
	}

	if(!reloadDNSCache(extend) || !reloadOtherCfg(extend))
	{
		return FALSE;
	}

	mMaintain=setTimer(TIMER_MAINTAIN_STATE);	//设置维护定时器

	if(mBindList.size())
	{
		//至少有一个合法的绑定地址
		mTransportThread->run();
		return TRUE;
	}
	else
	{
		UniERROR("bind empty address! stop running!");
		return FALSE;
	}
}

void TPsaRscTask::procMsg(std::auto_ptr<TUniNetMsg> msg)
{
	TUniNetMsg* unimsg=msg.get();
	if(unimsg==NULL) return;

	if(unimsg->sender.taskID==0 && unimsg->oAddr.logAddr==0)
	{
		//从transport收到的消息，先经过状态机，后映射为 rsc消息发到应用
		TComMsgHdr* hdr=NULL;
		TComMsgBody* body=NULL;
		hdr=dynamic_cast<TComMsgHdr*> (unimsg->getCtrlMsgHdr());
		body=dynamic_cast<TComMsgBody*> (unimsg->getMsgBody());
		if(hdr==NULL || body==NULL) return;
		TS_CODE	rt;
		TUniNetMsg* resp=NULL;
		//从网络收到消息，状态迁移
		rt=processTM(TRUE, unimsg, resp);
		if(TSC_REFUSE==rt)
		{
			//只回复响应
			if(resp)
			{
				mTransport->add(resp);
				return;
			}
		}
		else if(TSC_FORWARD==rt)
		{
			//有响应回响应，同时转发消息
			if(resp) mTransport->add(resp);
			//判断dialogid与地址映射，转发
			char* pmsg=NULL;
			pmsg=const_cast<char*>(body->info.c_str());
			TUniNetMsg* rscmsg=new TUniNetMsg();
			TRscMsgHdr* rschdr=new TRscMsgHdr();
			TRscMsgBody* rscbody=new TRscMsgBody();
			rscmsg->decode(pmsg);
			rschdr->decode(pmsg);
			rscbody->decode(pmsg);
			if(rschdr->host.user.length())
			{
				//rsc消息中的host保存了目标地址结构
				//host结构：  logAddr_phyAddr_taskInstID:linkid@IP:port
				//如果存在host.user，则存在TMsgAddress预定义结构
				//    解析后，原有的host被替换为承载消息中透传的源端地址信息结构
				rscmsg->tAddr.decodeCStr(rschdr->host.user);
				rschdr->host=hdr->argv[0];
			}
			else
			{
				//host结构中没有携带准确的地址信息，应该是第一个消息，则根据ruri查询配置文件
				if(mTaskMapping.empty())
				{
					rscmsg->tAddr.logAddr=mTaskDefault;
				}
				else
				{
					mTaskMapping.reset();
					do{
						if(rschdr->ruri.beginWith(mTaskMapping.current_r().regx.c_str()))
						{
							rscmsg->tAddr.logAddr=mTaskMapping.current_r().taskid;
							break;
						}
					}while(mTaskMapping.next());
				}
				rscmsg->tAddr.phyAddr=0;
				rscmsg->tAddr.taskInstID=0;
				rschdr->host=hdr->argv[0];
			}
                        rscmsg->tAddr.logAddr=mTaskDefault;
			rscmsg->setCtrlMsgHdr(rschdr);
			rscmsg->setMsgBody(rscbody);
			sendMsg(rscmsg);
			return;
		}
		else
		{
			//状态不对，抛弃消息
			return;
		}
	}
	else
	{
		//从应用下来的消息
		TRscMsgHdr* rschdr=NULL;
		TRscMsgBody* rscbody=NULL;
		rschdr=dynamic_cast<TRscMsgHdr*> (unimsg->getCtrlMsgHdr());
		rscbody=dynamic_cast<TRscMsgBody*> (unimsg->getMsgBody());
		if(rschdr==NULL || rscbody==NULL) return;

		TUniNetMsg* commsg=new TUniNetMsg();
		TComMsgHdr* hdr=new TComMsgHdr();
		TComMsgBody* body=new TComMsgBody();
		commsg->setCtrlMsgHdr(hdr);
		commsg->setMsgBody(body);
		commsg->oAddr=unimsg->oAddr;
		if(unimsg->msgName==RSC_REQUEST)
		{
			commsg->msgName=COM_MESSAGE_REQ;
			hdr->pri=1;	//要求应答
		}
		else if(unimsg->msgName==RSC_RESPONSE)
		{
			commsg->msgName=COM_MESSAGE_RSP;
			//响应不需要设置需要应答
		}
		else
		{
			commsg->msgName=COM_MESSAGE_REQ;
			//RSC_MESSAGE，单发，无应答
		}
		commsg->msgType=COM_MESSAGE_TYPE;
		commsg->dialogType=DIALOG_MESSAGE;
		if(unimsg->hasCSeq()) commsg->setCSeq(unimsg->getCSeq());
		else { mCSeq++; commsg->setCSeq(mCSeq);}
		if(unimsg->hasTransId()) commsg->setTransId(unimsg->getTransId());
		else commsg->setTransId();

		//rsc消息中的host保存了目标地址结构
		//host结构：  logAddr_phyAddr_taskInstID:linkid@IP:port
		//如果存在host.password，则存在linkid
		SBind bind;
		if(rschdr->host.password.length())
		{
			//存在预定义的linkid，直接构造commsg即可
			hdr->facility=rschdr->host.password.toInt();
			std::map<UINT, SBind>::iterator it=mBindList.find(hdr->facility);
			if(it!=mBindList.end())  bind=it->second;
			else bind=mBindList.begin()->second;
		}
		else
		{
			//没有预定义linkid
			if(rschdr->host.domain.length())
			{
				bind=transExist(rschdr->host.host);
				hdr->facility=bind.bindId;
			}
			else
			{
				UniERROR("no dest address!");
				delete commsg;
				return;
			}
		}
		//根据bind信息，构造commsg的地址参数
		//argc[0]=host:port，目标地址
		//argc[1]=logAddr_phyAddr_taskInstID:linkid@IP:port，本地信息
		hdr->argc=2;
		hdr->argv[0]=rschdr->host.domain;
		//unimsg->oAddr.encodeCStr(hdr->argv[1]);
		hdr->argv[1]<<unimsg->oAddr.logAddr<<"_"<<unimsg->oAddr.phyAddr<<"_"<<unimsg->oAddr.taskInstID;
		hdr->argv[1]<<":"<<bind.bindId<<"@"<<bind.host<<":"<<bind.port;

		memset(mBuffer,0,8000);
		char* msgbuffer=mBuffer;
		int length=0;
		length=unimsg->encode(msgbuffer);
		body->info.nCat(mBuffer, length);

		mTransport->add(commsg);
		return;
	}
}


void TPsaRscTask::onTimeOut(TTimerKey timerKey, void* para)
{
	if(timerKey==mMaintain)
	{
		//状态机维护定时器
		TS_CODE	 rt;
		TUniNetMsg* resp=NULL;
		do
		{
			rt=TSC_NULL;
			rt=processTM(resp, currentTime);
			if(rt==TSC_TIMEOUT && resp!=NULL)
			{
				TComMsgHdr* hdr=NULL;
				TComMsgBody* body=NULL;
				hdr=dynamic_cast<TComMsgHdr*>(resp->getCtrlMsgHdr());
				body=dynamic_cast<TComMsgBody*>(resp->getMsgBody());
				if(hdr==NULL || body==NULL) continue;
				char* pmsg=NULL;
				pmsg=const_cast<char*>(body->info.c_str());
				TUniNetMsg* rscmsg=new TUniNetMsg();
				TRscMsgHdr* rschdr=new TRscMsgHdr();
				rscmsg->decode(pmsg);
				rschdr->decode(pmsg);
				//重置msgName和address
				rscmsg->msgName=RSC_RESPONSE;
				if(rscmsg->dialogType==DIALOG_BEGIN)
					rscmsg->dialogType=DIALOG_CONTINUE;
				TURI uri(hdr->argv[1]);
				rscmsg->tAddr.decodeCStr(uri.user);
				sendMsg(rscmsg);
				delete resp;
			}
		}while(rt!=TSC_NULL);

		mMaintain=setTimer(TIMER_MAINTAIN_STATE);
	}
	else
	{
		//是状态机的定时器超时了
		TS_CODE	rt=TSC_NULL;;
		TUniNetMsg* resend=NULL;
		rt=processTM(timerKey, para, resend);
		if(rt==TSC_RESEND && resend!=NULL)
		{
			mTransport->add(resend);
		}
	}
}


TTimerKey	TPsaRscTask::setResendTimer(char* transid)
{
	return setTimer(TIMER_RESEND_TIMEOUT, transid);
}

BOOL	TPsaRscTask::delResendTimer(TTimerKey tk)
{
	return delTimer(tk);
}


SBind&	TPsaRscTask::transExist(CStr& dest)
{
	CStr dstip;
	if(!isValidIPv4(dest.c_str()))
	{
		string ip=mDNSCache[string(dest.c_str())];
		if(!ip.length())
		{
            //是域名，且没有DNS缓冲，则直接返回第一个绑定地址
			return mBindList.begin()->second;
		}
		dstip=ip.c_str();
	}
	else
	{
		dstip=dest;
	}
	CStr iphdr;
	dstip.cut(".", iphdr);

	for(std::map<UINT, SBind>::iterator iter = mBindList.begin(); iter != mBindList.end(); ++iter)
	{
		if(iter->second.host.beginWith(iphdr.c_str()))
		{
			return iter->second;
		}
	}

	//如果没找到匹配的，默认使用第一个
	return mBindList.begin()->second;
}
