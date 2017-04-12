#include "clientselftask.h"
#include "serverselftask.h"
#include "mqttclienttask.h"
#include "mqttservertask.h"
#include "idletask.h"
#include "subtask.h"
#include "subselftask.h"
#include "framemng.h"

char *appName = "PsaTCPTest";

int main()
{
  threadEnvCreate();
  InitLogInfo(File, Info, 100000, "log/PsaTCPTest_");

  TFRAMEManager manager;
  ServerSelfTask *serverSelfTask = new ServerSelfTask();
  ClientSelfTask *clientSelfTask = new ClientSelfTask();
  MqttServerTask *mqttServerTask = new MqttServerTask();
  MqttClientTask *mqttClientTask = new MqttClientTask();
  IdleTask *idleTask = new IdleTask();
  SubTask *subTask = new SubTask();
  SubSelfTask *subSelfTask = new SubSelfTask();

  manager.registerTask(1001,serverSelfTask);
  manager.registerTask(1002,clientSelfTask);
  manager.registerTask(1003,idleTask);
  manager.registerTask(1004,subSelfTask);
  manager.registerTask(101,mqttServerTask);
  manager.registerTask(102,mqttClientTask);
  manager.registerTask(104,subTask);

  manager.Init(1,appName);
  manager.Run();

  if( serverSelfTask!=NULL ) delete serverSelfTask;
  if( clientSelfTask!=NULL ) delete clientSelfTask;
  if( subSelfTask!=NULL ) delete subSelfTask;
  if( mqttServerTask!=NULL ) delete mqttServerTask;
  if( mqttClientTask!=NULL ) delete mqttClientTask;
  if( idleTask!=NULL ) delete idleTask;
  if( subTask!=NULL ) delete subTask;

  return 0;
}
