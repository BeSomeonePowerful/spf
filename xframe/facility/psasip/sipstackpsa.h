/****************************************************************
 * Copyright (c)2011, by BUPT
 * All rights reserved.
 *      The copyright notice above does not evidence any
 *      actual or intended publication of such source code
 * FileName:     abstractpsa.h $
 * System:       xframe
 * SubSystem:    common
 * Author:       Li Jinglin
 * Date：        2010.4.4
 * Description:
		psa(Protocol Stack Adapter)协议栈的基类，所有的PSA都必须派生自该类。
 *
 * Last Modified:

**********************************************************************/

#ifndef __SIPSTACKPSA_H
#define __SIPSTACKPSA_H

#include "abstracttask.h"
#include "cmd5.h"
#include "ofcounter.h"
#include "func.h"

#include "sipdialogmng.h"

#include "msgconstdef_sip.h"
#include "msgdef_sip.h"
#include "msghelper_sip.h"
#include "msgdef_event.h"
#include "msghelper_event.h"

#include "resip/stack/TransactionUser.hxx"
#include "resip/stack/SipStack.hxx"
#include "resip/stack/StackThread.hxx"
#include "resip/stack/TransportThread.hxx"
#include "resip/stack/ConnectionTerminated.hxx"
#include "db.h"

#include <vector>
#include <iostream>

//#define __MAX_BINDS_ADDR 6
#define SQL_STATEMENT_LEN 1024
#define STATISTICLOG_TIME_CHAR_LENGTH 15

const int TIMER_PSASIP_INTERNAL_HEARTBEAT=1;
//对话管理的定时回收清理定时器，默认30秒
const int TIMER_PSASIP_RECYCLE_DIALOG=2;
//流控的定时状态回写定时器，默认10分钟（600000）
const int TIMER_PSASIP_FLOWSTATUS=3;
//对话的默认生存时间
const int DLG_LIVING_TIME=2000;

_CLASSDEF(TBinds)
class TBinds
{
public:
	resip::Data		mHost;
	resip::TransportType	mTransportType;
	int		mPort;
	resip::IpVersion	mIPVersion;
	resip::StunSetting	mStun;
	
public:
	TBinds():mHost("0.0.0.0"),mTransportType(resip::UDP),mPort(5060),mIPVersion(resip::V4),mStun(resip::StunDisabled) {}
	resip::Data&		sentHost() {return mHost;}
	resip::TransportType&	transport() {return  mTransportType;}
	int&			sentPort()  {return mPort;}
	resip::IpVersion&	protocolVersion() {return mIPVersion;}
	resip::StunSetting&	stun() {return mStun;}
};

_CLASSDEF(SIPSTACKPSA);
class SipStackPsa : public TAbstractTask, public resip::TransactionUser
{
   public:
      SipStackPsa();
      virtual ~SipStackPsa();
      const resip::Data& name() const; 

	  //重载TAbstractTask 的方法
	  virtual TAbstractTask* clone() { return new SipStackPsa(); }
	  virtual BOOL onInit(TiXmlElement*	extend);
	  virtual void procMsg(std::auto_ptr<TUniNetMsg> msg);
	  virtual void onTimeOut(TTimerKey timerKey, void* para);

	  virtual BOOL reloadTaskEnv(CStr& cmd, TiXmlElement* & extend);
      
	  //重载TransactionUser 的方法，将Message 消息构造为UninetMsg，添加到内部队列中 
      virtual void post(resip::Message* msg);
	virtual void postToTransactionUser(resip::Message* msg, resip::TimeLimitFifo<resip::Message>::DepthUsage usage);      

	  //基类方法，发送到kernal
	  //void	sendMsg(TUniNetMsg* msg);
	  //void	sendMsg(std::auto_ptr<TUniNetMsg> msg);
 
	  void sendSipMessage(resip::SipMessage* sipmsg);
	  BOOL checkSipMessage(resip::SipMessage* sipmsg);

	  void closeResources();
	  void printState(CStr* cStr);

	  resip::Uri&  	getLocalUri(){return mLocalUri;};
   	  resip::Via&  	getLocalVia(){return mLocalVia;};
	  resip::Data& 	getLocalRealm(){return mLocalRealm;};
	  bool	     	getIsLooseRoute() {return mIsLooseRoute;};
	  
	  //流控状态输出
	  void OverFlow();
	  //被流控时的处理
	  void OverFlowMakeResp(resip::SipMessage* sipmsg);
	  
   private:
	  resip::SipStack *mStack;
	  
	  BOOL transExist(resip::Data host, int port, resip::TransportType tt);

      CDB* db;	//mysql数据库
	  BOOL reloadDBEnv();	//加载数据库结构
	  CHAR m_cSQLStatement[SQL_STATEMENT_LEN];
 	  void cleanSQLStatement() { memset(m_cSQLStatement, 0, SQL_STATEMENT_LEN); };

	 UINT  mEventCSeq;	//event msg cseq
   protected:
	//move to msghelper_sip
	 // CStr computeDialogId(resip::Uri& from, resip::Uri& to, const char* callid);
	 // void addMsgBody(PTUniNetMsg uniMsg, resip::SipMessage* msg);
	 // void addCtrlMsgHdr(PTUniNetMsg uniMsg, resip::SipMessage* msg);
	  
   protected:

	  INT				mBindsCount;				//绑定地址数量
	  CList<TBinds> 	mBinds;
	  //TBinds		mBinds[__MAX_BINDS_ADDR];	//理论上可以设置多个绑定地址

	  resip::Uri 		mLocalUri;		
	  resip::Via		mLocalVia;	
	  resip::Data 		mLocalRealm;
	  bool				mIsLooseRoute;
	  
	  std::map<std::string, std::string> mDNSCache;	//DNS域名缓冲，在下发消息的时候，如果forceTarget填写的是域名，则用这个尝试获取一下缓存的ip地址
	  
	  CStr				mDid;	//当前处理消息的did

	  int				mRegRefreshInterval;	//注册心跳刷新时间
	  int				mCallRefreshInterval;	//呼叫心跳刷新时间
	  int				mTargetTask;	//默认消息发送对象
	  int				mRegTask;		//注册消息发送对象
	  int				mDetecTask;		//心跳检测消息发送对象

	  //回收did的定时器
	  int 	mRecycleTimerkey;
	  //流控状态刷新定时器
	  int	mFlowControlTimerkey;
	  
	  //std::vector<StackThread *> stackThreads;
	  //std::vector<TransportThread*> transportThreads;
	  //std::vector<TransportThread*> recvTransportThreads;

	  resip::StackThread * 			stackThread;

	  CSIPPsaDialog					mDialog;		// Dialog 管理 对象
	  
	  //TAbstractTask提供了currentTime ，每次处理消息的时候会更新,省得处理对话的时候多次更新
	  //time_t			mCurrenttime;

	  //TAbstractTask内部的消息队列
	  //Fifo<TMsg>* 	mTaskRxFifo;
	  
	  //消息量统计
	  int		mAllNum;
	  int		mInDialogInviteNum;
	  int		mOutDialogInviteNum;
	  int		mInDialogRegesterNum;
	  int		mOutDialogRegesterNum;
	  int		mOtherNum;
	  
	  //流控峰值记录，流控仅针对对话外的INVITE、REGISTER、其他对话外消息
	  COverflow	*	mInvite;
	  COverflow	*	mRegester;
	  COverflow	*	mOther;
	  
	  TOverFlowInfo mRegesterInfo;
	  TOverFlowInfo mInviteInfo;
	  TOverFlowInfo mOtherInfo;
};


#endif

