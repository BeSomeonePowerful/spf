#if !defined(_TRANSPORT_THREAD_H_)
#define _TRANSPORT_THREAD_H_

#include "generalthread.h"
#include "fdset.h"
#include <stdlib.h>

_CLASSDEF(TTransport);
_CLASSDEF(TTransportThread);
class TTransportThread : public TGeneralThread
{
public:
	TTransportThread(TTransport& task, const char* tname, int tLogType = File, int tLogLevel = Info, UINT inst = 0);
	~TTransportThread();

	void process();
	void onstart();

public:
	TTransport&  mTask;
};

#endif

