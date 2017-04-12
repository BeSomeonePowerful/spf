/*****************************************************************************
 * msgdef_mqtt.C
 * It is an implementation file of message definition.
 * 
 * Note: This file is created automatically by msg compiler tool. 
 *       Please do not modify it.
 * 
 * Created at Fri Sep 23 21:28:29 2016
.
 * 
 ******************************************************************************/
#include "msgdef_mqtt.h"
#include "info.h"
#include "msgutil.h"
_CLASSDEF(CGFSM);


/////////////////////////////////////////////
//           for class TMqttCtrlMsg
/////////////////////////////////////////////
PTCtrlMsg TMqttCtrlMsg::clone()
{
	PTMqttCtrlMsg amsg = new TMqttCtrlMsg();
	amsg->optionSet                 = optionSet;

	amsg->orgAddr                   = orgAddr;

	amsg->cid                       = cid;
	return amsg;
}
TMqttCtrlMsg& TMqttCtrlMsg::operator=(const TMqttCtrlMsg &r)
{
	cid                       = r.cid;
	return *this;
}

BOOL TMqttCtrlMsg::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TMqttCtrlMsg,msg)

	COMPARE_FORCE_INT(TMqttCtrlMsg,cid)

	COMPARE_END
}

INT TMqttCtrlMsg::size()
{
	INT tmpSize = 0;

	tmpSize += sizeof(UINT); //for optionSet

	if( optionSet & orgAddr_flag )	tmpSize += orgAddr.size();

	tmpSize += sizeof(INT);

	return tmpSize;
}

INT TMqttCtrlMsg::encode(CHAR* &buf)
{
	ENCODE_INT( buf , optionSet )

	if( optionSet & orgAddr_flag )   orgAddr.encode(buf);

	ENCODE_INT( buf , cid )

	return size();
}

INT TMqttCtrlMsg::decode(CHAR* &buf)
{
	DECODE_INT( optionSet , buf )

	if( optionSet & orgAddr_flag )   orgAddr.decode(buf);

	DECODE_INT( cid, buf )

	return size();
}

BOOL TMqttCtrlMsg::decodeFromXML(TiXmlHandle& xmlParser)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_INT(TMqttCtrlMsg,cid)

	FILL_FIELD_END
}
void TMqttCtrlMsg::print(ostrstream& st)
{
	st << "TMqttCtrlMsg" << endl;
	st << getIndent() << "orgAddr    = ";
	if( optionSet & orgAddr_flag )
	{
		orgAddr.print(st);
	}
	else
		st << "(not present)" << endl;
	st << getIndent() << "$cid       = ";
	st << cid << endl;
}

int TMqttCtrlMsg::getFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if((optionSet & orgAddr_flag) && fieldName == "orgAddr") return orgAddr.getFieldValue(p, type, value);
	if(fieldName == "cid")
	{
		value = cid; type = 2; return 1;
	}
	UniERROR("TMqttCtrlMsg::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}

int TMqttCtrlMsg::setFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if((optionSet & orgAddr_flag) && fieldName == "orgAddr") return orgAddr.setFieldValue(p, type, value);
	if(fieldName == "cid")
	{
		cid = (UINT)(value.toInt()); return 1;
	}
	UniERROR("TMqttCtrlMsg::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}


/////////////////////////////////////////////
//           for class TMqttConnect
/////////////////////////////////////////////
PTMsgBody TMqttConnect::clone()
{
	PTMqttConnect amsg = new TMqttConnect();
	amsg->optionSet                 = optionSet;

	amsg->clientName                = clientName;
	amsg->userName                  = userName;
	amsg->password                  = password;
	return amsg;
}
TMqttConnect& TMqttConnect::operator=(const TMqttConnect &r)
{
	optionSet                 = r.optionSet;
	clientName                = r.clientName;
	userName                  = r.userName;
	password                  = r.password;
	return *this;
}

BOOL TMqttConnect::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TMqttConnect,msg)

	COMPARE_FORCE_VCHAR(TMqttConnect,clientName)
	COMPARE_OPTION_VCHAR(TMqttConnect,userName)
	COMPARE_OPTION_VCHAR(TMqttConnect,password)

	COMPARE_END
}

