#include "testepoll.h"
//#include "testpsatask.h"
#include "framemng.h"

char *appName = "PsaEpollTest";

int main()
{
    threadEnvCreate();
    InitLogInfo(File, Info, 100000, "log/PsaEpollTest_");
    
    TFRAMEManager manager;
    //TestPsaTask*   recvtask    = new TestPsaTask();
    //TestPsaTask*   sendtask    = new TestPsaTask();
    TestEpollTask*  epolltask    = new TestEpollTask();

    manager.registerTask(1001, epolltask);
    //manager.registerTask(102,  recvtask);
    //manager.registerTask(101,  sendtask);

    manager.Init(1, appName);
    manager.Run();

    if(epolltask!=NULL) delete epolltask;
    //if(recvtask!=NULL) delete recvtask;
    //if(sendtask!=NULL)  delete sendtask;

    return 0;
}


