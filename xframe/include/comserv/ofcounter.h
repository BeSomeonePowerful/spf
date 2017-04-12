/*********************************************************************
 * FileName：				ofcounter.h
 * Author：					李静林
 * Description：
		带流控的计数器，记录CAPS（请求数量/秒）出现的时间和取值
		支持快速限流和慢启动，可通过修改以下三个宏定义修改慢启动阈值
 *
 * Last Modified:
		2015-4-17 v1.2 李静林 增加checkOverflow参数，支持输入time_t时间，避免频繁time(0)
 *********************************************************************/

#ifndef _COUNTER_OVERFLOW
#define _COUNTER_OVERFLOW

#include <time.h>
#define FLOW_LIMITION_PERCENT 1.2
#define FLOW_SLOWSTART1_PERCENT 0.3
#define FLOW_SLOWSTART2_PERCENT 0.6

struct TOverFlowInfo
{
	int 	max_caps;
	time_t  max_caps_time;
};


class COverflow
{
private:
	time_t 	m_pre_time;	//上次一检查时间
	int 	m_pre_policy; //上一次处理策略

	int 	m_cur_caps;	//当前流量
	time_t 	m_cur_time;	//当前检查时间

	time_t 	m_max_caps_time;	//出现最大流量的时间
	int 	m_max_caps;	//最大流量

	int 	m_caps_limition;	//最大流量限制

	//流控阶段
	//	0=正常，未被流控；如超过流量限制，则拒绝本监控周期内的后继请求；
	//	1=流控阶段1，上一检测周期超过流量限制20%，进入慢启动1，限制阈值为1，间隔拒绝
	//	2=流控阶段2，上一检测周期为阶段1，限制阈值为2，间隔拒绝
	//	流控策略0 - 1 - 2 - 1 - 0
	int m_pd;			
	int m_pd_limition;
	int m_start1_limition;
	int m_start2_limition;
	int m_cur_limition;

	bool m_flow_control;
	
public:
	COverflow(int max_caps=0) 
	{
		m_caps_limition=max_caps; 
		m_cur_time=time(0); 
		m_pre_time=m_cur_time;
		m_max_caps_time=0;
		m_cur_caps=0; 
		m_max_caps=0;
		m_pd=0;
		m_pd_limition=int(m_caps_limition*FLOW_LIMITION_PERCENT);
		m_start1_limition=int(m_caps_limition*FLOW_SLOWSTART1_PERCENT);
		m_start2_limition=int(m_caps_limition*FLOW_SLOWSTART2_PERCENT);
		m_cur_limition=m_caps_limition;
		m_pre_policy=true;
		m_flow_control=false;
	};

	void resetThreshold(int max_caps=0) 
	{
		m_caps_limition=max_caps;
		m_pd_limition=int(m_caps_limition*FLOW_LIMITION_PERCENT);
		m_start1_limition=int(m_caps_limition*FLOW_SLOWSTART1_PERCENT);
		m_start2_limition=int(m_caps_limition*FLOW_SLOWSTART2_PERCENT);
		m_cur_limition=m_caps_limition;
	};
	
	//获得流控信息，每次获得流控信息后，最大流量和出现最大流量时间将被重置
	void getFlowInfo(TOverFlowInfo &info)
	{
		info.max_caps=m_max_caps;
		info.max_caps_time=m_max_caps_time;
		m_max_caps=0;
		m_max_caps_time=0;
	};
	
	//流控检测
	//TRUE: 通过检测
	//FALSE:检测失败，需要被流控
	bool checkOverflow(time_t 	currenttime=0);
	
};

#endif