INT TMqttConnect::size()
{
	INT tmpSize = 0;

	tmpSize += sizeof(UINT); //for optionSet

	tmpSize += clientName.size();
	if( optionSet & userName_flag) 	tmpSize += userName.size();
	if( optionSet & password_flag) 	tmpSize += password.size();

	return tmpSize;
}

INT TMqttConnect::encode(CHAR* &buf)
{
	ENCODE_INT( buf , optionSet )

	clientName.encode(buf);
	if( optionSet & userName_flag ) 	userName.encode(buf);
	if( optionSet & password_flag ) 	password.encode(buf);

	return size();
}

INT TMqttConnect::decode(CHAR* &buf)
{
	DECODE_INT( optionSet , buf )

	clientName.decode(buf);
	if( optionSet & userName_flag ) 	userName.decode(buf);
	if( optionSet & password_flag ) 	password.decode(buf);

	return size();
}

BOOL TMqttConnect::decodeFromXML(TiXmlHandle& xmlParser)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_VCHAR(TMqttConnect,clientName)
	FILL_OPTION_VCHAR(TMqttConnect,userName)
	FILL_OPTION_VCHAR(TMqttConnect,password)

	FILL_FIELD_END
}
void TMqttConnect::print(ostrstream& st)
{
	st << "TMqttConnect" << endl;
	st << getIndent() << "$clientName = ";
	st << "\""; clientName.bprint(st); st << "\"" << endl;
	st << getIndent() << "userName    = ";
	if(userName_flag & optionSet)
	{
		st << "\""; userName.bprint(st); st << "\"" << endl;
	}
	else
		st << "(not present)" << endl;
	st << getIndent() << "password    = ";
	if(password_flag & optionSet)
	{
		st << "\""; password.bprint(st); st << "\"" << endl;
	}
	else
		st << "(not present)" << endl;
}

int TMqttConnect::getFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if(fieldName == "clientName")
	{
		value.set(clientName.GetVarCharContentPoint(), clientName.GetVarCharLen()); type = 4; return 1;
	}
	else if(fieldName == "userName")
	{
		if(!(optionSet & userName_flag)) { type=1; return 1; }
		value.set(userName.GetVarCharContentPoint(), userName.GetVarCharLen()); type = 4; return 1;
	}
	else if(fieldName == "password")
	{
		if(!(optionSet & password_flag)) { type=1; return 1; }
		value.set(password.GetVarCharContentPoint(), password.GetVarCharLen()); type = 4; return 1;
	}
	UniERROR("TMqttConnect::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}

int TMqttConnect::setFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if(fieldName == "clientName")
	{
		clientName.set(value.c_str(), value.length()); return 1;
	}
	else if(fieldName == "userName")
	{
		optionSet |= userName_flag;
		userName.set(value.c_str(), value.length()); return 1;
	}
	else if(fieldName == "password")
	{
		optionSet |= password_flag;
		password.set(value.c_str(), value.length()); return 1;
	}
	UniERROR("TMqttConnect::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}


/////////////////////////////////////////////
//           for class TMqttConnAck
/////////////////////////////////////////////
PTMsgBody TMqttConnAck::clone()
{
	PTMqttConnAck amsg = new TMqttConnAck();
	amsg->code                      = code;
	return amsg;
}
TMqttConnAck& TMqttConnAck::operator=(const TMqttConnAck &r)
{
	code                      = r.code;
	return *this;
}

BOOL TMqttConnAck::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TMqttConnAck,msg)

	COMPARE_FORCE_INT(TMqttConnAck,code)

	COMPARE_END
}

INT TMqttConnAck::size()
{
	INT tmpSize = 0;

	tmpSize += sizeof(INT);

	return tmpSize;
}

INT TMqttConnAck::encode(CHAR* &buf)
{
	ENCODE_INT( buf , code )

	return size();
}

