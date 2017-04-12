#ifndef _TEST_TASK_HPP_
#define _TEST_TASK_HPP_

#include <iostream>

#include "msgdef_rsc.h"
#include "msgdef_com.h"
#include "abstracttask.h"

_CLASSDEF(TTestTask);
class TTestTask : public TAbstractTask
{
_COMPONETDEF(TTestTask);

public:
	TTestTask();
	virtual ~TTestTask();

	BOOL onInit(TiXmlElement*	extend);
	virtual BOOL reloadTaskEnv(CStr& cmd, TiXmlElement* & extend);

	void procMsg(std::auto_ptr<TUniNetMsg> msg);
	void onTimeOut(TTimerKey timerKey, void* para);

private:
	int _addr_;
};

#endif