#include "testselftask.h"
#include "testpsatask.h"
#include "framemng.h"

char *appName = "PsaTCPTest";

int main()
{
    threadEnvCreate();
    InitLogInfo(File, Debug, 100000, "log/PsaTCPTest_");
    
    TFRAMEManager manager;
    TestPsaTask*   recvtask    = new TestPsaTask();
    TestPsaTask*   sendtask    = new TestPsaTask();
    TestSelfTask*  selftask    = new TestSelfTask();

    manager.registerTask(1001, selftask);
    manager.registerTask(102,  recvtask);
    manager.registerTask(101,  sendtask);

    manager.Init(1, appName);
    manager.Run();

    if(selftask!=NULL) delete selftask;
    if(recvtask!=NULL) delete recvtask;
    if(sendtask!=NULL)  delete sendtask;

    return 0;
}


