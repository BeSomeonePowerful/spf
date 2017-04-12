/*****************************************************************************
 * msgdef_mqtt.h
 * It is a message declaration head file.
 * 
 * Note: This file is created automatically by msg compiler tool. 
 *       Please do not modify it.
 * 
 * Created at Fri Sep 23 21:28:29 2016
.
 * 
 ******************************************************************************/
#ifndef __MSGDEF_MQTT_H_
#define __MSGDEF_MQTT_H_

#include "comtypedef.h"
#include "comtypedef_vchar.h"
#include "msgdef_uninet.h"
#include "tinyxml.h"
#include "xmlmsgconvertor.h"
#include "clist.h"

_CLASSDEF(CGFSM);


#include <time.h>
#include <strstream>
using namespace std;

const INT MAX_TOPICS           = 30;

// TMqttConnect
const TUniNetMsgName MQTT_CONNECT = 2601;

// TMqttConnAck
const TUniNetMsgName MQTT_CONNACK = 2602;

// TMqttPublish
const TUniNetMsgName MQTT_PUBLISH = 2603;

// TMqttSubscribe
const TUniNetMsgName MQTT_SUBSCRIBE = 2604;

// TMqttUnsubscribe
const TUniNetMsgName MQTT_UNSUBSCRIBE = 2605;

// TMqttSubAck
const TUniNetMsgName MQTT_SUBACK = 2606;

// TMqttUnsubAck
const TUniNetMsgName MQTT_UNSUBACK = 2607;

// TMqttDisconnect
const TUniNetMsgName MQTT_DISCONNECT = 2608;

class MSGDEF_MQTT_H_IDName
{
public:
	static const char* n(int id, const char* defaultName)
	{
		switch(id)
		{
			case MQTT_CONNECT: return "MQTT_CONNECT";
			case MQTT_CONNACK: return "MQTT_CONNACK";
			case MQTT_PUBLISH: return "MQTT_PUBLISH";
			case MQTT_SUBSCRIBE: return "MQTT_SUBSCRIBE";
			case MQTT_UNSUBSCRIBE: return "MQTT_UNSUBSCRIBE";
			case MQTT_SUBACK: return "MQTT_SUBACK";
			case MQTT_UNSUBACK: return "MQTT_UNSUBACK";
			case MQTT_DISCONNECT: return "MQTT_DISCONNECT";
			default: return defaultName;
		};
	};
};


_CLASSDEF(TMqttCtrlMsg)
class TMqttCtrlMsg:public TCtrlMsg
{
	public:
		UINT           cid;
		const char* validPara() { return "cid"; }

		inline         TMqttCtrlMsg();

		TMqttCtrlMsg   &operator=(const TMqttCtrlMsg &r);
		PTCtrlMsg      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser);

		void           print(ostrstream& st);
		virtual UINT      getSessionId() { return cid; } 
		virtual void      setSessionId(UINT sid) { cid=sid; optionSet |=session_flag; } 

		const CHAR*    getMsgNameById(int id){ return MSGDEF_MQTT_H_IDName::n(id, "TMqttCtrlMsg");};
		int            getFieldValue(const char** p, int& type, CStr& value);
		int            setFieldValue(const char** p, int& type, CStr& value);
};


_CLASSDEF(TMqttConnect)
class TMqttConnect:public TMsgBody
{
	public:
		CStr           clientName;
		CStr           userName;
		void setUserName(CStr& _userName) { userName = _userName; optionSet |= userName_flag; }
		bool p_userName() { return optionSet & userName_flag; }
		CStr           password;
		void setPassword(CStr& _password) { password = _password; optionSet |= password_flag; }
		bool p_password() { return optionSet & password_flag; }
		const char* validPara() { return "clientName,userName,password"; }

		enum
		{
			userName_flag             = 0x00000001,
			password_flag             = userName_flag<<1
		};

		inline         TMqttConnect();

		TMqttConnect   &operator=(const TMqttConnect &r);
		PTMsgBody      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser);

		void           print(ostrstream& st);
		const CHAR*    getMsgNameById(int id){ return MSGDEF_MQTT_H_IDName::n(id, "TMqttConnect");};
		int            getFieldValue(const char** p, int& type, CStr& value);
		int            setFieldValue(const char** p, int& type, CStr& value);
};


_CLASSDEF(TMqttConnAck)
class TMqttConnAck:public TMsgBody
{
	public:
		UINT           code;
		const char* validPara() { return "code"; }

		inline         TMqttConnAck();

		TMqttConnAck   &operator=(const TMqttConnAck &r);
		PTMsgBody      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser);

		void           print(ostrstream& st);
		const CHAR*    getMsgNameById(int id){ return MSGDEF_MQTT_H_IDName::n(id, "TMqttConnAck");};
		int            getFieldValue(const char** p, int& type, CStr& value);
		int            setFieldValue(const char** p, int& type, CStr& value);
};


