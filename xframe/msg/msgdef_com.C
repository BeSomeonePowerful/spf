/*****************************************************************************
 * msgdef_com.C
 * It is an implementation file of message definition.
 * 
 * Note: This file is created automatically by msg compiler tool. 
 *       Please do not modify it.
 * 
 * Created at Wed Feb 15 09:53:19 2017
.
 * 
 ******************************************************************************/
#include "msgdef_com.h"
#include "info.h"
#include "msgutil.h"
_CLASSDEF(CGFSM);


/////////////////////////////////////////////
//           for class TComMsgHdr
/////////////////////////////////////////////
PTCtrlMsg TComMsgHdr::clone()
{
	PTComMsgHdr amsg = new TComMsgHdr();
	amsg->optionSet                 = optionSet;

	amsg->orgAddr                   = orgAddr;

	amsg->status                    = status;
	amsg->pri                       = pri;
	amsg->facility                  = facility;
	amsg->argc                      = argc;
	if(argc>0)
	{
		for(int i=0;i<argc;i++)
			amsg->argv[i] = argv[i];
	}
	return amsg;
}
TComMsgHdr& TComMsgHdr::operator=(const TComMsgHdr &r)
{
	status                    = r.status;
	pri                       = r.pri;
	facility                  = r.facility;
	argc                      = r.argc;
	if(argc>0)
	{
		for(int i=0;i<argc;i++)
			argv[i] = r.argv[i];
	}
	return *this;
}

BOOL TComMsgHdr::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TComMsgHdr,msg)

	COMPARE_FORCE_INT(TComMsgHdr,status)
	COMPARE_FORCE_INT(TComMsgHdr,pri)
	COMPARE_FORCE_INT(TComMsgHdr,facility)
	COMPARE_FORCE_INT(TComMsgHdr,argc)
	COMPARE_FORCE_VCHAR_ARRAY(TComMsgHdr,argv,argc)

	COMPARE_END
}

INT TComMsgHdr::size()
{
	INT tmpSize = 0;

	tmpSize += sizeof(UINT); //for optionSet

	if( optionSet & orgAddr_flag )	tmpSize += orgAddr.size();

	tmpSize += sizeof(INT);
	tmpSize += sizeof(INT);
	tmpSize += sizeof(INT);
	tmpSize += sizeof(INT);

	if(argc>0)
	{
		for(int i=0;i<argc;i++)
			tmpSize += argv[i].size();
	}

	return tmpSize;
}

INT TComMsgHdr::encode(CHAR* &buf)
{
	ENCODE_INT( buf , optionSet )

	if( optionSet & orgAddr_flag )   orgAddr.encode(buf);

	ENCODE_INT( buf , status )
	ENCODE_INT( buf , pri )
	ENCODE_INT( buf , facility )
	ENCODE_INT( buf , argc )
	ENCODE_VCHAR_ARRAY( buf , argv , argc )

	return size();
}

INT TComMsgHdr::decode(CHAR* &buf)
{
	DECODE_INT( optionSet , buf )

	if( optionSet & orgAddr_flag )   orgAddr.decode(buf);

	DECODE_INT( status, buf )
	DECODE_INT( pri, buf )
	DECODE_INT( facility, buf )
	DECODE_INT( argc, buf )
	CHECK_DECODE_ARRAY_SIZE(  argc , MAX_MSG_ARGV_NUM , argv , CStr )
	DECODE_VCHAR_ARRAY( argv, buf , argc )

	return size();
}

BOOL TComMsgHdr::decodeFromXML(TiXmlHandle& xmlParser)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_INT(TComMsgHdr,status)
	FILL_FORCE_INT(TComMsgHdr,pri)
	FILL_FORCE_INT(TComMsgHdr,facility)
	FILL_FORCE_INT(TComMsgHdr,argc)
	FILL_FORCE_VCHAR_ARRAY(TComMsgHdr,argv,argc)

	FILL_FIELD_END
}
void TComMsgHdr::print(ostrstream& st)
{
	st << "TComMsgHdr" << endl;
	st << getIndent() << "orgAddr    = ";
	if( optionSet & orgAddr_flag )
	{
		orgAddr.print(st);
	}
	else
		st << "(not present)" << endl;
	st << getIndent() << "$status    = ";
	st << status << endl;
	st << getIndent() << "$pri       = ";
	st << pri << endl;
	st << getIndent() << "$facility  = ";
	st << facility << endl;
	st << getIndent() << "$argc      = ";
	st << argc << endl;
	st << getIndent() << "$argv      = ";
	st << "ARRAY" << endl;
	incIndent();
	for(int i=0;i<argc;i++)
	{
		st << getIndent() << "[" << i << "] = ";
		st << "\"";  argv[i].bprint(st); st << "\"" << endl;
	}
	decIndent();
}

