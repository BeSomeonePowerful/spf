#if !defined __MSGHELPER_HTTP_H
#define __MSGHELPER_HTTP_H

#include "msgdef_http.h"


_CLASSDEF(HTTPMsgHelper);
class HTTPMsgHelper
{
public:
	static void addMsgBody(PTUniNetMsg uniMsg, tbnet::HttpMessage* msg);
	static void addCtrlMsgHdr(PTUniNetMsg uniMsg, tbnet::HttpMessage* msg);
	static tbnet::HttpMessage* makeHTTPResponse(tbnet::HttpMessage* msg);
};


#endif
