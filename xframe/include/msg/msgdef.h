/****************************************************************
 * Copyright (c)2005, by Beijing TeleStar Network Technology Company Ltd.(MT2)
 * All rights reserved.
 *      The copyright notice above does not evidence any
 *      actual or intended publication of such source code

 * FileName:    msgdef.h
 * Version:        1.0
 * Author:      Li Jinglin
 * Date:        2011.2.10
 * Description:
 * Last Modified:
****************************************************************/

#if !defined __MSGDEF_H
#define __MSGDEF_H

#include <strstream>

#include "comtypedef.h"
#include "comtypedef_vchar.h"
#include "msg.h"
#include "msgdatadef.h"
#include "msgutil.h"
#include "msgnamedef.h"
#include "cmd5.h"

_CLASSDEF(CGFSM)
using namespace std;


//typedef UINT  TMsgType;           //消息类型的数据类型定义. 与TMsgType含义同.
typedef BYTE  TDialogType;        //对话类型的数据类型定义

const TDialogType DIALOG_MESSAGE      =0;	//通知类消息，不会创建对话，一收一发
const TDialogType DIALOG_BEGIN        =1;	//创建对话消息
const TDialogType DIALOG_CONTINUE     =2;	//对话内消息
const TDialogType DIALOG_END          =3;	//结束对话消息
const TDialogType DIALOG_BROADCAST    =4;	//广播类消息，向所有Task的实例分发
const TDialogType DIALOG_MULTICAST    =5;	//多播类消息，向相同Task的实例分发

/*======================================================*/
//通用可选参数设置与存在判断宏
//使用该宏需要注意以下问题：
//1、消息头和消息体所有变量命名规则：小写字母开头，连续字串，第二个单词之后的单词首字母大写。例如：oAddr，tAddr
//2、可选参数一定命名为 optionSet
//3、可选参数Flag标识命名为 “可选参数名_flag” 。例如：oAddr_flag, tAddr_flag
/*=======================================================*/
#ifndef HAS_OPTION_PARA
#define HAS_OPTION_PARA(ParaName)\
        (optionSet & ParaName##_flag)
#endif

#ifndef SET_OPTION_PARA
#define SET_OPTION_PARA(ParaName);\
        optionSet |= ParaName##_flag;
#endif

/*======================================================*/
// 二进制消息解码宏，对控制消息头和消息体解码
// 在msglib_xx.C中被调用
/*======================================================*/
#ifndef DECODE_CTRLHDR
#define DECODE_CTRLHDR(CTRLMSG)\
   if(msg->optionSet & TUniNetMsg::ctrlMsgHdr_flag )\
    {\
        P##CTRLMSG  ctrlMsg = new CTRLMSG;\
        ctrlMsg->decode(buf);\
        msg->ctrlMsgHdr = ctrlMsg;\
    }
#endif

#ifndef DECODE_MSGBODY
#define DECODE_MSGBODY(MSGBODY)\
    if(msg->optionSet & TUniNetMsg::msgBody_flag )\
    {\
        P##MSGBODY msgBody = new MSGBODY;\
        msgBody->decode(buf);\
        msg->msgBody = msgBody;\
    }
#endif

//==========================================================
// 消息类定义

/*======================================================*/
//通用消息定义
//  所有SSC使用的消息都将由以下三部分构成：通用消息头，控制消息头和消息体
//控制消息头用来保存会话的两方需要用到的一些关键参数，如一些标识性
//的ID值。
/*=======================================================*/
_CLASSDEF(TUniNetMsg);
_CLASSDEF(TCtrlMsg);
_CLASSDEF(TMsgBody);

class TUniNetMsg :public TMsg
{
public:
  UINT              optionSet;    //可选参数的指示

  TMsgAddress       oAddr;        //源地址，缺省不用填写
  TMsgAddress       tAddr;        //目的地址，至少填写目的端逻辑地址填写目的子模块/模块标识，
  TDialogType       dialogType;   //DIALOG_BEGIN对话开始, DIALOG_CONTINUE对话过程中, DIALOG_END对话结束
  TUniNetMsgName    msgName;      //消息名
  TUniNetMsgType    msgType;      //消息类型
  CStr				transID;	  //事务id
  UINT				cseq;		  //消息序号
  PTCtrlMsg         ctrlMsgHdr;   //控制消息头
  PTMsgBody         msgBody;      //消息体

  enum
  {
    ctrlMsgHdr_flag  =0x00000001,  //包含控制消息头
    msgBody_flag     =0x00000002,  //包含消息体
	cseq_flag        =0x00000004,  //包含消息序号
	transid_flag	 =0x00000008   //包含事务id
  };

  inline              TUniNetMsg();
  inline              TUniNetMsg(const TUniNetMsg &r);
  virtual inline      ~TUniNetMsg();
  inline              TUniNetMsg &operator=(const TUniNetMsg &r);

  inline UINT         hasCtrlMsgHdr();
  inline UINT         hasMsgBody();
  inline UINT         setCtrlMsgHdr();
  inline UINT         setMsgBody();
  inline UINT         setCtrlMsgHdr(PTCtrlMsg ctrl);
  inline UINT         setMsgBody(PTMsgBody body);
  inline PTCtrlMsg    getCtrlMsgHdr();
  inline PTMsgBody    getMsgBody();

  //如果应用需要设置cseq，则应用需要自行维护一个唯一值
  inline UINT       hasCSeq();
  inline UINT       getCSeq(); 
  inline void       setCSeq(UINT seq); 

  //如果应用需要设置transactionid，则应用需要自行维护一个唯一串，（可基于应用的hostid、instid、taskid+seq进行md5计算，或直接拼接在一起）
  inline UINT       hasTransId();
  inline CStr&      getTransId(); 
  inline void      getTransId(CStr& tid); 
  inline void      setTransId(CStr& tid); 
  inline void      setTransId(const char* tid); 
  inline void      setTransId(); //如果不输入任何参数，则根据oAddr、消息名、消息类型、seq计算md5摘要
  
  inline PTMsg        clone();         
  inline PTMsgPara    cloneCtrlMsg();

  BOOL                operator==(TUniNetMsg&);

  INT                 encode(CHAR* &buf);
  INT                 decode(CHAR* &buf);
  INT                 size();
  void                print(ostrstream& st);
  void                brief(CStr& brief);
  //这个方法在msglib.C中被调用，只用来填充TMsg的消息头(oAddr,tAddr,dialogType,msgName,msgType几个域)。
  //而ctrlMsg和msgBody域需要调用其他方法生成（见msglib.C)
  virtual BOOL        decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);

};

