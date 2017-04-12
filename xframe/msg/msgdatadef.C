/*****************************************************************************
 * msgdatadef.C
 * It is an implementation file of message definition.
 * 
 * Note: This file is created automatically by msg compiler tool. 
 *       Please do not modify it.
 * 
 * Created at Wed Feb 15 14:55:48 2017
.
 * 
 ******************************************************************************/
#include "msgdatadef.h"
#include "info.h"
#include "msgutil.h"
_CLASSDEF(CGFSM);


/////////////////////////////////////////////
//           for class TMsgAddress
/////////////////////////////////////////////
PTMsgPara TMsgAddress::clone()
{
	PTMsgAddress amsg = new TMsgAddress();
	amsg->logAddr                   = logAddr;
	amsg->phyAddr                   = phyAddr;
	amsg->taskInstID                = taskInstID;
	return amsg;
}
TMsgAddress& TMsgAddress::operator=(const TMsgAddress &r)
{
	logAddr                   = r.logAddr;
	phyAddr                   = r.phyAddr;
	taskInstID                = r.taskInstID;
	return *this;
}

BOOL TMsgAddress::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TMsgAddress,msg)

	COMPARE_FORCE_INT(TMsgAddress,logAddr)
	COMPARE_FORCE_INT(TMsgAddress,phyAddr)
	COMPARE_FORCE_INT(TMsgAddress,taskInstID)

	COMPARE_END
}

INT TMsgAddress::size()
{
	INT tmpSize = 0;

	tmpSize += sizeof(INT);
	tmpSize += sizeof(INT);
	tmpSize += sizeof(INT);

	return tmpSize;
}

INT TMsgAddress::encode(CHAR* &buf)
{
	ENCODE_INT( buf , logAddr )
	ENCODE_INT( buf , phyAddr )
	ENCODE_INT( buf , taskInstID )

	return size();
}

INT TMsgAddress::decode(CHAR* &buf)
{
	DECODE_INT( logAddr, buf )
	DECODE_INT( phyAddr, buf )
	DECODE_INT( taskInstID, buf )

	return size();
}

BOOL TMsgAddress::decodeFromXML(TiXmlHandle& xmlParser)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_INT(TMsgAddress,logAddr)
	FILL_FORCE_INT(TMsgAddress,phyAddr)
	FILL_FORCE_INT(TMsgAddress,taskInstID)

	FILL_FIELD_END
}
void TMsgAddress::print(ostrstream& st)
{
	st << "TMsgAddress" << endl;
	st << getIndent() << "$logAddr    = ";
	st << logAddr << endl;
	st << getIndent() << "$phyAddr    = ";
	st << phyAddr << endl;
	st << getIndent() << "$taskInstID = ";
	st << taskInstID << endl;
}

int TMsgAddress::getFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if(fieldName == "logAddr")
	{
		value = logAddr; type = 2; return 1;
	}
	else if(fieldName == "phyAddr")
	{
		value = phyAddr; type = 2; return 1;
	}
	else if(fieldName == "taskInstID")
	{
		value = taskInstID; type = 2; return 1;
	}
	UniERROR("TMsgAddress::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}

int TMsgAddress::setFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if(fieldName == "logAddr")
	{
		logAddr = (UINT)(value.toInt()); return 1;
	}
	else if(fieldName == "phyAddr")
	{
		phyAddr = (UINT)(value.toInt()); return 1;
	}
	else if(fieldName == "taskInstID")
	{
		taskInstID = (UINT)(value.toInt()); return 1;
	}
	UniERROR("TMsgAddress::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}


/////////////////////////////////////////////
//           for class TURI
/////////////////////////////////////////////
PTMsgPara TURI::clone()
{
	PTURI amsg = new TURI();
	amsg->uri                       = uri;
	amsg->schema                    = schema;
	amsg->user                      = user;
	amsg->password                  = password;
	amsg->domain                    = domain;
	amsg->host                      = host;
	amsg->port                      = port;
	return amsg;
}
TURI& TURI::operator=(const TURI &r)
{
	uri                       = r.uri;
	schema                    = r.schema;
	user                      = r.user;
	password                  = r.password;
	domain                    = r.domain;
	host                      = r.host;
	port                      = r.port;
	return *this;
}

BOOL TURI::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TURI,msg)

	COMPARE_FORCE_VCHAR(TURI,uri)
	COMPARE_FORCE_VCHAR(TURI,schema)
	COMPARE_FORCE_VCHAR(TURI,user)
	COMPARE_FORCE_VCHAR(TURI,password)
	COMPARE_FORCE_VCHAR(TURI,domain)
	COMPARE_FORCE_VCHAR(TURI,host)
	COMPARE_FORCE_INT(TURI,port)

	COMPARE_END
}

