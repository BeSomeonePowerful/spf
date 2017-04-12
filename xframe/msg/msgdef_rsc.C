/*****************************************************************************
 * msgdef_rsc.C
 * It is an implementation file of message definition.
 * 
 * Note: This file is created automatically by msg compiler tool. 
 *       Please do not modify it.
 * 
 * Created at Wed Feb 15 09:54:27 2017
.
 * 
 ******************************************************************************/
#include "msgdef_rsc.h"
#include "info.h"
#include "msgutil.h"
_CLASSDEF(CGFSM);


/////////////////////////////////////////////
//           for class TRscMsgHdr
/////////////////////////////////////////////
PTCtrlMsg TRscMsgHdr::clone()
{
	PTRscMsgHdr amsg = new TRscMsgHdr();
	amsg->optionSet                 = optionSet;

	amsg->orgAddr                   = orgAddr;

	amsg->code                      = code;
	amsg->ruri                      = ruri;
	amsg->consumer                  = consumer;
	amsg->producer                  = producer;
	amsg->host                      = host;
	amsg->rid                       = rid;
	return amsg;
}
TRscMsgHdr& TRscMsgHdr::operator=(const TRscMsgHdr &r)
{
	code                      = r.code;
	ruri                      = r.ruri;
	consumer                  = r.consumer;
	producer                  = r.producer;
	host                      = r.host;
	rid                       = r.rid;
	return *this;
}

BOOL TRscMsgHdr::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TRscMsgHdr,msg)

	COMPARE_FORCE_INT(TRscMsgHdr,code)
	COMPARE_FORCE_VCHAR(TRscMsgHdr,ruri)
	COMPARE_FORCE_VCHAR(TRscMsgHdr,consumer)
	COMPARE_FORCE_VCHAR(TRscMsgHdr,producer)
	COMPARE_FORCE_NEST(TRscMsgHdr,TURI,host)
	COMPARE_FORCE_VCHAR(TRscMsgHdr,rid)

	COMPARE_END
}

INT TRscMsgHdr::size()
{
	INT tmpSize = 0;

	tmpSize += sizeof(UINT); //for optionSet

	if( optionSet & orgAddr_flag )	tmpSize += orgAddr.size();

	tmpSize += sizeof(INT);
	tmpSize += ruri.size();
	tmpSize += consumer.size();
	tmpSize += producer.size();
	tmpSize += host.size();
	tmpSize += rid.size();

	return tmpSize;
}

INT TRscMsgHdr::encode(CHAR* &buf)
{
	ENCODE_INT( buf , optionSet )

	if( optionSet & orgAddr_flag )   orgAddr.encode(buf);

	ENCODE_INT( buf , code )
	ruri.encode(buf);
	consumer.encode(buf);
	producer.encode(buf);
	host.encode(buf);
	rid.encode(buf);

	return size();
}

INT TRscMsgHdr::decode(CHAR* &buf)
{
	DECODE_INT( optionSet , buf )

	if( optionSet & orgAddr_flag )   orgAddr.decode(buf);

	DECODE_INT( code, buf )
	ruri.decode(buf);
	consumer.decode(buf);
	producer.decode(buf);
	host.decode(buf);
	rid.decode(buf);

	return size();
}

BOOL TRscMsgHdr::decodeFromXML(TiXmlHandle& xmlParser)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_INT(TRscMsgHdr,code)
	FILL_FORCE_VCHAR(TRscMsgHdr,ruri)
	FILL_FORCE_VCHAR(TRscMsgHdr,consumer)
	FILL_FORCE_VCHAR(TRscMsgHdr,producer)
	FILL_FORCE_NEST(TRscMsgHdr,TURI,host)
	FILL_FORCE_VCHAR(TRscMsgHdr,rid)

	FILL_FIELD_END
}
void TRscMsgHdr::print(ostrstream& st)
{
	st << "TRscMsgHdr" << endl;
	st << getIndent() << "orgAddr    = ";
	if( optionSet & orgAddr_flag )
	{
		orgAddr.print(st);
	}
	else
		st << "(not present)" << endl;
	st << getIndent() << "$code      = ";
	st << code << endl;
	st << getIndent() << "$ruri      = ";
	st << "\""; ruri.bprint(st); st << "\"" << endl;
	st << getIndent() << "$consumer  = ";
	st << "\""; consumer.bprint(st); st << "\"" << endl;
	st << getIndent() << "$producer  = ";
	st << "\""; producer.bprint(st); st << "\"" << endl;
	st << getIndent() << "$host      = ";
	incIndent();
	host.print(st);
	decIndent();
	st << getIndent() << "$rid       = ";
	st << "\""; rid.bprint(st); st << "\"" << endl;
}

