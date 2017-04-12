/*
 *
 * http 消息接收task
 * 作者：wzt 
 * 时间：20140411
 * */
#ifndef __HTTPSTACKPSA_H
#define __HTTPSTACKPSA_H

#include "abstracttask.h"
#include "tbnet.h"
#include "httpserver.h"
#include "httpclient.h"
#include "httpthread.h"
#include <map>

#include "msgdef_http.h"
#include "msghelper_http.h"


using namespace tbnet;
class HttpStackPsa : public TAbstractTask, public IServerAdapter,public IPacketHandler{
	public:
		HttpStackPsa();
		virtual ~HttpStackPsa();

		virtual TAbstractTask* clone()
		{
			return new HttpStackPsa();
		} 
		//初始化函数
		virtual BOOL onInit(TiXmlElement* extend); 
		//消息处理函数
		virtual void procMsg(std::auto_ptr<TUniNetMsg> msg);
		//定时处理函数
		virtual void onTimeOut(TTimerKey timerKey, void* para);
		//从协议栈接收到消息将消息压入任务消息队列函数

		IPacketHandler::HPRetCode handlePacket(Connection *connection, Packet *packet);
		IPacketHandler::HPRetCode handlePacket(Packet *packet,void *args);
	private:
		HttpServer * mServer;//协议栈实例
		HttpClient * mClient;//客户端实例
		//int mNotifyTask;    //目标任务id
        //DefaultHttpPacketFactory *_factory;
        map<string,int> dispatcher; //URL 前缀分发表
        string getRequestLinePrefix(string); //取得URL（去掉参数）
        void sendHttpReq(HttpMessage* msg);
};
#endif