INT TMqttConnAck::decode(CHAR* &buf)
{
	DECODE_INT( code, buf )

	return size();
}

BOOL TMqttConnAck::decodeFromXML(TiXmlHandle& xmlParser)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_INT(TMqttConnAck,code)

	FILL_FIELD_END
}
void TMqttConnAck::print(ostrstream& st)
{
	st << "TMqttConnAck" << endl;
	st << getIndent() << "$code = ";
	st << code << endl;
}

int TMqttConnAck::getFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if(fieldName == "code")
	{
		value = code; type = 2; return 1;
	}
	UniERROR("TMqttConnAck::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}

int TMqttConnAck::setFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if(fieldName == "code")
	{
		code = (UINT)(value.toInt()); return 1;
	}
	UniERROR("TMqttConnAck::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}


/////////////////////////////////////////////
//           for class TMqttPublish
/////////////////////////////////////////////
PTMsgBody TMqttPublish::clone()
{
	PTMqttPublish amsg = new TMqttPublish();
	amsg->clientName                = clientName;
	amsg->topic                     = topic;
	amsg->content                   = content;
	return amsg;
}
TMqttPublish& TMqttPublish::operator=(const TMqttPublish &r)
{
	clientName                = r.clientName;
	topic                     = r.topic;
	content                   = r.content;
	return *this;
}

BOOL TMqttPublish::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TMqttPublish,msg)

	COMPARE_FORCE_VCHAR(TMqttPublish,clientName)
	COMPARE_FORCE_VCHAR(TMqttPublish,topic)
	COMPARE_FORCE_VCHAR(TMqttPublish,content)

	COMPARE_END
}

INT TMqttPublish::size()
{
	INT tmpSize = 0;

	tmpSize += clientName.size();
	tmpSize += topic.size();
	tmpSize += content.size();

	return tmpSize;
}

INT TMqttPublish::encode(CHAR* &buf)
{
	clientName.encode(buf);
	topic.encode(buf);
	content.encode(buf);

	return size();
}

INT TMqttPublish::decode(CHAR* &buf)
{
	clientName.decode(buf);
	topic.decode(buf);
	content.decode(buf);

	return size();
}

BOOL TMqttPublish::decodeFromXML(TiXmlHandle& xmlParser)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_VCHAR(TMqttPublish,clientName)
	FILL_FORCE_VCHAR(TMqttPublish,topic)
	FILL_FORCE_VCHAR(TMqttPublish,content)

	FILL_FIELD_END
}
void TMqttPublish::print(ostrstream& st)
{
	st << "TMqttPublish" << endl;
	st << getIndent() << "$clientName = ";
	st << "\""; clientName.bprint(st); st << "\"" << endl;
	st << getIndent() << "$topic      = ";
	st << "\""; topic.bprint(st); st << "\"" << endl;
	st << getIndent() << "$content    = ";
	st << "\""; content.bprint(st); st << "\"" << endl;
}

int TMqttPublish::getFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if(fieldName == "clientName")
	{
		value.set(clientName.GetVarCharContentPoint(), clientName.GetVarCharLen()); type = 4; return 1;
	}
	else if(fieldName == "topic")
	{
		value.set(topic.GetVarCharContentPoint(), topic.GetVarCharLen()); type = 4; return 1;
	}
	else if(fieldName == "content")
	{
		value.set(content.GetVarCharContentPoint(), content.GetVarCharLen()); type = 4; return 1;
	}
	UniERROR("TMqttPublish::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}

int TMqttPublish::setFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if(fieldName == "clientName")
	{
		clientName.set(value.c_str(), value.length()); return 1;
	}
	else if(fieldName == "topic")
	{
		topic.set(value.c_str(), value.length()); return 1;
	}
	else if(fieldName == "content")
	{
		content.set(value.c_str(), value.length()); return 1;
	}
	UniERROR("TMqttPublish::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}


/////////////////////////////////////////////
//           for class TMqttSubscribe
/////////////////////////////////////////////
PTMsgBody TMqttSubscribe::clone()
{
	PTMqttSubscribe amsg = new TMqttSubscribe();
	amsg->clientName                = clientName;
	amsg->msgId                     = msgId;
	amsg->topic                     = topic;
	return amsg;
}
TMqttSubscribe& TMqttSubscribe::operator=(const TMqttSubscribe &r)
{
	clientName                = r.clientName;
	msgId                     = r.msgId;
	topic                     = ((TMqttSubscribe&)(r)).topic;
	return *this;
}

BOOL TMqttSubscribe::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TMqttSubscribe,msg)

	COMPARE_FORCE_VCHAR(TMqttSubscribe,clientName)
	COMPARE_FORCE_INT(TMqttSubscribe,msgId)
	COMPARE_FORCE_NEST_LIST(TMqttSubscribe,CStr,topic)

	COMPARE_END
}

