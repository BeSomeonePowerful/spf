#if !defined(__TRANSSTATE_HXX)
#define __TRANSSTATE_HXX 

#include "comtypedef.h"
#include "comtypedef_vchar.h"
#include "cexpirebuffer.h"
#include "msgdef_com.h"

//对话状态：0初始化/事务结束，1请求转发中，2产生临时响应 , 3事务结束
enum TS_TRANSSTATE { TS_INIT=0, TS_FORWARD, TS_TEMPORARY, TS_TERMINATED};
//状态迁移的返回码： 0什么都不做，1正常转发，2消息拒绝，3消息超时，4向网络发送重发消息 
enum TS_CODE { TSC_NULL=0, TSC_FORWARD, TSC_REFUSE, TSC_TIMEOUT, TSC_RESEND};

_CLASSDEF(TTransState);
_CLASSDEF(TTransMachine);
class TTransState
{
public:
	TTransState(BOOL type, CStr& transid, TTransMachine* task):tssend(type),transID(transid), mTask(task),msg(NULL),cseq(0),tsstate(TS_INIT),tschange_from(TS_INIT),tschange(FALSE),timer(0) {};
	~TTransState();
	TS_CODE	TS_CHANGE(TUniNetMsg* msg, TUniNetMsg* &resp);	//输入消息，进行状态迁移
	TS_CODE	TS_CHANGE(TTimerKey key, TUniNetMsg* &resend);	//定时器超时，获取要重发的消息

	TS_TRANSSTATE	state() { return tsstate; }
	BOOL			isSend() {return tssend;}
	TUniNetMsg*     unimsg() {return msg;}

private:
	TS_CODE	TSClient_CHANGE(TUniNetMsg* msg, TUniNetMsg* &resp);	//客户端状态机（发消息）
	TS_CODE	TSServer_CHANGE(TUniNetMsg* msg, TUniNetMsg* &resp);	//服务端状态机（收消息）
	
	BOOL			tssend;			//是否发送消息事务，TRUE:发送消息；FALSE:接收消息
	TUniNetMsg* 	msg;			//缓冲的重发消息
	TS_TRANSSTATE	tsstate;		//当前状态
	TS_TRANSSTATE	tschange_from;	//迁移前的状态
	BOOL			tschange;		//是否产生状态变换
	TTimerKey		timer;			//重发定时器id
	int				cseq;			//事务序号
	CStr			transID;		//事务ID
	BOOL			needrsp;		//是否需要响应（ComMsg的pri参数）
	
	TTransMachine*	mTask;			//父对象指针
};

class TTransMachine
{
public:
	TTransMachine(){}
	virtual ~TTransMachine();
	//设置发送消息超时时间，单位：秒
	void	tminit(int sendTimeOutSec) { if(sendTimeOutSec) mTSState.setLivingTime(sendTimeOutSec);};
	
	//设置重发定时器, transid指针需要设置到setTimer的void* para中
	virtual TTimerKey	setResendTimer(char* transid)=0;
	virtual BOOL		delResendTimer(TTimerKey tk)=0;		//设置取消定时器

	//状态迁移处理：收到消息，进行状态迁移处理
	//fromwire =TRUE, 从网络收到的消息
	//         =FALSE,从应用收到的消息
	//msg  消息指针
	//     如果反馈 TSC_REFUSE, 则抛弃 msg
	//     如果反馈 TSC_FORWARD，则转发 msg
	//resp 状态迁移处理后反馈的消息指针
	//     如果返回值为 TSC_REFUSE，则向网络发送 resp
	//     如果返回值为 TSC_FORWARD，如果resp有值，则向网络发送
	TS_CODE				processTM(BOOL fromwire, TUniNetMsg* msg, TUniNetMsg* &resp);	
	//状态迁移处理：定时器超时，判断是否重发
	//tk     定时器反馈的timerkey
	//resend 如果需要重发，则会填写该重发消息指针，仅用于发送到网络上（如果返回值为 TSC_RESEND 有效）
	TS_CODE				processTM(TTimerKey tk, void* para, TUniNetMsg* &resend);	

	//状态机轮询处理：进行状态机清理
	//resp   如果发消息一直没有收到回复，超过 sendTimeOutSec ，将返回一个响应，仅用于发送给app（如果返回值为 TSC_TIMEOUT 有效）
	//       由于每次只返回一个，因此需要轮询处理，直到返回TSC_NULL
	TS_CODE				processTM(TUniNetMsg* &resp, const time_t currenttime=0);	
	
private:
	CExpireBuffer<CStr, TTransState*> mTSState;		//事务信息列表，key=md5(rid+cseq)
};

#endif
