/*****************************************************************************
 * msgdef_rsc.h
 * It is a message declaration head file.
 * 
 * Note: This file is created automatically by msg compiler tool. 
 *       Please do not modify it.
 * 
 * Created at Wed Feb 15 09:54:27 2017
.
 * 
 ******************************************************************************/
#ifndef __MSGDEF_RSC_H_
#define __MSGDEF_RSC_H_

#include "comtypedef.h"
#include "comtypedef_vchar.h"
#include "msgdef_uninet.h"
#include "tinyxml.h"
#include "xmlmsgconvertor.h"

_CLASSDEF(CGFSM);

#include "msgdatadef.h"
#include "msgutil.h"

#include <time.h>
#include <strstream>
using namespace std;

const INT RSC_MESSAGE_TYPE     = 12;

// TRscMsgBody
const TUniNetMsgName RSC_REQUEST = 120;
const TUniNetMsgName RSC_RESPONSE = 121;
const TUniNetMsgName RSC_MESSAGE = 122;

class MSGDEF_RSC_H_IDName
{
public:
	static const char* n(int id, const char* defaultName)
	{
		switch(id)
		{
			case RSC_REQUEST: return "RSC_REQUEST";
			case RSC_RESPONSE: return "RSC_RESPONSE";
			case RSC_MESSAGE: return "RSC_MESSAGE";
			default: return defaultName;
		};
	};
};


_CLASSDEF(TRscMsgHdr)
class TRscMsgHdr:public TCtrlMsg
{
	public:
		UINT           code;
		CStr           ruri;
		CStr           consumer;
		CStr           producer;
		TURI           host;
		CStr           rid;
		const char* validPara() { return "code,ruri,consumer,producer,host,rid"; }

		inline         TRscMsgHdr();

		TRscMsgHdr     &operator=(const TRscMsgHdr &r);
		PTCtrlMsg      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser);

		void           print(ostrstream& st);
	virtual UINT  getSessionId(CStr& sid) 
	{ 
		if(!hasSessionId()) return 0;
		sid=rid;
		return 1;
	}
	// in order to use sessionid
	// must setSessionId() first

		const CHAR*    getMsgNameById(int id){ return MSGDEF_RSC_H_IDName::n(id, "TRscMsgHdr");};
		int            getFieldValue(const char** p, int& type, CStr& value);
		int            setFieldValue(const char** p, int& type, CStr& value);
};


_CLASSDEF(TRscMsgBody)
class TRscMsgBody:public TMsgBody
{
	public:
		CStr           rsc;
		const char* validPara() { return "rsc"; }

		inline         TRscMsgBody();

		TRscMsgBody    &operator=(const TRscMsgBody &r);
		PTMsgBody      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser);

		void           print(ostrstream& st);
		const CHAR*    getMsgNameById(int id){ return MSGDEF_RSC_H_IDName::n(id, "TRscMsgBody");};
		int            getFieldValue(const char** p, int& type, CStr& value);
		int            setFieldValue(const char** p, int& type, CStr& value);
};


//////////////////////////////////////////////////////
//         the implementations of inline function      
/////////////////////////////////////////////////////////
inline TRscMsgHdr::TRscMsgHdr()
{
	code                      = 0;
}

inline TRscMsgBody::TRscMsgBody()
{
}


#endif
 
