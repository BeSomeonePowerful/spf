#if !defined __MSGDEF_HTTPMSG_H
#define __MSGDEF_HTTPMSG_H

#include "msgdef.h"
#include "tbnet.h" 

using namespace tbnet;
_CLASSDEF(CGFSM);
_CLASSDEF(THttpCtrlMsg);
_CLASSDEF(THttpMsgReq);

// TMsgType
const int HTTP_MESSAGE_TYPE = 31;

// THTTPMsgReq
const int HTTP_MESSAGE_REQ = 3100;

// THTTPMsgResp
const int HTTP_MESSAGE_RSP = 3101;

class MSGDEF_HTTP_H_IDName
{
public:
	static const char* n(int id, const char* defaultName)
	{
		switch(id)
		{
			case HTTP_MESSAGE_REQ: return "HTTP_MESSAGE_REQ";
			case HTTP_MESSAGE_RSP: return "HTTP_MESSAGE_RSP";
			default: return defaultName;
		};
	};
};


class THttpCtrlMsg:public TCtrlMsg
{
	public:
		CStr           transid;
		INT            status;
		CStr           dialogid;
	//	const char* validPara() { return "transid,method,status,dialogid"; }

		THttpCtrlMsg(){};

		CHAR*          getMsgName(){ return (char*)"THttpCtrlMsg";};
		THttpCtrlMsg    &operator=(const THttpCtrlMsg &r);
		PTCtrlMsg      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

		void           print(ostrstream& st);
		const CHAR*    getMsgNameById(int id){ return MSGDEF_HTTP_H_IDName::n(id, "THttpCtrlMsg");};
		int            getFieldValue(const char** p, int& type, CStr& value);
		int            setFieldValue(const char** p, int& type, CStr& value);
};

class THttpMsg : public TMsgBody
{
public:
    HttpMessage *hMsg;
    THttpMsg(){};
    THttpMsg(const THttpMsg &r){ 
        hMsg = new HttpMessage(r.hMsg->getpcode());
        *hMsg = *r.hMsg;    
    };
    virtual ~THttpMsg(){ }; ////这个析构函数必须是虚的，才能保证在析构的时候能够调用子类的析构函

    CHAR*          getMsgName(){ return "THttpMsg";};
    THttpMsg    &operator=(const THttpMsg &r){
        if (this == &r) return *this;
        HttpMessage* phMsg = hMsg;
        hMsg = new HttpMessage(r.hMsg->getpcode());
        delete phMsg;
        return *this;
    };
    PTMsgBody      clone(){ };
    BOOL   operator == (TMsgPara&){};

    INT            size(){INT tmpSize = 0;      tmpSize += sizeof(hMsg);
        return tmpSize;};
    INT            encode(CHAR* &buf){};
    INT            decode(CHAR* &buf){};
    BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm){};

    void           print(ostrstream& st){ };
    HttpMessage *release() {  HttpMessage *msg= hMsg; hMsg=NULL; return msg;    }
};


//for server receive http request
class THttpMsgReq : public TMsgBody
{
public:
    HttpRequestPacket *hMsg;
    THttpMsgReq(){};
    THttpMsgReq(const THttpMsgReq &r){ hMsg = new HttpRequestPacket();
        *hMsg = *r.hMsg;    
};
    virtual ~THttpMsgReq(){	}; ////这个析构函数必须是虚的，才能保证在析构的时候能够调用子类的析构函

    CHAR*          getMsgName(){ return "THttpMsgReq";};
    THttpMsgReq    &operator=(const THttpMsgReq &r){if(this == &r)
        return *this;
        HttpRequestPacket *msg = hMsg;
        hMsg = new HttpRequestPacket(*r.hMsg);
        delete msg;
        return *this;
};
    PTMsgBody      clone(){ };
    BOOL   operator == (TMsgPara&){};

    INT            size(){INT tmpSize = 0;      tmpSize += sizeof(hMsg);
        return tmpSize;};
    INT            encode(CHAR* &buf){};
    INT            decode(CHAR* &buf){};
    BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm){};

    void           print(ostrstream& st){ };
    HttpRequestPacket *release() {	HttpRequestPacket *msg= hMsg; hMsg=NULL; return msg;	}
};

//for server receive http response
class THttpMsgResp : public TMsgBody
{
public:
    HttpResponsePacket *hMsg;
    THttpMsgResp(){};
    THttpMsgResp(const THttpMsgResp &r){ hMsg = new HttpResponsePacket();
        *hMsg = *r.hMsg;    
};
    virtual ~THttpMsgResp(){	}; ////这个析构函数必须是虚的，才能保证在析构的时候能够调用子类的析构函

    CHAR*          getMsgName(){ return "THttpMsgResp";};
    THttpMsgResp    &operator=(const THttpMsgResp &r){if(this == &r)
        return *this;
        HttpResponsePacket *msg = hMsg;
        hMsg = new HttpResponsePacket(*r.hMsg);
        delete msg;
        return *this;
};
    PTMsgBody      clone(){ };
    BOOL   operator == (TMsgPara&){};

    INT            size(){INT tmpSize = 0;      tmpSize += sizeof(hMsg);
        return tmpSize;};
    INT            encode(CHAR* &buf){};
    INT            decode(CHAR* &buf){};
    BOOL           decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm){};

    void           print(ostrstream& st){ };
    HttpResponsePacket *release() {	HttpResponsePacket *msg= hMsg; hMsg=NULL; return msg;	}
};


#endif
