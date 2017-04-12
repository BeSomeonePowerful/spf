#if !defined __MSGHELPER_COM_H
#define __MSGHELPER_COM_H

#include "msgdef_com.h"

_CLASSDEF(COMMsgHelper);
class COMMsgHelper
{
public:
	static int log_Level(CStr& s);
	static void addMsgBody(PTUniNetMsg uniMsg, CStr& msg);
	static void addCtrlMsgHdr(PTUniNetMsg uniMsg, INT st, INT pr,INT fa);
	static TComMsgHdr*  getComMsgCtrlHdr(PTUniNetMsg unimsg);
	static TComMsgBody* getComMsgBody(PTUniNetMsg unimsg);
	//基于请求产生响应
	static TUniNetMsg*  genComMsgResp(PTUniNetMsg unimsg, INT code=200);
	static INT getComMsgPri(PTUniNetMsg unimsg);
	
	//消息摘要
	static void brief(PTUniNetMsg unimsg, CStr& brief);
};


#endif
