#include "selftask.h"

SelfTask::SelfTask()
{

}

SelfTask::~SelfTask()
{

}

BOOL SelfTask::onInit(TiXmlElement *extend)
{
  //init timer
}

void SelfTask::onTimeOut(TTimerKey timerKey,void *para)
{
  //how to fit a MqttMessage into UniNetMsg
}
