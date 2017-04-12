/****************************************************************
* Copyright (c)2011, by BUPT
* All rights reserved.
*      The copyright notice above does not evidence any
*      actual or intended publication of such source code

* FileName:     taskthread.h $
* System:       xframe
* SubSystem:    psa
* Author:       Li Jinglin
* Date：        2010.4.4
* Description:
		应用线程

****************************************************************/

#if !defined(_TASKTHREAD_H_)
#define _TASKTHREAD_H_

#include "kernalthread.h"
#include "taskcontainer.h"


class TTaskThread: public TGeneralThread
{
public:
	TTaskThread(TTaskContainer& task, const char* tname, int tLogType = File, int tLogLevel = Info, UINT inst = 0);
	~TTaskThread();
	
	virtual void onstart();
	void process();

protected:
	TTaskContainer&  mTask;

};

#endif

