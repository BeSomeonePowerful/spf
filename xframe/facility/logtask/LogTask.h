#if !defined(__SYSLOGTASK_HXX)
#define __SYSLOGTASK_HXX 

#include "comtypedef.h"
#include "abstracttask.h"
#include <vector>

using namespace std;

#define MAX_FACILITY_NUM  33

_CLASSDEF(LogTask);
class LogTask : public TAbstractTask
{
public:
	LogTask();
	virtual ~LogTask();

	TAbstractTask* clone(){ return (TAbstractTask*)( new LogTask());}
	BOOL onInit(TiXmlElement*	extend);

	void procMsg(std::auto_ptr<TUniNetMsg> msg);
	
	BOOL reloadTaskEnv(CStr& cmd, TiXmlElement* & extend);

public:

	int  mFacility[MAX_FACILITY_NUM];
	int  mLogLevel;

	// info.h 中定义了 	MSGINFO、COUNTINFO、STATICINFO、ONLINEINFO 等几个标准输出，分别对应 logtype=1,2,3,4
	//  如果还需要定义新的输出，则可以直接用配置文件中的 <facility logtype="5" logname="ACTIVEUSERINFO" fid="21"/> 定义新的输出
	
};

#endif
