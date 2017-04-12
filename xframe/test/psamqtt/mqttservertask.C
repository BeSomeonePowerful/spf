#include "mqttservertask.h"
#include "mqttserver.h"

MqttServerTask::MqttServerTask()
{
  mqtt = new CMqttServerTcp();
  mqtt->setTask(this);
}

MqttServerTask::~MqttServerTask()
{
  mqtt->close();
  delete mqtt;
}

BOOL MqttServerTask::onInit(TiXmlElement *extend)
{
  UniINFO("MqttServerTask::onInit method being called");
  TiXmlHandle handle(extend);
  TiXmlElement *task=NULL;
  task=handle.FirstChild("task").Element();
  if( task )
  {
    const char *tmp = task->Attribute("localIP");
    if( tmp==NULL || strlen(tmp)==0 )
      UniERROR("Uninitialized localIP");
    localIP << tmp;
    if( !task->Attribute("localPort",&localPort))
      UniERROR("Uninitialized localPort");
  }
  UniINFO("localIP: %s,localPort: %d",localIP.c_str(),localPort);
  mqtt->init(localIP,localPort);
  return TRUE;
}

void MqttServerTask::sendToTask(TUniNetMsg *msg,int linkid)
{
  msg->tAddr.logAddr = 1001;
  msg->dialogType = DIALOG_BEGIN;
  TComCtrlMsg *ctrl = new TComCtrlMsg();
  ctrl->status = linkid;
  msg->setCtrlMsgHdr(ctrl);
  sendMsg(msg);
}

void MqttServerTask::procPub(TUniNetMsg *msg)
{
  UniINFO("MqttServerTask::procPub method being called");
  TMqttPublish *arg = (TMqttPublish *)msg->msgBody;
  set<int> subers = subDict[arg->topic];
  UniINFO("MqttServerTask::procPub: subers: %d",subers.size());
  mqtt->procPub(msg,subers);
}

void MqttServerTask::procSub(TUniNetMsg *msg,int linkid)
{
  UniINFO("MqttServerTask::procSub method being called");
  TMqttSubscribe *arg = (TMqttSubscribe *)msg->msgBody;
  CList<CStr> topic = arg->topic;
  for(CList<CStr>::iterator iter = topic.begin() ; iter!=topic.end() ; ++iter)
  {
    UniINFO("MqttServerTask::procSub: topic: %s,linkid: %d",(*iter).c_str(),linkid);
    subDict[*iter].insert(linkid);
    // if( subDict.find(*iter)!=subDict.end() )
    // {
    //   subDict[*iter].insert(linkid);
    // }
    // else
    // {
    //   subDict[*iter]
    // }
  }
}

void MqttServerTask::procUnsub(TUniNetMsg *msg,int linkid)
{
  TMqttSubscribe *arg = (TMqttSubscribe *)msg->msgBody;
  CList<CStr> topic = arg->topic;
  for(CList<CStr>::iterator iter = topic.begin() ; iter!=topic.end() ; ++iter)
  {
    if( subDict.find(*iter)!=subDict.end() )
      subDict[*iter].erase(linkid);
  }
}

void MqttServerTask::procMsg(std::auto_ptr<TUniNetMsg> msg)
{
  TUniNetMsg *unimsg = msg.get();
  if( unimsg!=NULL )
  {
    if( unimsg->oAddr.logAddr==1001 && unimsg->hasMsgBody() )
    {
      //message received from task need to be distributed to link
      mqtt->procMsg(unimsg);
    }
    if( unimsg->oAddr.logAddr==1003 && unimsg->hasMsgBody() )
    {
      UniINFO("MqttServerTask:: received from idletask");
    }
  }
}

void MqttServerTask::onTimeOut(TTimerKey timerKey,void *para)
{
  UniINFO("MqttServerTask::onTimeOut method being called");
}

void MqttServerTask::onBuildFDSet(CFdSet &fdset)
{
  mqtt->buildFdSet(fdset);
}

void MqttServerTask::onCheckFDSet(CFdSet &fdset)
{
  mqtt->procFdSet(fdset);
}
