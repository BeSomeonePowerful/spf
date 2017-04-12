/* psamqtt.C, by Zhang Haibin, 2015-05-25
 */
#include "fdset.h"
#include "dyncomponent.h"
#include "msgdef_uninet.h"
#include "uniframe.h"
#include "abstractamehandler.h"
#include "unihashtable.h"
#include "mqtt.h"

void initPsaMqtt(INT psaid);

INIT_PSA_COMP(PsaMqtt)

int g_thisPsaId;
bool g_trace = false;
CMqtt* g_mqtt = NULL;
void mqttInit();

void hookFuncBuildFdSetMqtt(CFdSet& fdSet)
{
   if(g_mqtt == NULL)
   {
      UniERROR("mqtt not initialized");
      return;
   }
   g_mqtt->buildFdSet(fdSet);
}

BOOL hookFuncProcFdSetMqtt(CFdSet& fdSet)
{
   if(g_mqtt == NULL)
   {
      UniERROR("mqtt not initialized");
      return FALSE;
   }
   g_mqtt->procFdSet(fdSet);
   return TRUE;
}

void hookFuncProcCloseResource()
{
   if(g_mqtt == NULL)
   {
      UniERROR("mqtt not initialized");
      return;
   }
   g_mqtt->close();
}

BOOL hookSendMsgPsaMqtt(TUniNetMsg* msg)
{
   if(g_mqtt == NULL)
   {
      UniERROR("mqtt not initialized");
      return FALSE;
   }
   return g_mqtt->procAppMsg(msg);
}

void hookSecActionPsaMqtt()
{
   g_mqtt->secAction();
}

extern char logInfoFileName[];
void cmd_psamqtt(int argc,char* argv[])
{
   if(argc == 2 && strcmp(argv[1], "printon") == 0)
   {
      g_trace = true;
      AmeFlush();
      return;
   }
   if(argc == 2 && strcmp(argv[1], "printoff") == 0)
   {
      g_trace = false;
      AmeFlush();
      return;
   }
   if(argc == 2 && strcmp(argv[1], "reload") == 0)
   {
      mqttInit();
      AmeFlush();
      return;
   }
   if(argc == 2 && strcmp(argv[1], "list") == 0)
   {
      if(g_mqtt == NULL)
      {
         AmeINFO("mqtt not initialized\n");
         AmeFlush();
         return;
      }
      CStr str;
      g_mqtt->list(str);
      AmeINFO("%s", str.c_str());
      AmeFlush();
      return;
   }
   if(argc == 3 && strcmp(argv[1], "close") == 0)
   {
      g_mqtt->closeLink(atoi(argv[2]));
      AmeFlush();
      return;
   }
   if(argc == 2 && strcmp(argv[1], "connect") == 0)
   {
      g_mqtt->reconnect();
      AmeFlush();
      return;
   }
   if(argc == 3 && strcmp(argv[1], "link") == 0)
   {
      CMqttLink* link = g_mqtt->getLink(atoi(argv[2]));
      if(link == NULL)
      {
         AmeINFO("invalid link id %d\n", atoi(argv[2]));
         AmeFlush();
         return;
      }
      CStr str;
      link->printAddrMap(str);
      AmeINFO("%s\n", str.c_str());
      AmeFlush();
      return;
   }
   AmeINFO("Usage: psamqtt list\n");
   AmeINFO("       psamqtt link <linkid>\n");
   AmeINFO("       psamqtt close [<linkid>]\n");
   AmeINFO("       psamqtt connect\n");
   AmeINFO("       psamqtt reload\n");
   AmeINFO("       psamqtt printon | printoff\n");
   AmeFlush();
}

TCommandItem psamqttCommandTable[]=
{
   { 0, "psamqtt",  cmd_psamqtt, "list the psa mqtt inforamtion\n"},
   { 0, "",        cmd_psamqtt, ""}
};

void initPsaMqtt(INT psaid)
{
   setHookBuildFdSet(psaid, &hookFuncBuildFdSetMqtt);
   setHookProcFdSet(psaid, &hookFuncProcFdSetMqtt);
   setHookCloseResource(psaid, &hookFuncProcCloseResource);
   setHookSendUniNetMsg(psaid, hookSendMsgPsaMqtt);
   setHookSecAction(psaid, hookSecActionPsaMqtt);

   g_thisPsaId = psaid;
   mqttInit();
   new TAbstractAMEHandler(psamqttCommandTable,psaid);
}
