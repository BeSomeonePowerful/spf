#if !defined(__PSA_RSC_TASK_HXX)
#define __PSA_RSC_TASK_HXX 

#include <map>
#include "abstracttask.h"
#include "transstate.h"
#include "transportudp.h"
#include "transportthread.h"
#include "msgdef_rsc.h"
#include "msgdef_com.h"


const int TIMER_RESEND_TIMEOUT=1;	//重发定时器，建议设置为500（500ms）
const int TIMER_MAINTAIN_STATE=2;	//状态维护定时器，建议设置为5000（5秒）

const int DEFAULT_MSG_SEND_TIMEOUT=10; 		//默认发消息超时时间为10秒
const int DEFAULT_DLG_LIVING_TIME=60;		//默认与应用的对话维护时间为60秒
const int DEFAULT_BIND_PORT=8000;
const int DEFAULT_BUFFER_SIZE=8192;

struct SBind
{
	int 	bindId;
	CStr    host;
	int		port;
};

struct STask
{
	int    taskid;
	CStr   regx;
};

_CLASSDEF(TPsaRscTask);
class TPsaRscTask : public TAbstractTask, public TTransMachine
{
_COMPONETDEF(TPsaRscTask);

public:
	TPsaRscTask();
	virtual ~TPsaRscTask();

	BOOL onInit(TiXmlElement*	extend);
	virtual BOOL reloadTaskEnv(CStr& cmd, TiXmlElement* & extend);

	void procMsg(std::auto_ptr<TUniNetMsg> msg);
	void onTimeOut(TTimerKey timerKey, void* para);

	//继承自TTransMachine，管理事务状态机
	virtual TTimerKey	setResendTimer(char* transid);		//设置重发定时器
	virtual BOOL		delResendTimer(TTimerKey tk);		//设置取消定时器
	
private:

	BOOL reloadDNSCache(TiXmlElement* & extend);
	BOOL reloadOtherCfg(TiXmlElement* & extend);
	SBind&	transExist(CStr& dest);		//判断使用哪一个bind地址发送消息

	UINT  								mCSeq;			//cseq序号
	int								mTaskDefault;	//默认taskid（如果mTaskMapping为空的话）
	CList<STask>  						mTaskMapping;	//taskid与ruri映射，目前采用beginWith的方式提取
	std::map<UINT, SBind> 				mBindList;		//绑定地址前缀列表，用于查询
	std::map<std::string, std::string> 	mDNSCache;		//DNS域名缓冲，在下发消息的时候，如果forceTarget填写的是域名，则用这个尝试获取一下缓存的ip地址

	TTimerKey  mMaintain;	//状态维护的定时器

	TUDPTrasnport*		mTransport;			//传输层对象
	TTransportThread*	mTransportThread;	//传输层线程
	
	CHAR				 mBuffer[DEFAULT_BUFFER_SIZE];	//消息编解码缓冲

};


#endif
