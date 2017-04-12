/*****************************************************************************
 * msgdef_event.h
 * It is a message declaration head file.
 * 
 * Note: This file is created automatically by msg compiler tool. 
 *       Please do not modify it.
 * 
 * Created at Wed Feb 15 09:55:29 2017
.
 * 
 ******************************************************************************/
#ifndef __MSGDEF_EVENT_H_
#define __MSGDEF_EVENT_H_

#include "comtypedef.h"
#include "comtypedef_vchar.h"
#include "msgdef_uninet.h"
#include "tinyxml.h"
#include "xmlmsgconvertor.h"

_CLASSDEF(CGFSM);


#include <time.h>
#include <strstream>
using namespace std;

const INT EVENT_MESSAGE_TYPE   = 11;

// TEventMsgBody
const TUniNetMsgName EVENT_MESSAGE_REQ = 110;
const TUniNetMsgName EVENT_MESSAGE_RSP = 111;

class MSGDEF_EVENT_H_IDName
{
public:
	static const char* n(int id, const char* defaultName)
	{
		switch(id)
		{
			case EVENT_MESSAGE_REQ: return "EVENT_MESSAGE_REQ";
			case EVENT_MESSAGE_RSP: return "EVENT_MESSAGE_RSP";
			default: return defaultName;
		};
	};
};


_CLASSDEF(TEventMsgHdr)
class TEventMsgHdr:public TCtrlMsg
{
	public:
		UINT           status;
		UINT           cseq;
		CStr           observer;
		CStr           trigger;
		CStr           eid;
		const char* validPara() { return "status,cseq,observer,trigger,eid"; }

		inline         TEventMsgHdr();

		TEventMsgHdr   &operator=(const TEventMsgHdr &r);
		PTCtrlMsg      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser);

		void           print(ostrstream& st);
		const CHAR*    getMsgNameById(int id){ return MSGDEF_EVENT_H_IDName::n(id, "TEventMsgHdr");};
		int            getFieldValue(const char** p, int& type, CStr& value);
		int            setFieldValue(const char** p, int& type, CStr& value);
};


_CLASSDEF(TEventMsgBody)
class TEventMsgBody:public TMsgBody
{
	public:
		CStr           ev;
		CStr           einfo;
		const char* validPara() { return "ev,einfo"; }

		inline         TEventMsgBody();

		TEventMsgBody  &operator=(const TEventMsgBody &r);
		PTMsgBody      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser);

		void           print(ostrstream& st);
		const CHAR*    getMsgNameById(int id){ return MSGDEF_EVENT_H_IDName::n(id, "TEventMsgBody");};
		int            getFieldValue(const char** p, int& type, CStr& value);
		int            setFieldValue(const char** p, int& type, CStr& value);
};


//////////////////////////////////////////////////////
//         the implementations of inline function      
/////////////////////////////////////////////////////////
inline TEventMsgHdr::TEventMsgHdr()
{
	status                    = 0;
	cseq                      = 0;
}

inline TEventMsgBody::TEventMsgBody()
{
}


#endif
 
