/****************************************************************
 * Copyright (c)2011, by BUPT
 * All rights reserved.
 *      The copyright notice above does not evidence any
 *      actual or intended publication of such source code

 * FileName:    kernalthread.h $
 * System:       xframe
 * SubSystem:    common
 * Author:       Li Jinglin
 * Date：        2010.4.4
 * Description:
		内核线程

****************************************************************/

#if !defined(_KERNALTHREAD_H_)
#define _KERNALTHREAD_H_

#include "generalthread.h"

_CLASSDEF(TFRAMEControl);
class TKernalThread: public TGeneralThread
{
public:
	TKernalThread(TFRAMEControl& kn);
	~TKernalThread();

        void process();
	void onstart();

protected:
	TFRAMEControl& mKn;
};

#endif