/*======================================================*/
//控制消息头
//  控制消息头用来保存会话的两方需要用到的一些关键参数，如一些标识性
//的ID值。
//
//如果目前控制消息头中的indexID不足以满足消息处理的需要，则各个接口可以自行扩展
//控制消息头，扩展的方法为继承TCtrlMsg。
//
//TxxCtrlMsg :public TCtrlMsg
//{
//public:
//  xxId=;
//}
//
//同时，在构造TUniNetMsg的时候，控制消息头使用TxxCtrlMsg生成的对象。
/*=======================================================*/
class TCtrlMsg :public TMsgPara
{
public:
  UINT              optionSet;    //可选参数的指示
  TMsgAddress	    orgAddr;

  inline TCtrlMsg()
  {
      optionSet=0;
  };

  virtual ~TCtrlMsg(){};  //这个析构函数必须是虚的，才能保证在析构的时候能够调用子类的析构函数
  enum {
    orgAddr_flag  =0x00000001,  //???
    session_flag  =0x00000002
  };
  inline UINT       hasOrgAddr();
  inline UINT       setOrgAddr();
  inline UINT       setOrgAddr(TMsgAddress& org);

  //add by LJL 20160714 ----------------------------------------------------------------------
  //如果应用需要自行设置sessionid，则应用需要重载 getSessionId，可以是整数，也可以是字符串
  //重载的消息里面，在设置sessionid的时候，需要调用一下 setSessionId ，好让系统知道设置了sid
  inline UINT       hasSessionId() {return optionSet&session_flag;};
  virtual UINT      getSessionId() { return 0; } 
  virtual UINT      getSessionId(CStr& sid) { return 0; }
  virtual void      setSessionId(CStr& sid) { setSessionId(); } 
  virtual void      setSessionId(UINT sid) { setSessionId(); } 
  virtual void      setSessionId() { optionSet |=session_flag; } 
  //------------------------------------------------------------------------------------------
  
