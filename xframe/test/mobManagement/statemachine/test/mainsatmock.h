#ifndef MAIN_SAT_MOCK_H
#define MAIN_SAT_MOCK_H

#include <map>
#include "abstracttask.h"
#include "msgdef_com.h"
#include <memory>
#include "../mobstatemachine.h"

using std::map;
using std::auto_ptr;

class MainSatMock:public TAbstractTask
{
public:
  MainSatMock();
  virtual ~MainSatMock();
  TAbstractTask *clone() { return new MainSatMock(); }
  BOOL onInit(TiXmlElement *extend);
  void procMsg(std::auto_ptr<TUniNetMsg> msg);
  void onTimeOut(TTimerKey timerKey,void *para);

  //need to call sm to make state transitions
  void sendToTask(TUniNetMsg *msg);
  //build a method that can start query
public:
  int rscpsaaddr;
  int nmeaddr;
  int rmeaddr;
  map<CStr,MobStateMachine *> smManager;

  //in the test version of the cache manager,record is of no time stamp
  //map<> cacheManager;
  //not 127.0.0.1 but an ip addr used by mock rscpsamock to
  //distribute message
  CStr localIP;
};

#endif
