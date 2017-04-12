#include <string.h>
#include <stdlib.h>

#include "psarsctask.h"
#include "TestTask.h"
#include "LogTask.h"

#include "framemng.h"

char *appName = "Testspf";

int main(int argc, char* argv[])
{
    threadEnvCreate();
    InitLogInfo(File, Info, 500, "log/Testspf_");
    
    TFRAMEManager* manager = NULL;
    LogTask* 	   log    = NULL;
    TPsaRscTask*   psa     = NULL;
    TTestTask* test = NULL;

    manager=    new TFRAMEManager();
    psa=        new TPsaRscTask();
    test = new TTestTask();
    log=       new LogTask();    
   // cout<<"check is null"<<endl;
    if(manager==NULL || psa==NULL || log==NULL || test==NULL)
    {
	UniERROR("Init Testspf Error!");
	if(manager) delete manager;
	if(psa) delete psa;
	if(test) delete test;
	if(log) delete log;

	exit(0);
    }

    //cout<<"manager regisertask logtask"<<endl;
    manager->registerTask(10, log);
    //cout<<"manager regisertask psa"<<endl;
    manager->registerTask(6,  psa);
    manager->registerTask(7,  test);
    
    //cout<<"Init"<<endl;
    manager->Init(1, appName);
    manager->Run();

    if(log!=NULL) delete log;
    if(psa!=NULL)  delete psa;
    if(test!=NULL)  delete test;
    return 0;
}