  virtual INT       encode(CHAR* &buf);
  virtual INT       decode(CHAR* &buf);
  virtual BOOL      decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm);
  virtual INT       size();
  virtual void      print(ostrstream& st);

  virtual PTCtrlMsg clone()=0; //这个函数的返回指针必须是基类的
  PTMsgPara         cloneMsg(){ return clone();}
  virtual BOOL      operator==(TMsgPara&){ return FALSE;}
};

/*======================================================*/
//消息体
//  消息体是消息的核心内容，每一个消息体必须包含一些方法
//
//  每个消息体的定义都必须继承TMsgBody，并实现其所定义的纯虚方法。
//
//TxxMsg :public TMsgBody
//{
//public:
//  xxId=;
//}
//
//同时，在构造TUniNetMsg的时候，消息体使用TxxMsg生成的对象。
/*=======================================================*/
class TMsgBody :public TMsgPara
{
public:
  UINT              optionSet;    //可选参数的指示           //Modify by LJL 2006-3-21

  TMsgBody()
  {
      optionSet=0;        //Modify by LJL 2006-3-21
  };

  virtual ~TMsgBody(){}; ////这个析构函数必须是虚的，才能保证在析构的时候能够调用子类的析构函数

  virtual INT        encode(CHAR* &buf)=0;
  virtual INT        decode(CHAR* &buf)=0;
  virtual BOOL       decodeFromXML(TiXmlHandle& xmlParser,PCGFSM fsm){return FALSE;}
  virtual INT        size()=0;
  virtual void       print(ostrstream& st)=0;

  virtual PTMsgBody  clone()=0;
  virtual BOOL       operator==(TMsgPara&){return FALSE;}

  PTMsgPara          cloneMsg(){ return clone();}

};


/*******************************************************************/
// clone 克隆函数
//  克隆函数的意义是为了完成消息体的复制；
//  克隆函数没有输入
//  克隆函数的输出为 自身类型的指针，该指针指向了自身类型的类对象，
//    内容与自身完全一致。
/*******************************************************************/

/*======================================================*/
//消息结构的具体实现
/*======================================================*/
//TUniNetMsg的实现
inline TUniNetMsg::TUniNetMsg()
{
  dialogType=DIALOG_BEGIN;
  msgName=0;
  msgType=1;    //取值为0时表示此消息为需要Kernel处理的消息，如SP的业务管理消息
  optionSet=0;  //缺省不包含控制消息头和消息体

  ctrlMsgHdr=NULL;
  msgBody=NULL;

}

inline TUniNetMsg::TUniNetMsg(const TUniNetMsg &r)
{
	sender=r.sender;
	
  oAddr=r.oAddr;
  tAddr=r.tAddr;
  dialogType=r.dialogType;
  msgName=r.msgName;
  msgType=r.msgType;
  cseq=r.cseq;
  transID=r.transID;

  optionSet=r.optionSet;

  if(hasCtrlMsgHdr())
  {
    ctrlMsgHdr=r.ctrlMsgHdr->clone();
  }
  else
  {
    ctrlMsgHdr=NULL;
  }

  if(hasMsgBody())
  {
        msgBody=r.msgBody->clone();
  }
  else
  {
    msgBody=NULL;
  }

}


inline TUniNetMsg::~TUniNetMsg()
{
  if(hasCtrlMsgHdr() && NULL!=ctrlMsgHdr)
  {
    delete ctrlMsgHdr;
  }

  if (hasMsgBody() && NULL!=msgBody)
  {
    delete msgBody;
  }
  ctrlMsgHdr=NULL;
  msgBody=NULL;
}


inline TUniNetMsg & TUniNetMsg::operator=(const TUniNetMsg &r)
{
	sender = r.sender;
	
  oAddr=r.oAddr;
  tAddr=r.tAddr;
  dialogType=r.dialogType;
  msgName=r.msgName;
  msgType=r.msgType;
  cseq=r.cseq;
  transID=r.transID;

  optionSet=r.optionSet;

  if(hasCtrlMsgHdr())
  {
    if (ctrlMsgHdr!=NULL)
    {
      delete ctrlMsgHdr;
      ctrlMsgHdr=NULL;
    }
    if(r.ctrlMsgHdr!=NULL) ctrlMsgHdr=r.ctrlMsgHdr->clone();
  }
  else
  {
    ctrlMsgHdr=NULL;
  }

  if(hasMsgBody())
  {
    if (msgBody!=NULL)
    {
      delete msgBody;
      msgBody=NULL;
    }
    if(r.msgBody!=NULL) msgBody=r.msgBody->clone();
  }
  else
  {
    msgBody=NULL;
  }
  return *this;
}


