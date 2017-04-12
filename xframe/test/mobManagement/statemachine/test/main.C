#include "framemng.h"
#include "mainsatmock.h"
#include "microsatmock.h"
#include "nmemock.h"
#include "rscpsamock.h"

char *appName = "UserRegTest";
int main()
{

  //4 task
  //1 microsatmock,1 mainsatmock,1 nmemock,1 rscpsamock.
  threadEnvCreate();
  InitLogInfo(File, Info, 100000, "log/UserRegTest_");

  TFRAMEManager manager;

  MicroSatMock *microsatmock = new MicroSatMock;
  MainSatMock *mainsatmock = new MainSatMock;
  NmeMock *nmemock = new NmeMock;
  RscPsaMock *rscpsamock = new RscPsaMock;


  manager.registerTask(101,microsatmock);
  manager.registerTask(102,mainsatmock);
  manager.registerTask(103,nmemock);
  manager.registerTask(104,rscpsamock);

  manager.Init(1,appName);
  manager.Run();

  //microsatmock->startAddrResolve(1);
  //microsatmock->startAddrResolve(2);

  if( microsatmock!=NULL ) delete microsatmock;
  if( mainsatmock!=NULL ) delete mainsatmock;
  if( nmemock!=NULL ) delete nmemock;
  if( rscpsamock!=NULL ) delete rscpsamock;

  return 0;
}
