#include "httpstackpsa.h"
#include "framemng.h"

char *appName = "PsaHttpTest2";

int main()
{
    threadEnvCreate();
    InitLogInfo(File, Info, 100000, "log/PsaHttpTest2_");
    
    TFRAMEManager manager;
    HttpStackPsa*   recvtask    = new HttpStackPsa();
    //TestHttpTask*  selftask    = new TestHttpTask();

    //manager.registerTask(109, selftask);
    manager.registerTask(109,  recvtask);

    manager.Init(1, appName);
    manager.Run();

    //if(selftask!=NULL) delete selftask;
    if(recvtask!=NULL) delete recvtask;

    return 0;
}


