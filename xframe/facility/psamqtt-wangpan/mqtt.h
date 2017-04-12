/* mqtt.h, by Zhang Haibin, 2015-05-25
 */
#ifndef _MQTT_H_
#define _MQTT_H_

#include "socket.h"

//every CMqtt get a pointer to task
//so is every CMqttLink

//I think we need lots of change
//since we don't have the header file msgdef_uninet.h yet

//commented out by wangpan
// #include "msgdef_uninet.h"
#include "mqttlink.h"
#include "unihashtable.h"
#include "CommonTask.h"

class CMqtt
{
public:
   CMqtt();
   virtual ~CMqtt() {}
   virtual void list(CStr& str);
   virtual void buildFdSet(CFdSet& fdSet);
   virtual void procFdSet(CFdSet& fdSet);
   virtual void close();
   virtual BOOL procAppMsg(TUniNetMsg* msg);

   void serviceConfig(CStr& tag, int appKey);
   int getAppKey(CStr& tag);

   virtual CMqttLink* getLink(int id) { return NULL; }

   virtual void secAction(){};
   virtual void closeLink(int linkId){};
   virtual void reconnect(){};

   virtual void wantToSend(std::shared_ptr<TUniNetMsg> msg) {}
   virtual void onTimeOut(TTimerKey timerKey,void *para);

   void setTask(CommonTask *_task) {task=_task;}
   CommonTask *getTask() { return task;}
protected:
   void listServiceConfig(CStr& str);
   CommonTask *task;
private:

   int mDefaultAppKey;

   //commented out by wangpan
   //any func that use this ds are being commented out
   //CHashTable<CStr, int> mAppKeyMap;
};

#endif