INT TMqttSubscribe::size()
{
	INT tmpSize = 0;

	tmpSize += clientName.size();
	tmpSize += sizeof(INT);

	if(topic.size()>0)
	{
		for(CList<CStr>::iterator i = topic.begin(); i != topic.end(); i++)
			tmpSize += (*i).size();
	}

	return tmpSize;
}

INT TMqttSubscribe::encode(CHAR* &buf)
{
	clientName.encode(buf);
	ENCODE_INT( buf , msgId )
	ENCODE_INT( buf , topic.size())
	ENCODE_VCHAR_LIST( buf , CStr, topic )

	return size();
}

INT TMqttSubscribe::decode(CHAR* &buf)
{
	clientName.decode(buf);
	DECODE_INT( msgId, buf )
	UINT topic_size; DECODE_INT( topic_size , buf )
	DECODE_VCHAR_LIST(CStr, topic, buf , topic_size )

	return size();
}

BOOL TMqttSubscribe::decodeFromXML(TiXmlHandle& xmlParser)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_VCHAR(TMqttSubscribe,clientName)
	FILL_FORCE_INT(TMqttSubscribe,msgId)
	// decodeFromXML not support CList

	FILL_FIELD_END
}
void TMqttSubscribe::print(ostrstream& st)
{
	st << "TMqttSubscribe" << endl;
	st << getIndent() << "$clientName = ";
	st << "\""; clientName.bprint(st); st << "\"" << endl;
	st << getIndent() << "$msgId      = ";
	st << msgId << endl;
	st << getIndent() << "$topic      = ";
	st << "ARRAY" << endl;
	incIndent(); int index = 0;
	for(CList<CStr>::iterator i=topic.begin();i!=topic.end();i++)
	{
		st << getIndent() << "[" << index << "] = ";index++;
		st << "\"";  (*i).bprint(st); st << "\"" << endl;
	}
	decIndent();
}

