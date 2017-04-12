/*****************************************************************************
 * msgdatadef.h
 * It is a message declaration head file.
 * 
 * Note: This file is created automatically by msg compiler tool. 
 *       Please do not modify it.
 * 
 * Created at Wed Feb 15 14:55:48 2017
.
 * 
 ******************************************************************************/
#ifndef __MSGDATADEF_H_
#define __MSGDATADEF_H_

#include "comtypedef.h"
#include "comtypedef_vchar.h"
#include "tinyxml.h"
#include "xmlmsgconvertor.h"

_CLASSDEF(CGFSM);


#include <time.h>
#include <strstream>
using namespace std;


class MSGDATADEF_H_IDName
{
public:
	static const char* n(int id, const char* defaultName)
	{
		switch(id)
		{
			default: return defaultName;
		};
	};
};


_CLASSDEF(TMsgAddress)
class TMsgAddress:public TMsgPara
{
	public:
		UINT           logAddr;
		UINT           phyAddr;
		UINT           taskInstID;
		const char* validPara() { return "logAddr,phyAddr,taskInstID"; }

		inline         TMsgAddress();

		TMsgAddress    &operator=(const TMsgAddress &r);
		PTMsgPara      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser);

		void           print(ostrstream& st);
		TMsgAddress(CStr& str)
		{
			CStr tmp[3];
			if(str.split(tmp,3,":")==3)
			{
				logAddr=tmp[0].toInt();
				phyAddr=tmp[1].toInt();
				taskInstID=tmp[2].toInt();
			}	
		}
		BOOL encodeCStr(CStr& buf)
		{
			buf<<logAddr<<"_"<<phyAddr<<"_"<<taskInstID;
			return TRUE;
		}
		BOOL decodeCStr(CStr& buf)
		{
			CStr tmp[3];
			if(buf.split(tmp,3,"_")==3)
			{
				logAddr=tmp[0].toInt();
				phyAddr=tmp[1].toInt();
				taskInstID=tmp[2].toInt();
				return TRUE;
			}
			return FALSE;
		}
	
		const CHAR*    getMsgNameById(int id){ return MSGDATADEF_H_IDName::n(id, "TMsgAddress");};
		int            getFieldValue(const char** p, int& type, CStr& value);
		int            setFieldValue(const char** p, int& type, CStr& value);
};


_CLASSDEF(TURI)
class TURI:public TMsgPara
{
	public:
		CStr           uri;
		CStr           schema;
		CStr           user;
		CStr           password;
		CStr           domain;
		CStr           host;
		INT            port;
		const char* validPara() { return "uri,schema,user,password,domain,host,port"; }

		inline         TURI();

		TURI           &operator=(const TURI &r);
		PTMsgPara      clone();
		BOOL           operator == (TMsgPara&);

		INT            size();
		INT            encode(CHAR* &buf);
		INT            decode(CHAR* &buf);
		BOOL           decodeFromXML(TiXmlHandle& xmlParser);

		void           print(ostrstream& st);
		TURI(CStr& str)
		{
			setUri(str.c_str());
		}
		void  setUri(const char* _uri)
		{
			uri.clear();
			uri=_uri;
			CStr _schema[2];
			if(uri.split(_schema,2,"://")==2)  schema=_schema[0];
			else _schema[1]=uri;
			
			CStr _user[2];
			if(_schema[1].split(_user,2,"@")==2)
			{
				CStr _password[2];
				if(_user[0].split(_password,2,":")==2)
				{
					user=_password[0];
					password=_password[1];
				}
				else
				{
					user=_user[0];
				}
				domain=_user[1];
			}
			else domain=_schema[1];
			
			CStr _host[2];
			if(domain.split(_host,2,":")==2) 
			{
				host=_host[0];
				port=_host[1].toInt();
			}
			else
			{
				host=domain;
			}	
		}

		void  setSchema(const char* st) { schema.clear(); schema=st; resetUri(); }
		void  setUser(const char* _user) { user.clear();  user=_user; resetUri(); }
		void  setPassword(const char* _password) {password.clear(); password=_password; resetUri(); }
		void  setHost(const char* _host) {host.clear(); host=_host; resetUri(); }
		void  setPort(int _port) { 	port=_port; resetUri(); }
		void  setDomain(const char* _domain)
		{
			domain.clear();
			domain=_domain;
			host.clear();
			port=0;
			uri.clear();
			domain.clear();
			if(schema.length()) uri<<schema<<"://";
			if(user.length()) uri<<user;
			if(password.length()) uri<<":"<<password;
			if(domain.length())
			{
				uri<<"@"<<domain;
				CStr tmp[2];
				if(domain.split(tmp,2,":")==2)
				{
					host=tmp[0];
					port=tmp[1].toInt();
				}
				else
				{
					host=domain;
				}
			}	
		}	
		
		void resetUri()
		{
			uri.clear();
			domain.clear();
			if(schema.length()) uri<<schema<<"://";
			if(user.length()) uri<<user;
			if(password.length()) uri<<":"<<password;
			if(host.length())
			{
				uri<<"@"<<host;
				domain<<host;
			}
			if(port)
			{
				uri<<":"<<port;
				domain<<port;
			}	
		}

		const char* c_str() {	return uri.c_str(); }
		
		BOOL  encodeCStr(CStr& buf) { buf<<uri; return TRUE; }
		BOOL  decodeCStr(CStr& buf) { uri=buf; resetUri(); return TRUE; }
	
		const CHAR*    getMsgNameById(int id){ return MSGDATADEF_H_IDName::n(id, "TURI");};
		int            getFieldValue(const char** p, int& type, CStr& value);
		int            setFieldValue(const char** p, int& type, CStr& value);
};


//////////////////////////////////////////////////////
//         the implementations of inline function      
/////////////////////////////////////////////////////////
inline TMsgAddress::TMsgAddress()
{
	logAddr                   = 0;
	phyAddr                   = 0;
	taskInstID                = 0;
}

inline TURI::TURI()
{
	port                      = 0;
}


#endif
 
