/*****************************************************************************
 * msgdef_sip.C
 * It is an implementation file of message definition.
 * 
 * Note: This file is created automatically by msg compiler tool. 
 *       Please do not modify it.
 * 
 * Created at Sun Sep 25 23:24:49 2016
.
 * 
 ******************************************************************************/
#include "msgdef_sip.h"
#include "info.h"
#include "msgutil.h"
_CLASSDEF(CGFSM);


/////////////////////////////////////////////
//           for class TSipCtrlMsg
/////////////////////////////////////////////
PTCtrlMsg TSipCtrlMsg::clone()
{
	PTSipCtrlMsg amsg = new TSipCtrlMsg();
	amsg->optionSet                 = optionSet;

	amsg->orgAddr                   = orgAddr;

	amsg->transid                   = transid;
	amsg->method                    = method;
	amsg->status                    = status;
	amsg->rsp_method                = rsp_method;
	amsg->needprack                 = needprack;
	amsg->sub_status                = sub_status;
	amsg->dialogid                  = dialogid;
	return amsg;
}
TSipCtrlMsg& TSipCtrlMsg::operator=(const TSipCtrlMsg &r)
{
	transid                   = r.transid;
	method                    = r.method;
	status                    = r.status;
	rsp_method                = r.rsp_method;
	needprack                 = r.needprack;
	sub_status                = r.sub_status;
	dialogid                  = r.dialogid;
	return *this;
}

BOOL TSipCtrlMsg::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TSipCtrlMsg,msg)

	COMPARE_FORCE_LONG(TSipCtrlMsg,transid)
	COMPARE_FORCE_INT(TSipCtrlMsg,method)
	COMPARE_FORCE_INT(TSipCtrlMsg,status)
	COMPARE_FORCE_INT(TSipCtrlMsg,rsp_method)
	COMPARE_FORCE_INT(TSipCtrlMsg,needprack)
	COMPARE_FORCE_INT(TSipCtrlMsg,sub_status)
	COMPARE_FORCE_VCHAR(TSipCtrlMsg,dialogid)

	COMPARE_END
}

INT TSipCtrlMsg::size()
{
	INT tmpSize = 0;

	tmpSize += sizeof(UINT); //for optionSet

	if( optionSet & orgAddr_flag )	tmpSize += orgAddr.size();

	tmpSize += sizeof(LONG);
	tmpSize += sizeof(INT);
	tmpSize += sizeof(INT);
	tmpSize += sizeof(INT);
	tmpSize += sizeof(INT);
	tmpSize += sizeof(INT);
	tmpSize += dialogid.size();

	return tmpSize;
}

INT TSipCtrlMsg::encode(CHAR* &buf)
{
	ENCODE_INT( buf , optionSet )

	if( optionSet & orgAddr_flag )   orgAddr.encode(buf);

	ENCODE_LONG( buf , transid )
	ENCODE_ENUM( buf , method )
	ENCODE_INT( buf , status )
	ENCODE_ENUM( buf , rsp_method )
	ENCODE_INT( buf , needprack )
	ENCODE_INT( buf , sub_status )
	dialogid.encode(buf);

	return size();
}

INT TSipCtrlMsg::decode(CHAR* &buf)
{
	DECODE_INT( optionSet , buf )

	if( optionSet & orgAddr_flag )   orgAddr.decode(buf);

	DECODE_LONG( transid, buf )
	DECODE_ENUM( method, buf )
	DECODE_INT( status, buf )
	DECODE_ENUM( rsp_method, buf )
	DECODE_INT( needprack, buf )
	DECODE_INT( sub_status, buf )
	dialogid.decode(buf);

	return size();
}

BOOL TSipCtrlMsg::decodeFromXML(TiXmlHandle& xmlParser)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_LONG(TSipCtrlMsg,transid)
	FILL_FORCE_ENUM(TSipCtrlMsg,SIPMETHODTYPE,method)
	FILL_FORCE_INT(TSipCtrlMsg,status)
	FILL_FORCE_ENUM(TSipCtrlMsg,SIPMETHODTYPE,rsp_method)
	FILL_FORCE_INT(TSipCtrlMsg,needprack)
	FILL_FORCE_INT(TSipCtrlMsg,sub_status)
	FILL_FORCE_VCHAR(TSipCtrlMsg,dialogid)

	FILL_FIELD_END
}
void TSipCtrlMsg::print(ostrstream& st)
{
	st << "TSipCtrlMsg" << endl;
	st << getIndent() << "orgAddr     = ";
	if( optionSet & orgAddr_flag )
	{
		orgAddr.print(st);
	}
	else
		st << "(not present)" << endl;
	st << getIndent() << "$transid    = ";
	st << transid << endl;
	st << getIndent() << "$method     = ";
	st << method << endl;
	st << getIndent() << "$status     = ";
	st << status << endl;
	st << getIndent() << "$rsp_method = ";
	st << rsp_method << endl;
	st << getIndent() << "$needprack  = ";
	st << needprack << endl;
	st << getIndent() << "$sub_status = ";
	st << sub_status << endl;
	st << getIndent() << "$dialogid   = ";
	st << "\""; dialogid.bprint(st); st << "\"" << endl;
}