inline UINT       TUniNetMsg::hasCSeq()
{ 
	return optionSet&cseq_flag;
}

inline UINT       TUniNetMsg::getCSeq() 
{ 
	if(hasCSeq()) return cseq; 
	else return 0; 
}

inline void       TUniNetMsg::setCSeq(UINT seq) 
{ 
	cseq=seq; 
	optionSet |=cseq_flag;
} 

inline UINT       TUniNetMsg::hasTransId()
{ 
	return optionSet&transid_flag;
}

inline CStr&      TUniNetMsg::getTransId()
{ 
	return transID; 
} 

inline void      TUniNetMsg::getTransId(CStr& tid) 
{ 
	tid=transID; 
} 

inline void      TUniNetMsg::setTransId(CStr& tid) 
{ 
	if(tid.size()) 
	{ 
		transID=tid; 
		optionSet |=transid_flag;
	} 
} 

inline void      TUniNetMsg::setTransId(const char* tid) 
{ 
	transID=tid; 
	if(transID.size()) 
	{ 
		optionSet |=transid_flag; 
	} 
} 

inline void  TUniNetMsg::setTransId() 
{ //如果不输入任何参数，则根据oAddr、消息名、消息类型、seq计算md5摘要
	if(!hasCSeq()) return;
	CMD5 md5;
	CStr tmp;
	tmp<<oAddr.logAddr<<"/"<<oAddr.phyAddr<<"/"<<oAddr.taskInstID<<"/"<<msgName<<"/"<<msgType<<"/"<<cseq;
	const UCHAR* dt=(UCHAR*)(tmp.str());
	transID=(char*)md5.GetDigest(dt, tmp.size(),1);
	delete dt;
	optionSet |= transid_flag; 
} 


inline UINT TUniNetMsg::hasCtrlMsgHdr()
{
  return optionSet&ctrlMsgHdr_flag;
}

inline UINT TUniNetMsg::hasMsgBody()
{
  return optionSet&msgBody_flag;
}

inline UINT TUniNetMsg::setCtrlMsgHdr()
{
  return optionSet |= ctrlMsgHdr_flag;
}

inline UINT TUniNetMsg::setMsgBody()
{
  return optionSet |= msgBody_flag;
}

inline UINT TUniNetMsg::setCtrlMsgHdr(PTCtrlMsg ctrl)
{
	if(ctrl==NULL) return 0;
	ctrlMsgHdr=ctrl;
	return setCtrlMsgHdr();
}

inline UINT TUniNetMsg::setMsgBody(PTMsgBody body)
{
	if(body==NULL) return 0;
	msgBody=body;
	return setMsgBody();
}

inline PTCtrlMsg TUniNetMsg::getCtrlMsgHdr()
{
	if(hasCtrlMsgHdr() && ctrlMsgHdr!=NULL) return ctrlMsgHdr;
	else return NULL;
}

inline PTMsgBody TUniNetMsg::getMsgBody()
{
	if(hasMsgBody() && msgBody!=NULL) return msgBody;
	else return NULL;
}
  
inline PTMsg  TUniNetMsg::clone()
{
   PTUniNetMsg msg = new TUniNetMsg();
    *msg = *this;
//CtrlHdr和MsgBody已经在拷贝构造函数里面赋过值了，这里再赋值就会内存泄漏
    return msg;
}


inline PTMsgPara  TUniNetMsg::cloneCtrlMsg()
{
    if(hasCtrlMsgHdr())
    {
        if(ctrlMsgHdr!=NULL)
        {
            return ctrlMsgHdr->clone();
        }
    }
    return NULL;
}

inline UINT TCtrlMsg::hasOrgAddr()
{
  return optionSet&orgAddr_flag;
}

inline UINT TCtrlMsg::setOrgAddr()
{
  return optionSet |=orgAddr_flag;
}

inline UINT TCtrlMsg::setOrgAddr(TMsgAddress& org)
{
	orgAddr = org;
  return optionSet |=orgAddr_flag;
}

#endif
