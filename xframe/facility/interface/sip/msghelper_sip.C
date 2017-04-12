#include "msghelper_sip.h"
#include "cmd5.h"

using namespace resip;


CSIPCredentials::CSIPCredentials()
{
	m_Qop = 0;
	m_ucAlgorithm = MD5;
	m_ucMsgType = resip::UNKNOWN;

	valid=FALSE;
}

const CStr& CSIPCredentials::GetUserNameRef() const
{
	return m_UserName;
}

CStr& CSIPCredentials::GetUserNameRef()
{
	return m_UserName;
}

void CSIPCredentials::SetUserName( const char* UserName )
{
	m_UserName = UserName;
}

const CStr& CSIPCredentials::GetRealmRef() const
{
	return m_Realm;
}

CStr& CSIPCredentials::GetRealmRef()
{
	return m_Realm;
}

void CSIPCredentials::SetRealm( const char*  Realm )
{
	m_Realm = Realm;
}

const CStr& CSIPCredentials::GetQopRef() const
{
	return m_Qop;
}

void CSIPCredentials::SetQop( const char* Qop )
{
	m_Qop = Qop;
}

const CStr& CSIPCredentials::GetNonceRef() const
{
	return m_Nonce;
}

CStr& CSIPCredentials::GetNonceRef()
{
	return m_Nonce;
}

void CSIPCredentials::SetNonce( const char* Nonce )
{
	m_Nonce = Nonce;
}

const CStr& CSIPCredentials::GetOpaqueRef() const
{
	return m_Opaque;
}

CStr& CSIPCredentials::GetOpaqueRef()
{
	return m_Opaque;
}

void CSIPCredentials::SetOpaque( const char* Opaque )
{
	m_Opaque = Opaque;
}

const CStr& CSIPCredentials::GetURIRef() const
{
	return m_URI;
}

inline CStr& CSIPCredentials::GetURIRef()
{
	return m_URI;
}

void CSIPCredentials::SetURI( const char* URI )
{
	m_URI = URI;
}

const CStr& CSIPCredentials::GetResponseRef() const
{
	return m_Response;
}

CStr& CSIPCredentials::GetResponseRef()
{
	return m_Response;
}

void CSIPCredentials::SetResponse( const char* Response )
{
	m_Response = Response;
}

CSIPCredentials::Algthm CSIPCredentials::GetAlgorithm() const
{
	return m_ucAlgorithm;
}

void CSIPCredentials::SetAlgorithm( CSIPCredentials::Algthm Algorithm )
{
	m_ucAlgorithm = Algorithm;
}

const CStr& CSIPCredentials::GetCNonceRef() const
{
	return m_CNonce;
}

CStr& CSIPCredentials::GetCNonceRef()
{
	return m_CNonce;
}

void CSIPCredentials::SetCNonce( const char* CNonce )
{
	m_CNonce = CNonce;
}

const CStr& CSIPCredentials::GetNonceCountRef() const
{
	return m_NonceCount;
}

CStr& CSIPCredentials::GetNonceCountRef()
{
	return m_NonceCount;
}

void CSIPCredentials::SetNonceCount( const char* NonceCount )
{
	m_NonceCount = NonceCount;
}

resip::MethodTypes CSIPCredentials::GetMsgType() const
{
	return m_ucMsgType;
}

void CSIPCredentials::SetMsgType( resip::MethodTypes MsgType )
{
	m_ucMsgType = MsgType;
}

const CStr& CSIPCredentials::GetHRef() const
{
	return m_H;
}

CStr& CSIPCredentials::GetHRef()
{
	return m_H;
}

void CSIPCredentials::SetH( const char* H )
{
	m_H = H;
};

INT CSIPCredentials::Size() const
{
	INT size = 0;

	size += m_UserName.size();
	size += m_Realm.size();
	size += m_Qop.size();
	size += m_Nonce.size();
	size += m_Opaque.size();
	size += m_URI.size();
	size += m_Response.size();
	size += sizeof(Algthm);
	size += m_CNonce.size();
	size += m_NonceCount.size();
	size += sizeof(resip::MethodTypes);
	size += m_H.size();

	return size;
};