int TMqttSubscribe::getFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if(fieldName == "clientName")
	{
		value.set(clientName.GetVarCharContentPoint(), clientName.GetVarCharLen()); type = 4; return 1;
	}
	else if(fieldName == "msgId")
	{
		value = msgId; type = 2; return 1;
	}
	else if(fieldName == "topic")
	{
		if(arrayIndex<0) { if(*p != NULL && strcmp(*p, "size")==0) {value=topic.size(); type = 2; return 1;} else {UniERROR("TMqttSubscribe::getFieldValue, field %s missing array index", fieldName.c_str()); type=1; return 1;}}
		if(arrayIndex>=topic.size()) { UniERROR("TMqttSubscribe::getFieldValue, field %s invalid array index %d, %d max", fieldName.c_str(), arrayIndex, topic.size()-1); type=1; return 1;}
		CStr* _p_topic; topic.get_p(arrayIndex, _p_topic);
		value.set(_p_topic->GetVarCharContentPoint(), _p_topic->GetVarCharLen()); type = 4; return 1;
	}
	UniERROR("TMqttSubscribe::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}

int TMqttSubscribe::setFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if(fieldName == "clientName")
	{
		clientName.set(value.c_str(), value.length()); return 1;
	}
	else if(fieldName == "msgId")
	{
		msgId = (UINT)(value.toInt()); return 1;
	}
	else if(fieldName == "topic")
	{
		if(arrayIndex<0) { UniERROR("TMqttSubscribe::setFieldValue, field %s missing array index", fieldName.c_str()); return -1;}
		if(arrayIndex>topic.size()) { UniERROR("TMqttSubscribe::setFieldValue, field %s invalid array index %d, %d max", fieldName.c_str(), arrayIndex, topic.size()); return -1;}
		if(arrayIndex==topic.size()) { CStr v; topic.push_back(v); }
		CStr* _p_topic; topic.get_p(arrayIndex, _p_topic);
		_p_topic->set(value.c_str(), value.length()); return 1;
	}
	UniERROR("TMqttSubscribe::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}


/////////////////////////////////////////////
//           for class TMqttUnsubscribe
/////////////////////////////////////////////
PTMsgBody TMqttUnsubscribe::clone()
{
	PTMqttUnsubscribe amsg = new TMqttUnsubscribe();
	amsg->clientName                = clientName;
	amsg->msgId                     = msgId;
	amsg->topic                     = topic;
	return amsg;
}
TMqttUnsubscribe& TMqttUnsubscribe::operator=(const TMqttUnsubscribe &r)
{
	clientName                = r.clientName;
	msgId                     = r.msgId;
	topic                     = ((TMqttUnsubscribe&)(r)).topic;
	return *this;
}

BOOL TMqttUnsubscribe::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TMqttUnsubscribe,msg)

	COMPARE_FORCE_VCHAR(TMqttUnsubscribe,clientName)
	COMPARE_FORCE_INT(TMqttUnsubscribe,msgId)
	COMPARE_FORCE_NEST_LIST(TMqttUnsubscribe,CStr,topic)

	COMPARE_END
}

INT TMqttUnsubscribe::size()
{
	INT tmpSize = 0;

	tmpSize += clientName.size();
	tmpSize += sizeof(INT);

	if(topic.size()>0)
	{
		for(CList<CStr>::iterator i = topic.begin(); i != topic.end(); i++)
			tmpSize += (*i).size();
	}

	return tmpSize;
}

INT TMqttUnsubscribe::encode(CHAR* &buf)
{
	clientName.encode(buf);
	ENCODE_INT( buf , msgId )
	ENCODE_INT( buf , topic.size())
	ENCODE_VCHAR_LIST( buf , CStr, topic )

	return size();
}

INT TMqttUnsubscribe::decode(CHAR* &buf)
{
	clientName.decode(buf);
	DECODE_INT( msgId, buf )
	UINT topic_size; DECODE_INT( topic_size , buf )
	DECODE_VCHAR_LIST(CStr, topic, buf , topic_size )

	return size();
}

BOOL TMqttUnsubscribe::decodeFromXML(TiXmlHandle& xmlParser)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_VCHAR(TMqttUnsubscribe,clientName)
	FILL_FORCE_INT(TMqttUnsubscribe,msgId)
	// decodeFromXML not support CList

	FILL_FIELD_END
}
void TMqttUnsubscribe::print(ostrstream& st)
{
	st << "TMqttUnsubscribe" << endl;
	st << getIndent() << "$clientName = ";
	st << "\""; clientName.bprint(st); st << "\"" << endl;
	st << getIndent() << "$msgId      = ";
	st << msgId << endl;
	st << getIndent() << "$topic      = ";
	st << "ARRAY" << endl;
	incIndent(); int index = 0;
	for(CList<CStr>::iterator i=topic.begin();i!=topic.end();i++)
	{
		st << getIndent() << "[" << index << "] = ";index++;
		st << "\"";  (*i).bprint(st); st << "\"" << endl;
	}
	decIndent();
}

