#ifndef NME_MOCK_H
#define NME_MOCK_H

#include <map>
#include "abstracttask.h"
#include "msgdef_com.h"
#include <memory>
#include "../mobstatemachine.h"

using std::map;

class NmeMock:public TAbstractTask
{
public:
  NmeMock();
  virtual ~NmeMock();
  TAbstractTask *clone() { return new NmeMock(); }
  BOOL onInit(TiXmlElement *extend);
  void procMsg(std::auto_ptr<TUniNetMsg> msg);
  void onTimeOut(TTimerKey timerKey,void *para);

  //need to call sm to make state transitions
  void sendToTask(TUniNetMsg *msg);
  //void startAddrResolve(int userId);
  //build a method that can start query
private:
  int rscpsaaddr;
  int cmeaddr;
  map<CStr,MobStateMachine *> smManager;

  //ip in the following two mean the ip of mainsat
  map<int,CStr> userid_ip_mapping;
  map<int,CStr> uaid_ip_mapping;
  map<int,int> userid_uaid_mapping;
  map<int,CStr> uaid_uaip_mapping;
  //in the test version of the cache manager,record is of no time stamp
  //map<> cacheManager;
  //not 127.0.0.1 but an ip addr used by mock rscpsamock to
  //distribute message
  CStr localIP;
};

#endif