int TComMsgHdr::getFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if((optionSet & orgAddr_flag) && fieldName == "orgAddr") return orgAddr.getFieldValue(p, type, value);
	if(fieldName == "status")
	{
		value = status; type = 2; return 1;
	}
	else if(fieldName == "pri")
	{
		value = pri; type = 2; return 1;
	}
	else if(fieldName == "facility")
	{
		value = facility; type = 2; return 1;
	}
	else if(fieldName == "argc")
	{
		value = argc; type = 2; return 1;
	}
	else if(fieldName == "argv")
	{
		if(arrayIndex<0) { if(*p != NULL && strcmp(*p, "size")==0) {value=argc; type = 2; return 1;} else {UniERROR("TComMsgHdr::getFieldValue, field %s missing array index", fieldName.c_str()); type=1; return 1;}}
		if(arrayIndex>=argc || arrayIndex>=MAX_MSG_ARGV_NUM) { UniERROR("TComMsgHdr::getFieldValue, field %s invalid array index %d, %d max", fieldName.c_str(), arrayIndex, (MAX_MSG_ARGV_NUM<argc?MAX_MSG_ARGV_NUM:argc)-1); type=1; return 1;}
		value.set(argv[arrayIndex].GetVarCharContentPoint(), argv[arrayIndex].GetVarCharLen()); type = 4; return 1;
	}
	UniERROR("TComMsgHdr::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}

int TComMsgHdr::setFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if((optionSet & orgAddr_flag) && fieldName == "orgAddr") return orgAddr.setFieldValue(p, type, value);
	if(fieldName == "status")
	{
		status = (INT)(value.toInt()); return 1;
	}
	else if(fieldName == "pri")
	{
		pri = (INT)(value.toInt()); return 1;
	}
	else if(fieldName == "facility")
	{
		facility = (INT)(value.toInt()); return 1;
	}
	else if(fieldName == "argc")
	{
		argc = (INT)(value.toInt()); return 1;
	}
	else if(fieldName == "argv")
	{
		if(arrayIndex<0) { UniERROR("TComMsgHdr::setFieldValue, field %s missing array index", fieldName.c_str()); return -1;}
		if(arrayIndex>=MAX_MSG_ARGV_NUM) { UniERROR("TComMsgHdr::setFieldValue, field %s invalid array index %d, %d max", fieldName.c_str(), arrayIndex, MAX_MSG_ARGV_NUM); return -1;}
		if(arrayIndex>=argc) { argc = arrayIndex+1; }
		argv[arrayIndex].set(value.c_str(), value.length()); return 1;
	}
	UniERROR("TComMsgHdr::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}


/////////////////////////////////////////////
//           for class TComMsgBody
/////////////////////////////////////////////
PTMsgBody TComMsgBody::clone()
{
	PTComMsgBody amsg = new TComMsgBody();
	amsg->info                      = info;
	return amsg;
}
TComMsgBody& TComMsgBody::operator=(const TComMsgBody &r)
{
	info                      = r.info;
	return *this;
}

BOOL TComMsgBody::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TComMsgBody,msg)

	COMPARE_FORCE_VCHAR(TComMsgBody,info)

	COMPARE_END
}

INT TComMsgBody::size()
{
	INT tmpSize = 0;

	tmpSize += info.size();

	return tmpSize;
}

INT TComMsgBody::encode(CHAR* &buf)
{
	info.encode(buf);

	return size();
}

INT TComMsgBody::decode(CHAR* &buf)
{
	info.decode(buf);

	return size();
}

BOOL TComMsgBody::decodeFromXML(TiXmlHandle& xmlParser)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_VCHAR(TComMsgBody,info)

	FILL_FIELD_END
}
void TComMsgBody::print(ostrstream& st)
{
	st << "TComMsgBody" << endl;
	st << getIndent() << "$info = ";
	st << "\""; info.bprint(st); st << "\"" << endl;
}

int TComMsgBody::getFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if(fieldName == "info")
	{
		value.set(info.GetVarCharContentPoint(), info.GetVarCharLen()); type = 4; return 1;
	}
	UniERROR("TComMsgBody::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}

int TComMsgBody::setFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if(fieldName == "info")
	{
		info.set(value.c_str(), value.length()); return 1;
	}
	UniERROR("TComMsgBody::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}

