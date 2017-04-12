#include "msghelper_rsc.h"

void TRscMsgHelper::brief(PTUniNetMsg unimsg, CStr& brief)
{
	if(unimsg==NULL) return;
	unimsg->brief(brief);
	if(unimsg->msgName==RSC_REQUEST) 	brief<<"\r\nRSC:{ req:{ ";
	else if(unimsg->msgName==RSC_RESPONSE)  brief<<"\r\nRSC:{ rsp:{ ";
	else if(unimsg->msgName==RSC_MESSAGE)   brief<<"\r\nRSC:{ msg:{ ";

	if(unimsg->hasCtrlMsgHdr())
	{
		TRscMsgHdr* hdr=(TRscMsgHdr*)(unimsg->getCtrlMsgHdr());
		brief<<"code:"<<hdr->code<<"; ruri:"<<hdr->ruri<<"; consumer:"<<hdr->consumer<<"; producer:"<<hdr->producer<<"; rid:"<<hdr->rid<<"\r\n";
	}
	if(unimsg->hasMsgBody())
	{
		TRscMsgBody* body=(TRscMsgBody*)(unimsg->getMsgBody());
		brief<<"        rsc:{"<<body->rsc<<"}";
	}
	brief<<"}}\r\n";
}
	
