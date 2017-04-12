/*****************************************************************************
 * msgdatadef_http.C
 * It is an implementation file of message definition.
 * 
 * Note: This file is created automatically by msg compiler tool. 
 *       Please do not modify it.
 * 
 * Created at Thu May  5 11:39:28 2011
.
 * 
 ******************************************************************************/
#include "msgdef_coap.h"
#include "info.h"
#include "msgutil.h"


/////////////////////////////////////////////
//           for class TCoAPCtrlMsg
/////////////////////////////////////////////
PTCtrlMsg TCoAPCtrlMsg::clone()
{
	PTCoAPCtrlMsg amsg = new TCoAPCtrlMsg();
	amsg->optionSet                 = optionSet;

	amsg->orgAddr                   = orgAddr;

	amsg->transid                   = transid;
	amsg->status                    = status;
	amsg->dialogid                  = dialogid;
	return amsg;
}
TCoAPCtrlMsg& TCoAPCtrlMsg::operator=(const TCoAPCtrlMsg &r)
{
	transid                   = r.transid;
	status                    = r.status;
	dialogid                  = r.dialogid;
	return *this;
}

BOOL TCoAPCtrlMsg::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TCoAPCtrlMsg,msg)

	COMPARE_FORCE_VCHAR(TCoAPCtrlMsg,transid)
	COMPARE_FORCE_INT(TCoAPCtrlMsg,status)
	COMPARE_FORCE_VCHAR(TCoAPCtrlMsg,dialogid)

	COMPARE_END
}

INT TCoAPCtrlMsg::size()
{
	INT tmpSize = 0;

	tmpSize += sizeof(UINT); //for optionSet

	if( optionSet & orgAddr_flag )	tmpSize += orgAddr.size();

	tmpSize += transid.size();
	tmpSize += sizeof(INT);
	tmpSize += sizeof(INT);
	tmpSize += dialogid.size();

	return tmpSize;
}

INT TCoAPCtrlMsg::encode(CHAR* &buf)
{
	ENCODE_INT( buf , optionSet )

	if( optionSet & orgAddr_flag )   orgAddr.encode(buf);

	transid.encode(buf);
	ENCODE_INT( buf , status )
	dialogid.encode(buf);

	return size();
}

INT TCoAPCtrlMsg::decode(CHAR* &buf)
{
	DECODE_INT( optionSet , buf )

	if( optionSet & orgAddr_flag )   orgAddr.decode(buf);

	transid.decode(buf);
	DECODE_INT( status, buf )
	dialogid.decode(buf);

	return size();
}

BOOL TCoAPCtrlMsg::decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_VCHAR(TCoAPCtrlMsg,transid)
	FILL_FORCE_INT(TCoAPCtrlMsg,status)
	FILL_FORCE_VCHAR(TCoAPCtrlMsg,dialogid)

	FILL_FIELD_END
}
void TCoAPCtrlMsg::print(ostrstream& st)
{
	st << "TCoAPCtrlMsg" << endl;
	CHAR temp[30];
	sprintf(temp,"0x%x", optionSet);
	st << getIndent() << "optionSet  = " << temp << endl;
	st << getIndent() << "orgAddr    = ";
	if( optionSet & orgAddr_flag )
	{
		orgAddr.print(st);
	}
	else
		st << "(not present)" << endl;
	st << getIndent() << "$transid   = ";
	st << "\"" << transid.GetVarCharContentPoint() << "\"" << endl;
	st << getIndent() << "$method    = ";
	st << getIndent() << "$status    = ";
	st << status << endl;
	st << getIndent() << "$dialogid  = ";
	st << "\"" << dialogid.GetVarCharContentPoint() << "\"" << endl;
}

int TCoAPCtrlMsg::getFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if((optionSet & orgAddr_flag) && fieldName == "orgAddr") return orgAddr.getFieldValue(p, type, value);
	if(fieldName == "transid")
	{
		value = transid.GetVarCharContentPoint(); type = 4; return 1;
	}
	else if(fieldName == "status")
	{
		value = status; type = 2; return 1;
	}
	else if(fieldName == "dialogid")
	{
		value = dialogid.GetVarCharContentPoint(); type = 4; return 1;
	}
	return -1;
}

int TCoAPCtrlMsg::setFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if((optionSet & orgAddr_flag) && fieldName == "orgAddr") return orgAddr.setFieldValue(p, type, value);
	if(fieldName == "transid")
	{
		transid = value.c_str(); return 1;
	}
	else if(fieldName == "status")
	{
		status = (INT)(value.toInt()); return 1;
	}
	else if(fieldName == "dialogid")
	{
		dialogid = value.c_str(); return 1;
	}
	return -1;
}

