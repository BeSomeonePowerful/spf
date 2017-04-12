#if !defined __MSGDEF_COAPMSG_H
#define __MSGDEF_COAPMSG_H

#include "msgdef.h"
#include "pdu.h" 


_CLASSDEF(CGFSM);
_CLASSDEF(TCoAPCtrlMsg);
_CLASSDEF(TCoAPMsg);

// TMsgType
const int COAP_MESSAGE_TYPE = 32;

// TCoAPMsgReq
const int COAP_MESSAGE_REQ = 3200;

// TCoAPMsgResp
const int COAP_MESSAGE_RSP = 3201;

class MSGDEF_COAP_H_IDName
{
public:
	static const char* n(int id, const char* defaultName)
	{
		switch(id)
		{
			case COAP_MESSAGE_REQ: return "COAP_MESSAGE_REQ";
			case COAP_MESSAGE_RSP: return "COAP_MESSAGE_RSP";
			default: return defaultName;
		};
	};
};


class TCoAPCtrlMsg:public TCtrlMsg
{
	public:
		UINT           transid;
		UINT		   msgtype;	
		UINT           status;
		CStr           uri;

		TCoAPCtrlMsg(){};

		CHAR*          getMsgName(){ return (char*)"TCoAPCtrlMsg";};
		TCoAPCtrlMsg    &operator=(const TCoAPCtrlMsg &r);
		PTCtrlMsg      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

		void           print(ostrstream& st);
		const CHAR*    getMsgNameById(int id){ return MSGDEF_HTTP_H_IDName::n(id, "TCoAPCtrlMsg");};
		int            getFieldValue(const char** p, int& type, CStr& value);
		int            setFieldValue(const char** p, int& type, CStr& value);
};

class TCoAPMsg : public TMsgBody
{
public:
    TCoAPPDU *coap;
    TCoAPMsg(){};
    TCoAPMsg(const TCoAPMsg &r){ 
        coap = new TCoAPPDU(r.coap->getpcode());
        *coap = *r.coap;    
    };
    virtual ~TCoAPMsg(){ }; ////这个析构函数必须是虚的，才能保证在析构的时候能够调用子类的析构函

    CHAR*          getMsgName(){ return "TCoAPMsg";};
    TCoAPMsg    &operator=(const TCoAPMsg &r){
        if (this == &r) return *this;
        TCoAPPDU* phMsg = coap;
        coap = new TCoAPPDU(r.coap->getpcode());
        delete phMsg;
        return *this;
    };
    PTMsgBody      clone(){ };
    BOOL   operator == (TMsgPara&){};

    INT            size(){INT tmpSize = 0;      tmpSize += sizeof(coap);
        return tmpSize;};
    INT            encode(CHAR* &buf){};
    INT            decode(CHAR* &buf){};
    BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm){};

    void           print(ostrstream& st){ };
    TCoAPPDU *release() {  TCoAPPDU *msg= coap; coap=NULL; return msg;    }
};


#endif
