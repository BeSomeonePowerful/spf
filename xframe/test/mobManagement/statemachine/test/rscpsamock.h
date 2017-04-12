#ifndef RSC_PSA_MOCK_H
#define RSC_PSA_MOCK_H

#include <map>
#include "abstracttask.h"
#include "msgdef_com.h"
#include <memory>

using std::map;

class RscPsaMock:public TAbstractTask
{
public:
  RscPsaMock();
  virtual ~RscPsaMock();
  TAbstractTask *clone() { return new RscPsaMock(); }
  BOOL onInit(TiXmlElement *extend);
  void procMsg(std::auto_ptr<TUniNetMsg> msg);
  void onTimeOut(TTimerKey timerKey,void *para);

  //need to call sm to make state transitions
  void sendToTask(TUniNetMsg *msg);
  void startAddrResolve(int userId);
  //build a method that can start query
private:
  //int rscpsaaddr;

  //combination of mainsatIP and port
  //CStr mainsatIP;
  map<CStr,int> taskManager;

  //RidGenerator *ridgenerator;
  //in the test version of the cache manager,record is of no time stamp
  //map<> cacheManager;
  //not 127.0.0.1 but an ip addr used by mock rscpsamock to
  //distribute message
  //CStr localIP;
};

#endif
