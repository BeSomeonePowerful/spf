
#include "LogTask.h"
#include <iostream>
#include <cstdlib>
#include <syslog.h>

#include "db.h"
#include "func.h"
#include "info.h"
#include "msgdef_com.h"
#include "msghelper_com.h"

using namespace std;

LogTask::LogTask():TAbstractTask()
{
}

LogTask::~LogTask()
{
}

BOOL LogTask::reloadTaskEnv(CStr& cmd, TiXmlElement* & extend)
{
	CStr scmd[5];
	cmd.split(scmd,5);
	if(!strcmp(scmd[0].c_str(),"reload"))
	{
		onInit(extend);
	}
	return true;
}
 
BOOL LogTask::onInit(TiXmlElement*	extend)
{
	CStr info;
	TiXmlHandle handle(extend);
	TiXmlElement*   syslog=NULL;
    syslog=handle.FirstChild("sysLog").Element();
	if(syslog)
	{
		CStr tmp;
		if(!syslog->Attribute("logLevel", &mLogLevel)) mLogLevel=6;	
		tmp=syslog->Attribute("sysloglevel");
		info<<"  syslog: logLevel="<<mLogLevel<<"("<<tmp<<");\r\n";	
	}
	
	memset(mFacility, 0, sizeof(int)*MAX_FACILITY_NUM);
	info<<"  facilities:\r\n";
	TiXmlElement* facilities = NULL;
	facilities=handle.FirstChild("facilities").Element();
	if(facilities)
	{
		TiXmlHandle fhandle(facilities);
		TiXmlElement*	facility=NULL;
		facility=fhandle.FirstChild("facility").Element();
		while(facility)
		{
			int logtype=0;	
			CStr tmp2;
			int fid=0;
			if(!facility->Attribute("logtype", &logtype)) logtype=0;	//如果logtype=0，则该设置不生效
			if(logtype>0 && logtype< MAX_FACILITY_NUM)
			{
				tmp2=facility->Attribute("logname");
				if(!facility->Attribute("fid", &fid)) fid=0;
				mFacility[logtype]=fid;
				info<<"    logtype="<<logtype<<"("<<tmp2<<"), fid="<<fid<<";\r\n";
			}
			facility=facility->NextSiblingElement();
		}
	}		
	
	UniINFO("Init logTask OK!");
	UniINFO("%s", info.c_str());
	return TRUE;

}


void LogTask::procMsg(std::auto_ptr<TUniNetMsg> msg)
{

	TUniNetMsg* unimsg=msg.get();
	if(!unimsg) return;
	if(!unimsg->hasCtrlMsgHdr()) return;
	if(!unimsg->hasMsgBody()) return; 
	
	CStr logmsg(COMMsgHelper::getComMsgBody(unimsg)->info);

	int loglevel=COMMsgHelper::getComMsgCtrlHdr(unimsg)->pri;
	int logtype =COMMsgHelper::getComMsgCtrlHdr(unimsg)->facility;
	
	if(logtype>=MAX_FACILITY_NUM || logtype<=0)
	{
		logtype=MSGINFO;
	}
	if(logtype!=MSGINFO || (logtype==MSGINFO && loglevel <= mLogLevel))
	{
		syslog(LOG_MAKEPRI(mFacility[logtype],loglevel),logmsg.c_str());
	}

	return;
}