_CLASSDEF(TMqttPublish)
class TMqttPublish:public TMsgBody
{
	public:
		CStr           clientName;
		CStr           topic;
		CStr           content;
		const char* validPara() { return "clientName,topic,content"; }

		inline         TMqttPublish();

		TMqttPublish   &operator=(const TMqttPublish &r);
		PTMsgBody      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser);

		void           print(ostrstream& st);
		const CHAR*    getMsgNameById(int id){ return MSGDEF_MQTT_H_IDName::n(id, "TMqttPublish");};
		int            getFieldValue(const char** p, int& type, CStr& value);
		int            setFieldValue(const char** p, int& type, CStr& value);
};


_CLASSDEF(TMqttSubscribe)
class TMqttSubscribe:public TMsgBody
{
	public:
		CStr           clientName;
		UINT           msgId;
		CList<CStr>    topic;
		const char* validPara() { return "clientName,msgId,topic[]"; }

		inline         TMqttSubscribe();

		TMqttSubscribe &operator=(const TMqttSubscribe &r);
		PTMsgBody      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser);

		void           print(ostrstream& st);
		const CHAR*    getMsgNameById(int id){ return MSGDEF_MQTT_H_IDName::n(id, "TMqttSubscribe");};
		int            getFieldValue(const char** p, int& type, CStr& value);
		int            setFieldValue(const char** p, int& type, CStr& value);
};


_CLASSDEF(TMqttUnsubscribe)
class TMqttUnsubscribe:public TMsgBody
{
	public:
		CStr           clientName;
		UINT           msgId;
		CList<CStr>    topic;
		const char* validPara() { return "clientName,msgId,topic[]"; }

		inline         TMqttUnsubscribe();

		TMqttUnsubscribe &operator=(const TMqttUnsubscribe &r);
		PTMsgBody      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser);

		void           print(ostrstream& st);
		const CHAR*    getMsgNameById(int id){ return MSGDEF_MQTT_H_IDName::n(id, "TMqttUnsubscribe");};
		int            getFieldValue(const char** p, int& type, CStr& value);
		int            setFieldValue(const char** p, int& type, CStr& value);
};


_CLASSDEF(TMqttSubAck)
class TMqttSubAck:public TMsgBody
{
	public:
		UINT           msgId;
		UINT           code;
		const char* validPara() { return "msgId,code"; }

		inline         TMqttSubAck();

		TMqttSubAck    &operator=(const TMqttSubAck &r);
		PTMsgBody      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser);

		void           print(ostrstream& st);
		const CHAR*    getMsgNameById(int id){ return MSGDEF_MQTT_H_IDName::n(id, "TMqttSubAck");};
		int            getFieldValue(const char** p, int& type, CStr& value);
		int            setFieldValue(const char** p, int& type, CStr& value);
};


_CLASSDEF(TMqttUnsubAck)
class TMqttUnsubAck:public TMsgBody
{
	public:
		UINT           msgId;
		const char* validPara() { return "msgId"; }

		inline         TMqttUnsubAck();

		TMqttUnsubAck  &operator=(const TMqttUnsubAck &r);
		PTMsgBody      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser);

		void           print(ostrstream& st);
		const CHAR*    getMsgNameById(int id){ return MSGDEF_MQTT_H_IDName::n(id, "TMqttUnsubAck");};
		int            getFieldValue(const char** p, int& type, CStr& value);
		int            setFieldValue(const char** p, int& type, CStr& value);
};


_CLASSDEF(TMqttDisconnect)
class TMqttDisconnect:public TMsgBody
{
	public:
		CStr           clientName;
		const char* validPara() { return "clientName"; }

		inline         TMqttDisconnect();

		TMqttDisconnect &operator=(const TMqttDisconnect &r);
		PTMsgBody      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser);

		void           print(ostrstream& st);
		const CHAR*    getMsgNameById(int id){ return MSGDEF_MQTT_H_IDName::n(id, "TMqttDisconnect");};
		int            getFieldValue(const char** p, int& type, CStr& value);
		int            setFieldValue(const char** p, int& type, CStr& value);
};


//////////////////////////////////////////////////////
//         the implementations of inline function      
/////////////////////////////////////////////////////////
inline TMqttCtrlMsg::TMqttCtrlMsg()
{
	cid                       = 1;
}

inline TMqttConnect::TMqttConnect()
{
	optionSet                 = 0;

}

inline TMqttConnAck::TMqttConnAck()
{
	code                      = 0;
}

inline TMqttPublish::TMqttPublish()
{
}

inline TMqttSubscribe::TMqttSubscribe()
{
	msgId                     = 0;
}

inline TMqttUnsubscribe::TMqttUnsubscribe()
{
	msgId                     = 0;
}

inline TMqttSubAck::TMqttSubAck()
{
	msgId                     = 0;
	code                      = 0;
}

inline TMqttUnsubAck::TMqttUnsubAck()
{
	msgId                     = 0;
}

inline TMqttDisconnect::TMqttDisconnect()
{
}


#endif
 
