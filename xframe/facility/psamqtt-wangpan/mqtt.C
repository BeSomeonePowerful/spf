/* mqtt.C, by Zhang Haibin, 2015-05-25
 */
#include "mqttserver.h"
#include "mqttclient.h"

//commented out by wangpan
// #include "uniframe.h"


//extern int g_thisPsaId;

// commented out by wangpan
// extern char uniEtcDir[];

//commented out by wangpan
//extern CMqtt* g_mqtt;

/*void mqttInit()
{
   char fileName[256];
   sprintf(fileName, "%s/psamqtt.cfg", uniEtcDir);
   FILE* f;
   if((f = fopen(fileName,"rt")) == NULL)
   {
      UniINFO("no file %s", fileName);
      return;
   }
   if(g_mqtt != NULL)
   {
      g_mqtt->close();
      delete g_mqtt;
      g_mqtt = NULL;
   }
   bool isServer = false;
   char buffer[1024];
   int line = 0;
   while(getFileLine(buffer, sizeof(buffer), f, &line)!=NULL)
   {
      CStr lineBuffer = buffer;
      lineBuffer.trim();
      CStr words[10];
      int wordCount = lineBuffer.split(words, sizeof(words));
      if(words[0].toInt() != g_thisPsaId)
         continue;
      if(wordCount == 5 && words[1] == "server")
      {
         if(g_mqtt != NULL)
         {
            UniERROR("%s, line %d, mqtt has been initialized, ignore this line", fileName, line);
            continue;
         }
         isServer = true;
         if(words[2] == "tcp")
            g_mqtt = new CMqttServerTcp();
         else if(words[2] == "udp")
            g_mqtt = new CMqttServerUdp();
         else
         {
            UniERROR("%s, line %d, invalid protocol name", fileName, line, words[2].c_str());
            continue;
         }
         // 0: psaid
         // 1: server
         // 2: tcp or udp
         // 3: server ip
         // 4: server port
         // 5: app key
         ((CMqttServer*)g_mqtt)->init(words[3], words[4].toInt());
         continue;
      }
      if(wordCount == 9 && words[1] == "client")
      {
         if(g_mqtt != NULL)
         {
            UniERROR("%s, line %d, mqtt has been initialized, ignore this line", fileName, line);
            continue;
         }
         isServer = false;
         if(words[2] == "tcp")
            g_mqtt = new CMqttClientTcp();
         else if(words[2] == "udp")
            g_mqtt = new CMqttClientUdp();
         else
         {
            UniERROR("%s, line %d, invalid protocol name", fileName, line, words[2].c_str());
            continue;
         }
         // 0: psaid
         // 1: server
         // 2: tcp or udp
         // 3: server ip
         // 4: server port
         // 5: client name
         // 6: user name
         // 7: passwd
         // 8: keep alive time
         ((CMqttClient*)g_mqtt)->init(words[3], words[4].toInt(), words[5], words[6], words[7], words[8].toInt());
         continue;
      }
      if(wordCount == 4 && words[1] == "service")
      {
         if(g_mqtt == NULL)
         {
            UniERROR("%s, line %d, must config a server or client first", fileName, line);
            continue;
         }
         g_mqtt->serviceConfig(words[2], words[3].toInt());
         continue;
      }
      UniERROR("%s, line %d, invalid format", fileName, line);
   }
}*/

CMqtt::CMqtt()
{
   mDefaultAppKey = 300;
}

void CMqtt::list(CStr& str)
{
}

void CMqtt::buildFdSet(CFdSet& fdSet)
{
}

void CMqtt::procFdSet(CFdSet& fdSet)
{
}


void CMqtt::close()
{
}

BOOL CMqtt::procAppMsg(TUniNetMsg* msg)
{
   int linkId = msg->tAddr.taskInstID;
   CMqttLink* link = getLink(linkId);
   if(link == NULL)
   {
      UniERROR("can not find a link %d to process app message", linkId);
      return FALSE;
   }
   return link->procAppMsg(msg);
}

void CMqtt::serviceConfig(CStr& tag, int appKey)
{
   if(tag == "*" || tag == "default")
   {
      mDefaultAppKey = appKey;
      return;
   }
   //mAppKeyMap.put(tag, appKey);
}

void CMqtt::listServiceConfig(CStr& str)
{
   str << "\n--- Service Config ---\n";
  // mAppKeyMap.reset();
   CStr tag;
   int appKey;
  //  while(mAppKeyMap.getNext(tag, appKey))
  //     str.fCat("%-20s : %d\n", tag.c_str(), appKey);
  //  str.fCat("%-20s : %d\n", "default", mDefaultAppKey);
}

int CMqtt::getAppKey(CStr& tag)
{
   int appKey;
  //  if(mAppKeyMap.get(tag, appKey))
  //     return appKey;
   return mDefaultAppKey;
}

// void CMqtt::wantToSend(TestMessage msg)
// {
//   cout << "###ERROR:calling CMqtt::wantToSend method" << endl;
// }

void CMqtt::onTimeOut(TTimerKey timerKey,void *para)
{
  cout << "##ERROR:calling CMqtt::onTimeOut method" << endl;
}
