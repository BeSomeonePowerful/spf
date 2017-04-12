/*****************************************************************************
 * msgdef_event.C
 * It is an implementation file of message definition.
 * 
 * Note: This file is created automatically by msg compiler tool. 
 *       Please do not modify it.
 * 
 * Created at Wed Feb 15 09:55:29 2017
.
 * 
 ******************************************************************************/
#include "msgdef_event.h"
#include "info.h"
#include "msgutil.h"
_CLASSDEF(CGFSM);


/////////////////////////////////////////////
//           for class TEventMsgHdr
/////////////////////////////////////////////
PTCtrlMsg TEventMsgHdr::clone()
{
	PTEventMsgHdr amsg = new TEventMsgHdr();
	amsg->optionSet                 = optionSet;

	amsg->orgAddr                   = orgAddr;

	amsg->status                    = status;
	amsg->cseq                      = cseq;
	amsg->observer                  = observer;
	amsg->trigger                   = trigger;
	amsg->eid                       = eid;
	return amsg;
}
TEventMsgHdr& TEventMsgHdr::operator=(const TEventMsgHdr &r)
{
	status                    = r.status;
	cseq                      = r.cseq;
	observer                  = r.observer;
	trigger                   = r.trigger;
	eid                       = r.eid;
	return *this;
}

BOOL TEventMsgHdr::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TEventMsgHdr,msg)

	COMPARE_FORCE_INT(TEventMsgHdr,status)
	COMPARE_FORCE_INT(TEventMsgHdr,cseq)
	COMPARE_FORCE_VCHAR(TEventMsgHdr,observer)
	COMPARE_FORCE_VCHAR(TEventMsgHdr,trigger)
	COMPARE_FORCE_VCHAR(TEventMsgHdr,eid)

	COMPARE_END
}

INT TEventMsgHdr::size()
{
	INT tmpSize = 0;

	tmpSize += sizeof(UINT); //for optionSet

	if( optionSet & orgAddr_flag )	tmpSize += orgAddr.size();

	tmpSize += sizeof(INT);
	tmpSize += sizeof(INT);
	tmpSize += observer.size();
	tmpSize += trigger.size();
	tmpSize += eid.size();

	return tmpSize;
}

INT TEventMsgHdr::encode(CHAR* &buf)
{
	ENCODE_INT( buf , optionSet )

	if( optionSet & orgAddr_flag )   orgAddr.encode(buf);

	ENCODE_INT( buf , status )
	ENCODE_INT( buf , cseq )
	observer.encode(buf);
	trigger.encode(buf);
	eid.encode(buf);

	return size();
}

INT TEventMsgHdr::decode(CHAR* &buf)
{
	DECODE_INT( optionSet , buf )

	if( optionSet & orgAddr_flag )   orgAddr.decode(buf);

	DECODE_INT( status, buf )
	DECODE_INT( cseq, buf )
	observer.decode(buf);
	trigger.decode(buf);
	eid.decode(buf);

	return size();
}

BOOL TEventMsgHdr::decodeFromXML(TiXmlHandle& xmlParser)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_INT(TEventMsgHdr,status)
	FILL_FORCE_INT(TEventMsgHdr,cseq)
	FILL_FORCE_VCHAR(TEventMsgHdr,observer)
	FILL_FORCE_VCHAR(TEventMsgHdr,trigger)
	FILL_FORCE_VCHAR(TEventMsgHdr,eid)

	FILL_FIELD_END
}
void TEventMsgHdr::print(ostrstream& st)
{
	st << "TEventMsgHdr" << endl;
	st << getIndent() << "orgAddr    = ";
	if( optionSet & orgAddr_flag )
	{
		orgAddr.print(st);
	}
	else
		st << "(not present)" << endl;
	st << getIndent() << "$status    = ";
	st << status << endl;
	st << getIndent() << "$cseq      = ";
	st << cseq << endl;
	st << getIndent() << "$observer  = ";
	st << "\""; observer.bprint(st); st << "\"" << endl;
	st << getIndent() << "$trigger   = ";
	st << "\""; trigger.bprint(st); st << "\"" << endl;
	st << getIndent() << "$eid       = ";
	st << "\""; eid.bprint(st); st << "\"" << endl;
}

