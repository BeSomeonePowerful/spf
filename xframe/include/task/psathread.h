#if !defined(_PSATACK_THREAD_H_)
#define _PSASTACK_THREAD_H_

#include "generalthread.h"
#include "psacontainer.h"
#include "fdset.h"
#include <stdlib.h>

_CLASSDEF(TPSAThread);

class TPSAThread : public TGeneralThread
{
public:
	TPSAThread(TPSAContainer& task, const char* tname, int tLogType = File, int tLogLevel = Info, UINT inst = 0);
	~TPSAThread();

	void process();
	void onstart();

public:
	TPSAContainer&  mTask;
};

#endif

