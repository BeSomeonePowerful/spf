/* mqttmsg.h, byZhang Haibin, 20150612
 */

#ifndef _MQTTMSG_H_
#define _MQTTMSG_H_

#include "comtypedef_vchar.h"

class CMqttMsg
{
public:
#define MQTT_P_CONNECT      1  // Client to Server Client request to connect to Server
#define MQTT_P_CONNACK      2  // Server to Client Connect acknowledgment
#define MQTT_P_PUBLISH      3  // Client to Server or Server to Client Publish message
#define MQTT_P_PUBACK       4  // Client to Server or Server to Client Publish acknowledgment
#define MQTT_P_PUBREC       5  // Client to Server or Server to Client Publish received (assured delivery part 1)
#define MQTT_P_PUBREL       6  // Client to Server or Server to Client Publish release (assured delivery part 2)
#define MQTT_P_PUBCOMP      7  // Client to Server or Server to Client Publish complete (assured delivery part 3)
#define MQTT_P_SUBSCRIBE    8  // Client to Server Client subscribe request
#define MQTT_P_SUBACK       9  // Server to Client Subscribe acknowledgment
#define MQTT_P_UNSUBSCRIBE 10  // Client to Server Unsubscribe request
#define MQTT_P_UNSUBACK    11  // Server to Client Unsubscribe acknowledgment
#define MQTT_P_PINGREQ     12  // Client to Server PING request
#define MQTT_P_PINGRESP    13  // Server to Client PING response
#define MQTT_P_DISCONNECT  14  // Client to Server Client is disconnecting
   static bool isPacket(const char* content, int packetId);

   static void pack(CStr& msg, int packetType, CStr& body, int flags = 0);
   static void pack(CStr& msg, int packetType);

   static void appendStr(CStr& msg, const char* str);
   static void appendLength(CStr& msg, int length);
   static void appendShort(CStr& msg, int value);

      // return -1, error detected
   // return -2, msg not complete
   static int parseLength(const char*& content, int& length);

   static bool parseByte(int& result, const char*& p, int& length);
   static bool parseShort(int& result, const char*& p, int& length);
   static bool parseString(CStr& result, const char*& p, int& length);
   static void parseError(const char* msgName, const char* paraName);
};

#endif
