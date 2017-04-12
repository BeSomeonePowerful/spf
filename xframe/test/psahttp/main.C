/*#include "../../facility/psahttp/httpstackpsa.h"*/
#include "testhttptask.h"
#include "framemng.h"

char *appName = "PsaHttpTest";

int main()
{
    threadEnvCreate();
    InitLogInfo(File, Info, 100000, "log/PsaHttpTest_");
    
    TFRAMEManager manager;
    //HttpStackPsa*   recvtask    = new HttpStackPsa();
    TestHttpTask*  selftask    = new TestHttpTask();

    manager.registerTask(109, selftask);
    //manager.registerTask(102,  recvtask);

    manager.Init(1, appName);
    manager.Run();

    if(selftask!=NULL) delete selftask;
    //if(recvtask!=NULL) delete recvtask;

    return 0;
}


