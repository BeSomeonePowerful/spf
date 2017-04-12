#include "CommonTask.h"
#include "mqtt.h"

CommonTask::~CommonTask()
{

}

void CommonTask::onTimeOut(TTimerKey timerKey,void *para)
{
  //to do
  mqtt->onTimeOut(timerKey,para);
}

void CommonTask::onBuildFDSet(CFdSet &fdset)
{
  mqtt->buildFdSet(fdset);
}

void CommonTask::onCheckFDSet(CFdSet &fdset)
{
  mqtt->procFdSet(fdset);
}

void CommonTask::setMqtt(CMqtt *m)
{
  mqtt=m;
}