INT TURI::size()
{
	INT tmpSize = 0;

	tmpSize += uri.size();
	tmpSize += schema.size();
	tmpSize += user.size();
	tmpSize += password.size();
	tmpSize += domain.size();
	tmpSize += host.size();
	tmpSize += sizeof(INT);

	return tmpSize;
}

INT TURI::encode(CHAR* &buf)
{
//	uri.encode(buf);
//	schema.encode(buf);
//	user.encode(buf);
//	password.encode(buf);
//	domain.encode(buf);
//	host.encode(buf);
//	ENCODE_INT( buf , port )
//	return size();
	if(uri.empty()) resetUri();
	return uri.encode(buf);
}

INT TURI::decode(CHAR* &buf)
{
//	uri.decode(buf);
//	schema.decode(buf);
//	user.decode(buf);
//	password.decode(buf);
//	domain.decode(buf);
//	host.decode(buf);
//	DECODE_INT( port, buf )
//	return size();
	CStr _uri;
	int length=_uri.decode(buf);
	if(length) setUri(_uri.c_str());
	return length;
}

BOOL TURI::decodeFromXML(TiXmlHandle& xmlParser)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_VCHAR(TURI,uri)
	FILL_FORCE_VCHAR(TURI,schema)
	FILL_FORCE_VCHAR(TURI,user)
	FILL_FORCE_VCHAR(TURI,password)
	FILL_FORCE_VCHAR(TURI,domain)
	FILL_FORCE_VCHAR(TURI,host)
	FILL_FORCE_INT(TURI,port)

	FILL_FIELD_END
}
void TURI::print(ostrstream& st)
{
	st << "TURI" << endl;
	st << getIndent() << "$uri      = ";
	st << "\""; uri.bprint(st); st << "\"" << endl;
	st << getIndent() << "$schema   = ";
	st << "\""; schema.bprint(st); st << "\"" << endl;
	st << getIndent() << "$user     = ";
	st << "\""; user.bprint(st); st << "\"" << endl;
	st << getIndent() << "$password = ";
	st << "\""; password.bprint(st); st << "\"" << endl;
	st << getIndent() << "$domain   = ";
	st << "\""; domain.bprint(st); st << "\"" << endl;
	st << getIndent() << "$host     = ";
	st << "\""; host.bprint(st); st << "\"" << endl;
	st << getIndent() << "$port     = ";
	st << port << endl;
}

int TURI::getFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if(fieldName == "uri")
	{
		value.set(uri.GetVarCharContentPoint(), uri.GetVarCharLen()); type = 4; return 1;
	}
	else if(fieldName == "schema")
	{
		value.set(schema.GetVarCharContentPoint(), schema.GetVarCharLen()); type = 4; return 1;
	}
	else if(fieldName == "user")
	{
		value.set(user.GetVarCharContentPoint(), user.GetVarCharLen()); type = 4; return 1;
	}
	else if(fieldName == "password")
	{
		value.set(password.GetVarCharContentPoint(), password.GetVarCharLen()); type = 4; return 1;
	}
	else if(fieldName == "domain")
	{
		value.set(domain.GetVarCharContentPoint(), domain.GetVarCharLen()); type = 4; return 1;
	}
	else if(fieldName == "host")
	{
		value.set(host.GetVarCharContentPoint(), host.GetVarCharLen()); type = 4; return 1;
	}
	else if(fieldName == "port")
	{
		value = port; type = 2; return 1;
	}
	UniERROR("TURI::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}

int TURI::setFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if(fieldName == "uri")
	{
		uri.set(value.c_str(), value.length()); return 1;
	}
	else if(fieldName == "schema")
	{
		schema.set(value.c_str(), value.length()); return 1;
	}
	else if(fieldName == "user")
	{
		user.set(value.c_str(), value.length()); return 1;
	}
	else if(fieldName == "password")
	{
		password.set(value.c_str(), value.length()); return 1;
	}
	else if(fieldName == "domain")
	{
		domain.set(value.c_str(), value.length()); return 1;
	}
	else if(fieldName == "host")
	{
		host.set(value.c_str(), value.length()); return 1;
	}
	else if(fieldName == "port")
	{
		port = (INT)(value.toInt()); return 1;
	}
	UniERROR("TURI::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}

