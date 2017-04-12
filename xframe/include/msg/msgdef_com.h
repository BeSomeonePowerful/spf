/*****************************************************************************
 * msgdef_com.h
 * It is a message declaration head file.
 * 
 * Note: This file is created automatically by msg compiler tool. 
 *       Please do not modify it.
 * 
 * Created at Wed Feb 15 09:53:19 2017
.
 * 
 ******************************************************************************/
#ifndef __MSGDEF_COM_H_
#define __MSGDEF_COM_H_

#include "comtypedef.h"
#include "comtypedef_vchar.h"
#include "msgdef_uninet.h"
#include "tinyxml.h"
#include "xmlmsgconvertor.h"

_CLASSDEF(CGFSM);


#include <time.h>
#include <strstream>
using namespace std;

const INT COM_MESSAGE_TYPE     = 10;
const INT MAX_MSG_ARGV_NUM     = 5;

// TComMsgBody
const TUniNetMsgName COM_MESSAGE_REQ = 100;
const TUniNetMsgName COM_MESSAGE_RSP = 101;
const TUniNetMsgName COM_MESSAGE_TMP = 102;

class MSGDEF_COM_H_IDName
{
public:
	static const char* n(int id, const char* defaultName)
	{
		switch(id)
		{
			case COM_MESSAGE_REQ: return "COM_MESSAGE_REQ";
			case COM_MESSAGE_RSP: return "COM_MESSAGE_RSP";
			case COM_MESSAGE_TMP: return "COM_MESSAGE_TMP";
			default: return defaultName;
		};
	};
};


_CLASSDEF(TComMsgHdr)
class TComMsgHdr:public TCtrlMsg
{
	public:
		INT            status;
		INT            pri;
		INT            facility;
		INT            argc;
		CStr           argv[MAX_MSG_ARGV_NUM];
		const char* validPara() { return "status,pri,facility,argc,argv[]"; }

		inline         TComMsgHdr();

		TComMsgHdr     &operator=(const TComMsgHdr &r);
		PTCtrlMsg      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser);

		void           print(ostrstream& st);
		const CHAR*    getMsgNameById(int id){ return MSGDEF_COM_H_IDName::n(id, "TComMsgHdr");};
		int            getFieldValue(const char** p, int& type, CStr& value);
		int            setFieldValue(const char** p, int& type, CStr& value);
};


_CLASSDEF(TComMsgBody)
class TComMsgBody:public TMsgBody
{
	public:
		CStr           info;
		const char* validPara() { return "info"; }

		inline         TComMsgBody();

		TComMsgBody    &operator=(const TComMsgBody &r);
		PTMsgBody      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser);

		void           print(ostrstream& st);
		const CHAR*    getMsgNameById(int id){ return MSGDEF_COM_H_IDName::n(id, "TComMsgBody");};
		int            getFieldValue(const char** p, int& type, CStr& value);
		int            setFieldValue(const char** p, int& type, CStr& value);
};


//////////////////////////////////////////////////////
//         the implementations of inline function      
/////////////////////////////////////////////////////////
inline TComMsgHdr::TComMsgHdr()
{
	status                    = 0;
	pri                       = 0;
	facility                  = 0;
	argc                      = 0;
}

inline TComMsgBody::TComMsgBody()
{
}


#endif
 