int TSipCtrlMsg::getFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if((optionSet & orgAddr_flag) && fieldName == "orgAddr") return orgAddr.getFieldValue(p, type, value);
	if(fieldName == "transid")
	{
		value = transid; type = 2; return 1;
	}
	else if(fieldName == "method")
	{
		value = method; type = 2; return 1;
	}
	else if(fieldName == "status")
	{
		value = status; type = 2; return 1;
	}
	else if(fieldName == "rsp_method")
	{
		value = rsp_method; type = 2; return 1;
	}
	else if(fieldName == "needprack")
	{
		value = needprack; type = 2; return 1;
	}
	else if(fieldName == "sub_status")
	{
		value = sub_status; type = 2; return 1;
	}
	else if(fieldName == "dialogid")
	{
		value.set(dialogid.GetVarCharContentPoint(), dialogid.GetVarCharLen()); type = 4; return 1;
	}
	UniERROR("TSipCtrlMsg::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}

int TSipCtrlMsg::setFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if((optionSet & orgAddr_flag) && fieldName == "orgAddr") return orgAddr.setFieldValue(p, type, value);
	if(fieldName == "transid")
	{
		transid = (ULONG)(value.toInt()); return 1;
	}
	else if(fieldName == "method")
	{
		method = (SIPMETHODTYPE)(value.toInt()); return 1;
	}
	else if(fieldName == "status")
	{
		status = (INT)(value.toInt()); return 1;
	}
	else if(fieldName == "rsp_method")
	{
		rsp_method = (SIPMETHODTYPE)(value.toInt()); return 1;
	}
	else if(fieldName == "needprack")
	{
		needprack = (INT)(value.toInt()); return 1;
	}
	else if(fieldName == "sub_status")
	{
		sub_status = (INT)(value.toInt()); return 1;
	}
	else if(fieldName == "dialogid")
	{
		dialogid.set(value.c_str(), value.length()); return 1;
	}
	UniERROR("TSipCtrlMsg::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}


/////////////////////////////////////////////
//           for class TSipMsg
/////////////////////////////////////////////
//mdf ÐÞ¸ÄCStrµ½Ö¸Õë
TSipMsg::TSipMsg(const TSipMsg& r) {
	//optionSet = r.optionSet;
	sipMsg = new resip::SipMessage();
	*sipMsg = *r.sipMsg;
}

PTMsgBody TSipMsg::clone()
{
	PTSipMsg amsg = new TSipMsg(*this);
        //PTSipMsg amsg = new TSipMsg();
	//amsg->sipMsg                    = sipMsg;
	return amsg;
}
TSipMsg& TSipMsg::operator=(const TSipMsg &r)
{
	//sipMsg                    = r.sipMsg;
	if (this == &r) return *this;
	resip::SipMessage* pSipMsg = sipMsg;
	sipMsg = new resip::SipMessage(*r.sipMsg);
	delete pSipMsg;
	return *this;
}

BOOL TSipMsg::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TSipMsg,msg)
	COMPARE_FORCE_INT(TSipMsg,sipMsg)
	//COMPARE_FORCE_VCHAR(TSipMsg,sipMsg)

	COMPARE_END
}

INT TSipMsg::size()
{
	INT tmpSize = 0;

	//tmpSize += sipMsg.size();
	tmpSize += sizeof(sipMsg);

	return tmpSize;
}

INT TSipMsg::encode(CHAR* &buf)
{
	//sipMsg.encode(buf);
        SIPMsgHelper::brief(sipMsg).encode(buf);
	return size();
}

INT TSipMsg::decode(CHAR* &buf)
{
	//sipMsg.decode(buf);
        SIPMsgHelper::brief(sipMsg).decode(buf);
	return size();
}

BOOL TSipMsg::decodeFromXML(TiXmlHandle& xmlParser)
{
	FILL_FIELD_BEGIN

	//FILL_FORCE_VCHAR(TSipMsg,sipMsg)

	FILL_FIELD_END
}
void TSipMsg::print(ostrstream& st)
{
	st << "TSipMsg" << endl;
	st << getIndent() << "$sipMsg = ";
        CStr msg=SIPMsgHelper::brief(sipMsg);
	st << "\""; msg.bprint(st); st << "\"" << endl;
}

int TSipMsg::getFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if(fieldName == "sipMsg")
	{
                CStr msg=SIPMsgHelper::brief(sipMsg);
		value.set(msg.GetVarCharContentPoint(), msg.GetVarCharLen()); type = 4; return 1;
         }
	UniERROR("TSipMsg::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}

int TSipMsg::setFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if(fieldName == "sipMsg")
	{
		//sipMsg.set(value.c_str(), value.length()); return 1;
	}
	UniERROR("TSipMsg::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}

