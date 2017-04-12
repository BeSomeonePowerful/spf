#ifndef _ENV_H
#define _ENV_H

#include <stdlib.h>     //include getenv()
#include <stdio.h>

#include <string.h>

#include "comtypedef.h"
#include "comtypedef_vchar.h"
#include "unihashtable.h"
#include "clist.h"
#include "cvector.h"
#include "tinyxml.h"
#include "defs.h"
#include "info.h"

_CLASSDEF(CDB);
_CLASSDEF(TTimerRsc);
class TTimerRsc
{
#define __TASK_TIMER_NUM	32

	public:
		TTimeMarkExt mTimerList[__TASK_TIMER_NUM];
		INT		 mTimerMarkList[__TASK_TIMER_NUM];

	public:
		TTimerRsc() {};
		~TTimerRsc() {};

		void list(CStr& result);
		TTimeMark getTimerIDbyName(char* name);

		void clear();
        TTimeMarkExt& operator[](const TTimeMark timerId);

};


struct TDBMarkExt
{
   INT        dbId;
   INT        dbType;
   CStr       dbUser;
   CStr       dbPass;
   CStr       dbName;
   CStr       dbHost;
   INT        dbPort;
};

_CLASSDEF(TDBEnv);
class TDBEnv
{
#define __TASK_DB_NUM 	32

	public:
		TDBMarkExt  mDBList[__TASK_DB_NUM];
		INT 		mDBMarkList[__TASK_DB_NUM];

	public:
		TDBEnv();
		~TDBEnv();

		UINT  load(TiXmlElement* dblistnode);
		void list(CStr& result);

		void clear();
		inline TDBMarkExt& operator[](const INT dbID);

};

_CLASSDEF(TDBRsc);
class TDBRsc
{
#define __TASK_DB_CONN_NUM 	6

	public:
		TDBMarkExt	mDBList[__TASK_DB_CONN_NUM];
		CDB*		mpDB[__TASK_DB_CONN_NUM];

	public:
		TDBRsc();
		~TDBRsc();
		BOOL conn(const INT dbidx);
		BOOL conn();
		BOOL disconn(const INT dbidx);
		void clear();
		CDB* operator[](const INT dbidx);
};


_CLASSDEF(TEnv);
class TEnv
{
	protected:
		TiXmlDocument* 	config;
		TiXmlElement* 	root;
		TiXmlElement*	timerrsc;

		TDBEnv 			mDBEnv;
		int				mLogType;			//Log����
		int				mLogLevel;			//Log�ȼ�
        CStr            mThreadName;        //�߳���

		int				mLogToSyslog;		//�����syslog���أ�Ĭ�ϲ����
		int				mSyslogTaskID;		//�������syslog��������Ҫ����syslog��taskid
		BOOL  loadCommonEnv(TiXmlElement* root);

	public:

		TEnv();
		~TEnv();
		//���������ļ�
		UINT load();
		//�̳е�����Ҫ����������μ����Զ�������
		virtual BOOL onLoad() {return TRUE;}
		//���������б�
		void list(CStr& result);
		//�̳е������������������ʾ����
		virtual void onList(CStr& result) {return;}

		virtual int getLogType() { return mLogType;}
		virtual int getLogLevel() { return mLogLevel; }
		int logToSyslog() { return mLogToSyslog; }
		int syslogTaskID() { return mSyslogTaskID; }
		virtual const char * getThreadName() {return mThreadName.c_str(); }
		virtual BOOL getTimerRsc(TTimerRsc& tm);


};

_CLASSDEF(TTaskEnv);
class TTaskEnv : public TEnv
{
private:
	INT mTaskID;
	TiXmlElement*	task;
	TiXmlElement*	log;
	TiXmlElement*	dbrsc;

public:
	TiXmlElement*	extend;
	TTaskEnv(UINT taskID) { mTaskID=taskID; task = log = dbrsc = NULL; }
	~TTaskEnv() {  }

	virtual BOOL onLoad();
	virtual void onList(CStr& result);
	virtual int getLogType();
	virtual int getLogLevel();

	BOOL getTaskInfo( INT&	taskType, CStr&	taskName);
	BOOL getDBRsc(TDBRsc& db);

};

//add by LJL һ����������֧�ֶ��task
_CLASSDEF(TTaskRsc);
class TTaskRsc
{
public:
	INT  taskID;
	INT  taskType;
	CStr taskName;
	CStr taskDll;
	TTaskRsc() {}
	~TTaskRsc() {}
};

_CLASSDEF(TProcRsc);
class TProcRsc
{
public:
	INT   threadNum;
	INT   instType;
	CStr  instName;
	CList<TTaskRsc> taskList;
	TProcRsc() {}
	~TProcRsc() {}
};


_CLASSDEF(TKernalEnv);
class TKernalEnv : public TEnv
{
	private:
		INT mAppID;
		CStr mAppName;
		CStr mHostIP;
		INT  mRecycleTime;	//Ĭ�ϵ�instӳ�����ʱ��/task����ʱ�䣬��������ã���7200��

		TiXmlElement* 	tasklist;
		TiXmlElement* 	proclist;
		TiXmlElement* 	sc;
		TiXmlElement* 	log;

	public:
		TKernalEnv() {tasklist = proclist = sc = log = NULL;};
		~TKernalEnv() {};

		virtual BOOL onLoad();
		virtual void onList(CStr& result);
		virtual int getLogType();
		virtual int getLogLevel();
		UINT getHostID() {return mAppID;}
		CStr getAppName() {return mAppName;}
		CStr getHostIP() {return mHostIP;}
		INT  getRecycleTime() { return mRecycleTime;}
		BOOL getProcList(CList<TProcRsc>& list);
		BOOL getSCInfo(CStr& scip, UINT& scport, UINT& hb);

};

_CLASSDEF(GeneralThreadEnv);
struct GeneralThreadEnv{
    const char * threadName;
    int logType;
    int logLevel;
};

//���������ϵͳ������ʱ���ʼ�������ڼ���ϵͳ�������ļ�·�����ڳ��������κεط��������޸ģ��Ա����̳߳�ͻ
extern char*	appName;


#endif
