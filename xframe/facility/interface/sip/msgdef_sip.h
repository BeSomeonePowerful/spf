/*****************************************************************************
 * msgdef_sip.h
 * It is a message declaration head file.
 * 
 * Note: This file is created automatically by msg compiler tool. 
 *       Please do not modify it.
 * 
 * Created at Sun Sep 25 23:24:49 2016
.
 * 
 ******************************************************************************/
#ifndef __MSGDEF_SIP_H_
#define __MSGDEF_SIP_H_

#include "comtypedef.h"
#include "comtypedef_vchar.h"
#include "msgdef_uninet.h"
#include "tinyxml.h"
#include "xmlmsgconvertor.h"

_CLASSDEF(CGFSM);

#include "msgconstdef_sip.h"
#include "msghelper_sip.h"
#include "msgdef.h"
#include "msgdatadef.h"
#include "msgutil.h"
#include "msgnamedef.h"

//mdf
#include "resip/stack/SipMessage.hxx"

#include <time.h>
#include <strstream>
using namespace std;

const INT SIP_MESSAGE_TYPE     = 21;

// TSipMsg
const TUniNetMsgName SIP_MESSAGE_REQ = 210;
const TUniNetMsgName SIP_MESSAGE_RSP = 211;

class MSGDEF_SIP_H_IDName
{
public:
	static const char* n(int id, const char* defaultName)
	{
		switch(id)
		{
			case SIP_MESSAGE_REQ: return "SIP_MESSAGE_REQ";
			case SIP_MESSAGE_RSP: return "SIP_MESSAGE_RSP";
			default: return defaultName;
		};
	};
};

//mdf transid, �����TCP/TLS�����ӣ�ָ�����ӵ�fd�������udp��ָ������˿ڵ�fd
_CLASSDEF(TSipCtrlMsg)
class TSipCtrlMsg:public TCtrlMsg
{
	public:
		ULONG          transid;
		SIPMETHODTYPE  method;
		INT            status;
		SIPMETHODTYPE  rsp_method;
		INT            needprack;
		INT            sub_status;
		CStr           dialogid;
		const char* validPara() { return "transid,method,status,rsp_method,needprack,sub_status,dialogid"; }

		inline         TSipCtrlMsg();

		TSipCtrlMsg    &operator=(const TSipCtrlMsg &r);
		PTCtrlMsg      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser);

		void           print(ostrstream& st);
		const CHAR*    getMsgNameById(int id){ return MSGDEF_SIP_H_IDName::n(id, "TSipCtrlMsg");};
		int            getFieldValue(const char** p, int& type, CStr& value);
		int            setFieldValue(const char** p, int& type, CStr& value);
};


_CLASSDEF(TSipMsg)
class TSipMsg:public TMsgBody
{
	public:
                //mdf �޸�CStrΪָ��
		//CStr           sipMsg;
                resip::SipMessage* sipMsg;
		const char* validPara() { return "sipMsg"; }

		inline         TSipMsg();
		TSipMsg(const TSipMsg&);

	        //mdf ���ָ����޶�
                virtual ~TSipMsg(){ if(sipMsg!=NULL) delete sipMsg; sipMsg == NULL;}; ////�������������������ģ����ܱ�֤��������ʱ���ܹ����������������
	        //�ͷ�sipMsg ָ�룬TUniNetMsg �ͷŵ�ʱ�򲻻�ɾ����װ�� SipMessage ָ��
	        resip::SipMessage*   release() { resip::SipMessage* msg=sipMsg; sipMsg=NULL; return msg; }

		TSipMsg        &operator=(const TSipMsg &r);
		PTMsgBody      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser);

		void           print(ostrstream& st);
		const CHAR*    getMsgNameById(int id){ return MSGDEF_SIP_H_IDName::n(id, "TSipMsg");};
		int            getFieldValue(const char** p, int& type, CStr& value);
		int            setFieldValue(const char** p, int& type, CStr& value);
};


//////////////////////////////////////////////////////
//         the implementations of inline function      
/////////////////////////////////////////////////////////
inline TSipCtrlMsg::TSipCtrlMsg()
{
	transid                   = 0;
	method                    = SIPMETHODTYPE(0);
	status                    = 0;
	rsp_method                = SIPMETHODTYPE(0);
	needprack                 = 0;
	sub_status                = 0;
}

inline TSipMsg::TSipMsg()
{
        sipMsg=NULL;
}


#endif
 