const char* SIPMsgHelper::methodName(UINT type)
{
	if(type>=SIP_UNKNOWN && type<SIP_MAX_METHODS)
	{
		return sipmethodname[type];
	}
	else
	{
		return sipmethodname[SIP_UNKNOWN];
	}
}

const char* SIPMsgHelper::methodName(SIPMETHODTYPE type)
{
	return sipmethodname[type];
}

//Convert Message Type
SIPMETHODTYPE SIPMsgHelper::methodType(resip::MethodTypes type)
{
	int tmp;
	tmp=type;
	return (SIPMETHODTYPE)tmp;
	/*	switch(type){
	case UNKNOWN:
		return SIP_UNKNOWN;
	case ACK:
		return SIP_ACK;
	case BYE:
		return SIP_BYE;
	case CANCEL:
		return SIP_CANCEL;
	case INVITE:
		return SIP_INVITE;
	case NOTIFY:
		return SIP_NOTIFY;
	case OPTIONS:
		return SIP_OPTIONS;
	case REFER:
		return SIP_REFER;
	case REGISTER:
		return SIP_REGISTER;
	case SUBSCRIBE:
		return SIP_SUBSCRIBE;
	case RESPONSE:
		return SIP_RESPONSE;
	case MESSAGE:
		return SIP_MESSAGE;
	case INFO:
		return SIP_INFO;
	case PRACK:
		return SIP_PRACK;
	case PUBLISH:
		return SIP_PUBLISH;
	case SERVICE:
		return SIP_SERVICE;
	case UPDATE:
		return SIP_UPDATE;
	case MAX_METHODS:
		return SIP_MAX_METHODS;
	}
	*/
}

const char* SIPMsgHelper::transName(UINT type)
{
	if(type>=TRANS_UNKNOWN && type<TRANS_MAX)
	{
		return siptransname[type];
	}
	else
	{
		return siptransname[TRANS_MAX];
	}
}

const char* SIPMsgHelper::transName(SIPTRANSTYPE type)
{
	return siptransname[type];
}

SIPTRANSTYPE SIPMsgHelper::transType(resip::TransportType type)
{
	int tmp;
	tmp=type;
	return (SIPTRANSTYPE)tmp;
}

//Compute DialogId
CStr SIPMsgHelper::computeDialogId(const char* from, const char* to, const char* callid)
{
	CStr dialog;
	dialog<<from;
	dialog<<"+";
	dialog<<to;
	dialog<<"+";
	dialog<<callid;
	
	CMD5 md5;
	CStr dialogid((const char*)md5.GetDigest((UCHAR*)dialog.c_str(), dialog.length(), 1)); //MD5
	return dialogid;
}

CStr SIPMsgHelper::computeDialogId(resip::SipMessage* msg)
{
	if(msg!=NULL)
	return computeDialogId(msg->header(h_From).uri().user().c_str(), msg->header(h_To).uri().user().c_str(), msg->header(h_CallID).value().c_str());
}

void SIPMsgHelper::addMsgBody(PTUniNetMsg uniMsg, resip::SipMessage* msg)
{
	TSipMsg* sipbody=NULL;
	if(uniMsg==NULL || msg==NULL) return;
	if(uniMsg->hasMsgBody())
	{
		if(sipbody=dynamic_cast<TSipMsg*>(uniMsg->msgBody))
		if(sipbody->sipMsg!=NULL)
		{
			delete sipbody->sipMsg;
			sipbody->sipMsg=NULL;
		}
		else
		delete uniMsg->msgBody;
	}

	if(sipbody==NULL)
	{
		sipbody = new TSipMsg();
		uniMsg->msgBody = sipbody;
		uniMsg->setMsgBody();
	}
	sipbody->sipMsg=msg;
}