int TMqttUnsubscribe::getFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if(fieldName == "clientName")
	{
		value.set(clientName.GetVarCharContentPoint(), clientName.GetVarCharLen()); type = 4; return 1;
	}
	else if(fieldName == "msgId")
	{
		value = msgId; type = 2; return 1;
	}
	else if(fieldName == "topic")
	{
		if(arrayIndex<0) { if(*p != NULL && strcmp(*p, "size")==0) {value=topic.size(); type = 2; return 1;} else {UniERROR("TMqttUnsubscribe::getFieldValue, field %s missing array index", fieldName.c_str()); type=1; return 1;}}
		if(arrayIndex>=topic.size()) { UniERROR("TMqttUnsubscribe::getFieldValue, field %s invalid array index %d, %d max", fieldName.c_str(), arrayIndex, topic.size()-1); type=1; return 1;}
		CStr* _p_topic; topic.get_p(arrayIndex, _p_topic);
		value.set(_p_topic->GetVarCharContentPoint(), _p_topic->GetVarCharLen()); type = 4; return 1;
	}
	UniERROR("TMqttUnsubscribe::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}

int TMqttUnsubscribe::setFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if(fieldName == "clientName")
	{
		clientName.set(value.c_str(), value.length()); return 1;
	}
	else if(fieldName == "msgId")
	{
		msgId = (UINT)(value.toInt()); return 1;
	}
	else if(fieldName == "topic")
	{
		if(arrayIndex<0) { UniERROR("TMqttUnsubscribe::setFieldValue, field %s missing array index", fieldName.c_str()); return -1;}
		if(arrayIndex>topic.size()) { UniERROR("TMqttUnsubscribe::setFieldValue, field %s invalid array index %d, %d max", fieldName.c_str(), arrayIndex, topic.size()); return -1;}
		if(arrayIndex==topic.size()) { CStr v; topic.push_back(v); }
		CStr* _p_topic; topic.get_p(arrayIndex, _p_topic);
		_p_topic->set(value.c_str(), value.length()); return 1;
	}
	UniERROR("TMqttUnsubscribe::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}


/////////////////////////////////////////////
//           for class TMqttSubAck
/////////////////////////////////////////////
PTMsgBody TMqttSubAck::clone()
{
	PTMqttSubAck amsg = new TMqttSubAck();
	amsg->msgId                     = msgId;
	amsg->code                      = code;
	return amsg;
}
TMqttSubAck& TMqttSubAck::operator=(const TMqttSubAck &r)
{
	msgId                     = r.msgId;
	code                      = r.code;
	return *this;
}

BOOL TMqttSubAck::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TMqttSubAck,msg)

	COMPARE_FORCE_INT(TMqttSubAck,msgId)
	COMPARE_FORCE_INT(TMqttSubAck,code)

	COMPARE_END
}

INT TMqttSubAck::size()
{
	INT tmpSize = 0;

	tmpSize += sizeof(INT);
	tmpSize += sizeof(INT);

	return tmpSize;
}

INT TMqttSubAck::encode(CHAR* &buf)
{
	ENCODE_INT( buf , msgId )
	ENCODE_INT( buf , code )

	return size();
}

INT TMqttSubAck::decode(CHAR* &buf)
{
	DECODE_INT( msgId, buf )
	DECODE_INT( code, buf )

	return size();
}

BOOL TMqttSubAck::decodeFromXML(TiXmlHandle& xmlParser)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_INT(TMqttSubAck,msgId)
	FILL_FORCE_INT(TMqttSubAck,code)

	FILL_FIELD_END
}
void TMqttSubAck::print(ostrstream& st)
{
	st << "TMqttSubAck" << endl;
	st << getIndent() << "$msgId = ";
	st << msgId << endl;
	st << getIndent() << "$code  = ";
	st << code << endl;
}

int TMqttSubAck::getFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if(fieldName == "msgId")
	{
		value = msgId; type = 2; return 1;
	}
	else if(fieldName == "code")
	{
		value = code; type = 2; return 1;
	}
	UniERROR("TMqttSubAck::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}

int TMqttSubAck::setFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if(fieldName == "msgId")
	{
		msgId = (UINT)(value.toInt()); return 1;
	}
	else if(fieldName == "code")
	{
		code = (UINT)(value.toInt()); return 1;
	}
	UniERROR("TMqttSubAck::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}


/////////////////////////////////////////////
//           for class TMqttUnsubAck
/////////////////////////////////////////////
PTMsgBody TMqttUnsubAck::clone()
{
	PTMqttUnsubAck amsg = new TMqttUnsubAck();
	amsg->msgId                     = msgId;
	return amsg;
}
TMqttUnsubAck& TMqttUnsubAck::operator=(const TMqttUnsubAck &r)
{
	msgId                     = r.msgId;
	return *this;
}

BOOL TMqttUnsubAck::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TMqttUnsubAck,msg)

	COMPARE_FORCE_INT(TMqttUnsubAck,msgId)

	COMPARE_END
}

