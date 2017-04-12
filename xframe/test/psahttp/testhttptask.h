#if !defined(__TestHttpTask_HXX)
#define __TestHttpTask_HXX 

#include "comserv/comtypedef.h"
#include "task/abstracttask.h"
#include "httpserver.h"
/*#include "msgdef_http.h"
#include "msghelper_http.h"*/
#include "tbnet.h"
#include <map>

_CLASSDEF(TestHttpTask);
class TestHttpTask : public TAbstractTask
{
public:
	TestHttpTask();
	virtual ~TestHttpTask();

	TAbstractTask* clone(){ return (TAbstractTask*)( new TestHttpTask());}
	BOOL onInit(TiXmlElement*	extend);

	void procMsg(std::auto_ptr<TUniNetMsg> msg);
	void onTimeOut(TTimerKey timerKey, void* para);
	int mCount;
	TTimerKey mTK1;
	TTimerKey mTK2;

private:
	HttpServer * mServer;//协议栈实例
	//int mNotifyTask;    //目标任务id
    //DefaultHttpPacketFactory *_factory;
    map<string,int> dispatcher; //URL 前缀分发表
};

#endif