void SIPMsgHelper::addCtrlMsgHdr(PTUniNetMsg uniMsg, resip::SipMessage* msg)
{
	TSipCtrlMsg* sipctrl=NULL;
	if(uniMsg==NULL || msg==NULL) return;
	if(uniMsg->hasCtrlMsgHdr())
	{
		if(sipctrl=dynamic_cast<TSipCtrlMsg*>(uniMsg->ctrlMsgHdr))
		{
			if(!sipctrl) delete uniMsg->ctrlMsgHdr;
		}
	}
	if(sipctrl==NULL)
	{
		sipctrl = new TSipCtrlMsg();
		uniMsg->ctrlMsgHdr=sipctrl;
		uniMsg->setCtrlMsgHdr();
	}
	sipctrl->transid = msg->getReceivedTransportTuple().getFlowKey();
	sipctrl->method =  methodType(msg->method());
	sipctrl->dialogid = computeDialogId(msg);
	if(msg->isRequest())
	{
		sipctrl->status=0;
		sipctrl->rsp_method=sipctrl->method;
		if(sipctrl->method==SIP_NOTIFY && msg->exists(h_SubscriptionState))
		{
			//sub_status填写状态，0=NULL，1=Pending，2=Active，3=Terminated
            CStr st=msg->header(h_SubscriptionState).value().c_str();
            if(st == resip::Symbols::Pending)
			{
				sipctrl->sub_status=1;
			}
			else if( st ==  resip::Symbols::Active)
			{
				sipctrl->sub_status=2;
			}
			else if( st == resip::Symbols::Terminated)
			{
				sipctrl->sub_status=3;
			}
		}
		else
			sipctrl->sub_status=0;
	}
	else
	{
		sipctrl->method=SIP_RESPONSE;
		//响应，填写响应码，响应应答的方法名
		sipctrl->status = msg->header(h_StatusLine).responseCode(); //0-request >0-response code
		if(msg->exists(h_CSeq))
		{
			//rsp_method取值为响应是针对那个请求
			sipctrl->rsp_method =  methodType(msg->header(h_CSeq).method());
		}
	}
	
	if(msg->exists(h_Requires))
	{
		//如果存在Require，且包含100rel，则设置needprack=1
		if(msg->header(h_Requires).find(Token(resip::Symbols::C100rel)))
		{
			sipctrl->needprack =  1;
		}
	}
}

resip::SipMessage*  SIPMsgHelper::getSipMsgBody(PTUniNetMsg unimsg)
{
	if(!unimsg) return NULL;
	if(!unimsg->hasMsgBody()) return NULL;  

	TSipMsg* sipbody=NULL;
	sipbody=dynamic_cast<TSipMsg*>(unimsg->msgBody);
	if(!sipbody) return NULL;
	
	return sipbody->sipMsg;
}



BOOL SIPMsgHelper::buildCredential(const resip::SipMessage* msg, CSIPCredentials& Credentials)
{
	if(!msg) return FALSE;
	resip::Auth auth;

	switch(msg->header(h_RequestLine).getMethod())
	{
	case resip::REFER:
	case resip::INVITE:
	case resip::SUBSCRIBE:
	case resip::PUBLISH:
	case resip::MESSAGE:
		if(msg->exists(h_ProxyAuthorizations)&&(!msg->header(h_ProxyAuthorizations).empty())) 
		{
			auth=msg->header(h_ProxyAuthorizations).front(); 
			break;
		}
		else
		{
			Credentials.valid=FALSE;
			return FALSE;
		}	

	case resip::REGISTER:
		if(msg->exists(h_Authorizations)&&(!msg->header(h_Authorizations).empty()) ) 
		{
			auth=msg->header(h_Authorizations).front(); 
			if(strcmp(auth.param(p_response).c_str(),"") ==0)
			return FALSE;
			break;
		}
		else
		{
			Credentials.valid=FALSE;
			return FALSE;
		}
	default:
		Credentials.valid=FALSE;
		return FALSE;
	}

	
	
	Credentials.SetAlgorithm(CSIPCredentials::MD5);
	Credentials.SetMsgType(msg->header(h_RequestLine).getMethod());
	Credentials.SetNonce(auth.param(p_nonce).c_str()); 
	Credentials.SetRealm(auth.param(p_realm).c_str()); 
	Credentials.SetResponse(auth.param(p_response).c_str()); 
	Credentials.SetURI(auth.param(p_uri).c_str()); 
	Credentials.SetNonceCount(auth.param(p_nc).c_str());
	Credentials.SetCNonce(auth.param(p_cnonce).c_str());
	Credentials.SetUserName(auth.param(p_username).c_str());
	Credentials.SetQop(auth.param(p_qop).c_str());
	Credentials.SetOpaque(auth.param(p_opaque).c_str());
	Credentials.valid=TRUE;
	return TRUE;
}


