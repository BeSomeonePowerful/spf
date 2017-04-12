#if !defined __MSGHELPER_SIP_H
#define __MSGHELPER_SIP_H

#include "msgconstdef_sip.h"
#include "msgdef_sip.h"
#include "resip/stack/SipMessage.hxx"
#include "resip/stack/MethodTypes.hxx"

/***************************CSIPCredentials 定义*************************/
class CSIPCredentials
{
  public:
	enum Algthm{MD5,MD5_SESS};
	bool valid;

  private:
    CStr m_UserName;
    CStr m_Realm;
    CStr m_Qop;
    CStr m_Nonce;
    CStr m_Opaque;
    CStr m_URI;
    CStr m_Response;
    Algthm m_ucAlgorithm;
    CStr m_CNonce;
    CStr m_NonceCount;
    resip::MethodTypes m_ucMsgType;
    CStr m_H;


  public:
     CSIPCredentials();

     const CStr& GetUserNameRef() const;
     CStr& GetUserNameRef();
     void SetUserName( const char* );

     const CStr& GetRealmRef() const;
     CStr& GetRealmRef();
     void SetRealm( const char* );

     const CStr& GetQopRef() const;
     void SetQop( const char* );

     const CStr& GetNonceRef() const;
     CStr& GetNonceRef();
     void SetNonce( const char* );

     const CStr& GetOpaqueRef() const;
     CStr& GetOpaqueRef();
     void SetOpaque( const char* );

     const CStr& GetURIRef() const;
     CStr& GetURIRef();
     void SetURI( const char* );

     const CStr& GetResponseRef() const;
     CStr& GetResponseRef();
     void SetResponse( const char* );

     Algthm GetAlgorithm() const;
     void SetAlgorithm( Algthm );

     const CStr& GetCNonceRef() const;
     CStr& GetCNonceRef();
     void SetCNonce( const char* );

     const CStr& GetNonceCountRef() const;
     CStr& GetNonceCountRef();
     void SetNonceCount( const char* );

    resip::MethodTypes GetMsgType() const;
     void SetMsgType( resip::MethodTypes );

     const CStr& GetHRef() const;
     CStr& GetHRef();
     void SetH( const char* );

     INT Size() const;

    void Print(ostrstream &st);

};

const char sipmethodname[18][10]={"UNKNOWN","ACK","BYE","CANCEL","INVITE","NOTIFY","OPTIONS","REFER","REGISTER","SUBSCRIBE","RESPONSE","MESSAGE","INFO","PRACK","PUBLISH","SERVICE","UPDATE","UNKNOWN"};
const char siptransname[10][8]={"DEFAULT","TLS","TCP","UDP","SCTP","DCCP","DTLS","WS","WSS","UNKNOWN"};

_CLASSDEF(SIPMsgHelper);
class SIPMsgHelper
{
public:
	
	static const char* methodName(SIPMETHODTYPE type);
	static const char* methodName(UINT type);
	static SIPMETHODTYPE methodType(resip::MethodTypes type);
	static const char* transName(SIPTRANSTYPE type);
	static const char* transName(UINT type);
	static SIPTRANSTYPE transType(resip::TransportType type);
	
	static CStr computeDialogId(const char* from, const char* to, const char* callid);
	static CStr computeDialogId(resip::SipMessage* msg);
	static void addMsgBody(PTUniNetMsg uniMsg, resip::SipMessage* msg);
	static void addCtrlMsgHdr(PTUniNetMsg uniMsg, resip::SipMessage* msg);
	static resip::SipMessage*  getSipMsgBody(PTUniNetMsg unimsg);

	static BOOL buildCredential(const resip::SipMessage* msg, CSIPCredentials& Credentials);

    static CStr brief(const resip::SipMessage* sipmsg);
};


#endif