INT TMqttUnsubAck::size()
{
	INT tmpSize = 0;

	tmpSize += sizeof(INT);

	return tmpSize;
}

INT TMqttUnsubAck::encode(CHAR* &buf)
{
	ENCODE_INT( buf , msgId )

	return size();
}

INT TMqttUnsubAck::decode(CHAR* &buf)
{
	DECODE_INT( msgId, buf )

	return size();
}

BOOL TMqttUnsubAck::decodeFromXML(TiXmlHandle& xmlParser)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_INT(TMqttUnsubAck,msgId)

	FILL_FIELD_END
}
void TMqttUnsubAck::print(ostrstream& st)
{
	st << "TMqttUnsubAck" << endl;
	st << getIndent() << "$msgId = ";
	st << msgId << endl;
}

int TMqttUnsubAck::getFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if(fieldName == "msgId")
	{
		value = msgId; type = 2; return 1;
	}
	UniERROR("TMqttUnsubAck::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}

int TMqttUnsubAck::setFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if(fieldName == "msgId")
	{
		msgId = (UINT)(value.toInt()); return 1;
	}
	UniERROR("TMqttUnsubAck::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}


/////////////////////////////////////////////
//           for class TMqttDisconnect
/////////////////////////////////////////////
PTMsgBody TMqttDisconnect::clone()
{
	PTMqttDisconnect amsg = new TMqttDisconnect();
	amsg->clientName                = clientName;
	return amsg;
}
TMqttDisconnect& TMqttDisconnect::operator=(const TMqttDisconnect &r)
{
	clientName                = r.clientName;
	return *this;
}

BOOL TMqttDisconnect::operator == (TMsgPara& msg)
{
	COMPARE_MSG_BEGIN(TMqttDisconnect,msg)

	COMPARE_FORCE_VCHAR(TMqttDisconnect,clientName)

	COMPARE_END
}

INT TMqttDisconnect::size()
{
	INT tmpSize = 0;

	tmpSize += clientName.size();

	return tmpSize;
}

INT TMqttDisconnect::encode(CHAR* &buf)
{
	clientName.encode(buf);

	return size();
}

INT TMqttDisconnect::decode(CHAR* &buf)
{
	clientName.decode(buf);

	return size();
}

BOOL TMqttDisconnect::decodeFromXML(TiXmlHandle& xmlParser)
{
	FILL_FIELD_BEGIN

	FILL_FORCE_VCHAR(TMqttDisconnect,clientName)

	FILL_FIELD_END
}
void TMqttDisconnect::print(ostrstream& st)
{
	st << "TMqttDisconnect" << endl;
	st << getIndent() << "$clientName = ";
	st << "\""; clientName.bprint(st); st << "\"" << endl;
}

int TMqttDisconnect::getFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if(fieldName == "clientName")
	{
		value.set(clientName.GetVarCharContentPoint(), clientName.GetVarCharLen()); type = 4; return 1;
	}
	UniERROR("TMqttDisconnect::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}

int TMqttDisconnect::setFieldValue(const char** p, int& type, CStr& value)
{
	if(*p == NULL) return -1;
	CStr fieldName; int arrayIndex;
	getMsgFieldName(p, fieldName, arrayIndex);
	if(fieldName == "clientName")
	{
		clientName.set(value.c_str(), value.length()); return 1;
	}
	UniERROR("TMqttDisconnect::getFieldValue, no field %s, valid fields: %s", fieldName.c_str(), validPara());
	return -1;
}