CStr SIPMsgHelper::brief(const resip::SipMessage* sipmsg)
{
	CStr brief;
	if(sipmsg!=NULL)
	{
		if(sipmsg->isResponse())
		{
			brief<<"RSP:"<<sipmsg->header(h_StatusLine).responseCode()<<","<<sipmsg->header(h_StatusLine).reason().c_str()<<"/";
			brief<<"CSeq:"<<sipmsg->header(h_CSeq).sequence()<<","<<sipmsg->methodStr().c_str()<<"/";
		}
		else if(sipmsg->isRequest())
		{
			brief<<"REQ:"<<sipmsg->methodStr().c_str()<<",";
			brief<<sipmsg->header(h_RequestLine).uri().user().c_str()<<"@"<<sipmsg->header(h_RequestLine).uri().host().c_str()<<":"<<sipmsg->header(h_RequestLine).uri().port();
			if(sipmsg->header(h_RequestLine).uri().exists(p_transport)) brief<<";"<<sipmsg->header(h_RequestLine).uri().param(p_transport).c_str();
			brief<<"/";
			brief<<"CSeq:"<<sipmsg->header(h_CSeq).sequence()<<"/";
		}
		brief<<"To:"<<sipmsg->header(h_To).uri().user().c_str()<<"/";
		brief<<"From:"<<sipmsg->header(h_From).uri().user().c_str()<<"/";
		brief<<"CallID:"<<sipmsg->header(h_CallID).value().c_str()<<"/";
		if(sipmsg->exists(h_Vias) && !sipmsg->header(h_Vias).empty())
		{
			brief<<"TVia:"<<sipmsg->header(h_Vias).front().sentHost().c_str()<<":"<<sipmsg->header(h_Vias).front().sentPort()<<","<<sipmsg->header(h_Vias).front().transport().c_str();
			if(sipmsg->header(h_Vias).front().exists(p_received)) brief<<",r="<<sipmsg->header(h_Vias).front().param(p_received).c_str();
			if(sipmsg->header(h_Vias).front().exists(p_rport)) brief<<",p="<<sipmsg->header(h_Vias).front().param(p_rport).port();
			brief<<"/";
		}
		if(sipmsg->exists(h_Routes) && (!sipmsg->header(h_Routes).empty()))
		{
			brief<<"TRoute:"<<sipmsg->header(h_Routes).front().uri().user().c_str()<<"@"<<sipmsg->header(h_Routes).front().uri().host().c_str()<<":"<<sipmsg->header(h_Routes).front().uri().port();
			if(sipmsg->header(h_Routes).front().uri().exists(p_transport)) brief<<";"<<sipmsg->header(h_Routes).front().uri().param(p_transport).c_str();
			brief<<"/";
		}
		if((sipmsg->exists(h_RecordRoutes)))
		{
			brief<<"Record-Route:"<<sipmsg->header(h_RecordRoutes).front().uri().user().c_str()<<"@"<<sipmsg->header(h_RecordRoutes).front().uri().host().c_str()<<":"<<sipmsg->header(h_RecordRoutes).front().uri().port();
			if(sipmsg->header(h_RecordRoutes).front().uri().exists(p_transport)) brief<<";"<<sipmsg->header(h_RecordRoutes).front().uri().param(p_transport).c_str()<<"/";
			brief<<"/";			
		}
		if((sipmsg->exists(h_Contacts)) && (!sipmsg->header(h_Contacts).empty()))
		{
			brief<<"Contact:"<<sipmsg->header(h_Contacts).front().uri().user().c_str()<<"@"<<sipmsg->header(h_Contacts).front().uri().host().c_str()<<":"<<sipmsg->header(h_Contacts).front().uri().port();
			if(sipmsg->header(h_Contacts).front().uri().exists(p_transport)) brief<<";"<<sipmsg->header(h_Contacts).front().uri().param(p_transport).c_str();
			brief<<"/";
		}
	}
	return brief;
}