int TRscMsgHdr::getFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if((optionSet & orgAddr_flag) && fieldName == "orgAddr") return orgAddr.getFieldValue(p, type, value);
	if(fieldName == "code")
	{
		value = code; type = 2; return 1;
	}
	else if(fieldName == "ruri")
	{
		value.set(ruri.GetVarCharContentPoint(), ruri.GetVarCharLen()); type = 4; return 1;
	}
	else if(fieldName == "consumer")
	{
		value.set(consumer.GetVarCharContentPoint(), consumer.GetVarCharLen()); type = 4; return 1;
	}
	else if(fieldName == "producer")
	{
		value.set(producer.GetVarCharContentPoint(), producer.GetVarCharLen()); type = 4; return 1;
	}
	else if(fieldName == "host")
	{
		return host.getFieldValue(p, type, value);
	}
	else if(fieldName == "rid")
	{
		value.set(rid.GetVarCharContentPoint(), rid.GetVarCharLen()); type = 4; return 1;
	}
	UniERROR("TRscMsgHdr::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}

int TRscMsgHdr::setFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if((optionSet & orgAddr_flag) && fieldName == "orgAddr") return orgAddr.setFieldValue(p, type, value);
	if(fieldName == "code")
	{
		code = (UINT)(value.toInt()); return 1;
	}
	else if(fieldName == "ruri")
	{
		ruri.set(value.c_str(), value.length()); return 1;
	}
	else if(fieldName == "consumer")
	{
		consumer.set(value.c_str(), value.length()); return 1;
	}
	else if(fieldName == "producer")
	{
		producer.set(value.c_str(), value.length()); return 1;
	}
	else if(fieldName == "host")
	{
		return host.setFieldValue(p, type, value);
	}
	else if(fieldName == "rid")
	{
		rid.set(value.c_str(), value.length()); return 1;
	}
	UniERROR("TRscMsgHdr::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}


/////////////////////////////////////////////
//           for class TRscMsgBody
/////////////////////////////////////////////
PTMsgBody TRscMsgBody::clone()
{
	PTRscMsgBody amsg = new TRscMsgBody();
	amsg->rsc                       = rsc;
	return amsg;
}
TRscMsgBody& TRscMsgBody::operator=(const TRscMsgBody &r)
{
	rsc                       = r.rsc;
	return *this;
}

BOOL TRscMsgBody::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TRscMsgBody,msg)

	COMPARE_FORCE_VCHAR(TRscMsgBody,rsc)

	COMPARE_END
}

INT TRscMsgBody::size()
{
	INT tmpSize = 0;

	tmpSize += rsc.size();

	return tmpSize;
}

INT TRscMsgBody::encode(CHAR* &buf)
{
	rsc.encode(buf);

	return size();
}

INT TRscMsgBody::decode(CHAR* &buf)
{
	rsc.decode(buf);

	return size();
}

BOOL TRscMsgBody::decodeFromXML(TiXmlHandle& xmlParser)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_VCHAR(TRscMsgBody,rsc)

	FILL_FIELD_END
}
void TRscMsgBody::print(ostrstream& st)
{
	st << "TRscMsgBody" << endl;
	st << getIndent() << "$rsc = ";
	st << "\""; rsc.bprint(st); st << "\"" << endl;
}

int TRscMsgBody::getFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if(fieldName == "rsc")
	{
		value.set(rsc.GetVarCharContentPoint(), rsc.GetVarCharLen()); type = 4; return 1;
	}
	UniERROR("TRscMsgBody::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}

int TRscMsgBody::setFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if(fieldName == "rsc")
	{
		rsc.set(value.c_str(), value.length()); return 1;
	}
	UniERROR("TRscMsgBody::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}

