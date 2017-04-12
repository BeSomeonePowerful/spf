#include "msghelper_event.h"

void TEventMsgHelper::brief(PTUniNetMsg unimsg, CStr& brief)
{
	if(unimsg==NULL) return;
	unimsg->brief(brief);
	if(unimsg->msgName==EVENT_MESSAGE_REQ) 	brief<<"\r\nEVENT:{ event:{ ";
	else if(unimsg->msgName==EVENT_MESSAGE_RSP)  brief<<"\r\nEVENT:{ reply:{ ";

	if(unimsg->hasCtrlMsgHdr())
	{
		TEventMsgHdr* hdr=(TEventMsgHdr*)(unimsg->getCtrlMsgHdr());
		brief<<"status:"<<hdr->status<<"; observer:"<<hdr->observer<<"; trigger:"<<hdr->trigger<<"; eid:"<<hdr->eid<<"; cseq:"<<hdr->cseq<<"\r\n";
	}
	if(unimsg->hasMsgBody())
	{
		TEventMsgBody* body=(TEventMsgBody*)(unimsg->getMsgBody());
		brief<<"        einfo:{ ev:"<<body->ev<<"; einfo:{"<<body->einfo<<"}}";
	}
	brief<<"}}\r\n";
}
	
