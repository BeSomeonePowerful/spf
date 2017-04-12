#ifndef MICRO_SAT_MOCK_H
#define MICRO_SAT_MOCK_H

#include <map>
#include "abstracttask.h"
#include "msgdef_com.h"
#include <memory>
#include "../mobutility.h"
#include "../mobstatemachine.h"

using std::map;

class MicroSatMock:public TAbstractTask
{
public:
  MicroSatMock();
  virtual ~MicroSatMock();
  TAbstractTask *clone() { return new MicroSatMock(); }
  BOOL onInit(TiXmlElement *extend);
  void procMsg(std::auto_ptr<TUniNetMsg> msg);
  void onTimeOut(TTimerKey timerKey,void *para);

  //need to call sm to make state transitions
  void sendToTask(TUniNetMsg *msg);
  void startAddrResolve(int userId);
  void startUaReg(int uaid,CStr uaip);
  void startUserReg(int userid,int uaid);
  //build a method that can start query
public:
  int rscpsaaddr;

  //combination of mainsatIP and port
  CStr mainsatIP;
  map<CStr,MobStateMachine *> smManager;

  RidGenerator *ridgenerator;
  TTimerKey addrResolveTimer;
  //in the test version of the cache manager,record is of no time stamp
  //map<> cacheManager;
  //not 127.0.0.1 but an ip addr used by mock rscpsamock to
  //distribute message
  CStr localIP;
};

#endif
