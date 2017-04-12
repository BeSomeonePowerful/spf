#ifndef _TEST_LOGICAL_TASK_HPP_
#define _TEST_LOGICAL_TASK_HPP_

#include <iostream>

#include "msgdef_rsc.h"
#include "msgdef_com.h"
#include "msgdatadef.h"
#include "abstracttask.h"

_CLASSDEF(TTestLogicalTask);
class TTestLogicalTask : public TAbstractTask
{
_COMPONETDEF(TTestLogicalTask);

public:
	TTestLogicalTask();
	virtual ~TTestLogicalTask();

	BOOL onInit(TiXmlElement*	extend);
	virtual BOOL reloadTaskEnv(CStr& cmd, TiXmlElement* & extend);
    virtual BOOL preTaskStart();

	void procMsg(std::auto_ptr<TUniNetMsg> msg);
	void onTimeOut(TTimerKey timerKey, void* para);

private:
	int _addr_;
};

#endif