int TEventMsgHdr::getFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if((optionSet & orgAddr_flag) && fieldName == "orgAddr") return orgAddr.getFieldValue(p, type, value);
	if(fieldName == "status")
	{
		value = status; type = 2; return 1;
	}
	else if(fieldName == "cseq")
	{
		value = cseq; type = 2; return 1;
	}
	else if(fieldName == "observer")
	{
		value.set(observer.GetVarCharContentPoint(), observer.GetVarCharLen()); type = 4; return 1;
	}
	else if(fieldName == "trigger")
	{
		value.set(trigger.GetVarCharContentPoint(), trigger.GetVarCharLen()); type = 4; return 1;
	}
	else if(fieldName == "eid")
	{
		value.set(eid.GetVarCharContentPoint(), eid.GetVarCharLen()); type = 4; return 1;
	}
	UniERROR("TEventMsgHdr::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}

int TEventMsgHdr::setFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if((optionSet & orgAddr_flag) && fieldName == "orgAddr") return orgAddr.setFieldValue(p, type, value);
	if(fieldName == "status")
	{
		status = (UINT)(value.toInt()); return 1;
	}
	else if(fieldName == "cseq")
	{
		cseq = (UINT)(value.toInt()); return 1;
	}
	else if(fieldName == "observer")
	{
		observer.set(value.c_str(), value.length()); return 1;
	}
	else if(fieldName == "trigger")
	{
		trigger.set(value.c_str(), value.length()); return 1;
	}
	else if(fieldName == "eid")
	{
		eid.set(value.c_str(), value.length()); return 1;
	}
	UniERROR("TEventMsgHdr::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}


/////////////////////////////////////////////
//           for class TEventMsgBody
/////////////////////////////////////////////
PTMsgBody TEventMsgBody::clone()
{
	PTEventMsgBody amsg = new TEventMsgBody();
	amsg->ev                        = ev;
	amsg->einfo                     = einfo;
	return amsg;
}
TEventMsgBody& TEventMsgBody::operator=(const TEventMsgBody &r)
{
	ev                        = r.ev;
	einfo                     = r.einfo;
	return *this;
}

BOOL TEventMsgBody::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TEventMsgBody,msg)

	COMPARE_FORCE_VCHAR(TEventMsgBody,ev)
	COMPARE_FORCE_VCHAR(TEventMsgBody,einfo)

	COMPARE_END
}

INT TEventMsgBody::size()
{
	INT tmpSize = 0;

	tmpSize += ev.size();
	tmpSize += einfo.size();

	return tmpSize;
}

INT TEventMsgBody::encode(CHAR* &buf)
{
	ev.encode(buf);
	einfo.encode(buf);

	return size();
}

INT TEventMsgBody::decode(CHAR* &buf)
{
	ev.decode(buf);
	einfo.decode(buf);

	return size();
}

BOOL TEventMsgBody::decodeFromXML(TiXmlHandle& xmlParser)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_VCHAR(TEventMsgBody,ev)
	FILL_FORCE_VCHAR(TEventMsgBody,einfo)

	FILL_FIELD_END
}
void TEventMsgBody::print(ostrstream& st)
{
	st << "TEventMsgBody" << endl;
	st << getIndent() << "$ev    = ";
	st << "\""; ev.bprint(st); st << "\"" << endl;
	st << getIndent() << "$einfo = ";
	st << "\""; einfo.bprint(st); st << "\"" << endl;
}

int TEventMsgBody::getFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if(fieldName == "ev")
	{
		value.set(ev.GetVarCharContentPoint(), ev.GetVarCharLen()); type = 4; return 1;
	}
	else if(fieldName == "einfo")
	{
		value.set(einfo.GetVarCharContentPoint(), einfo.GetVarCharLen()); type = 4; return 1;
	}
	UniERROR("TEventMsgBody::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}

int TEventMsgBody::setFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if(fieldName == "ev")
	{
		ev.set(value.c_str(), value.length()); return 1;
	}
	else if(fieldName == "einfo")
	{
		einfo.set(value.c_str(), value.length()); return 1;
	}
	UniERROR("TEventMsgBody::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}

