/* mqttlink.C, by Zhang Haibin, 2015-06-12
 */
#include "mqttmsg.h"
#include <string.h>

//simply to resolve unierror declaration problem
#include "CommonTask.h"

bool CMqttMsg::isPacket(const char* content, int packetId)
{
   return (unsigned char)(*content) >> 4 == packetId;
}

void CMqttMsg::appendLength(CStr& msg, int length)
{
   do
   {
      char c = length % 128;
      length = length / 128;
      // if there are more data to encode, set the top bit of this byte
      if(length > 0)
          c = c | 128;
      msg += c;
   }
   while(length > 0);
}

void CMqttMsg::pack(CStr& msg, int packetType)
{
   msg += (char)(packetType << 4);
   msg += (char)0;
}

void CMqttMsg::pack(CStr& msg, int packetType, CStr& body, int flags)
{
   char headByte = (packetType << 4) + flags;
   msg += headByte;
   appendLength(msg, body.length());
   msg += body;
}

void CMqttMsg::appendStr(CStr& msg, const char* str)
{
   //modified by wangpan
   //cout << "CMqttMsg::appendStr:msg-size: " << msg.size() << "," << msg.c_str() << endl;
   int len = strlen(str);
   len=htonl(len);
   msg.nCat((char *)&len+2, 2);

   //cout << "CMqttMsg::appendStr:msg-size: " << msg.size() << "," << msg.c_str() << endl;
   msg << str;
   //cout << "msgLength: " << msg.GetVarCharLen() << endl;
}

void CMqttMsg::appendShort(CStr& msg, int value)
{
   //append sth into 2byte space
   value=htonl(value);
   msg.nCat((char *)&value+2, 2);
}

int CMqttMsg::parseLength(const char*& content, int& length)
{
   int multiplier = 1;
   int value = 0;
   int encodedByte;
   do
   {
      if(length < 0)
         return -2;
      encodedByte = (unsigned char)(*content);
      content++;
      length--;
      value += (encodedByte & 127) * multiplier;
      multiplier *= 128;
      if (multiplier > 128*128*128)
      {
         UniERROR("received message invalid length");
         return -1;
      }
   }
   while((encodedByte & 128) != 0);
   return value;
}

bool CMqttMsg::parseByte(int& result, const char*& p, int& length)
{
   if(length < 1)
   {
      UniERROR("expected 1 bytes for parse byte");
      return false;
   }
   result = (unsigned char)(*p);
   p++;
   length--;
   return true;
}

bool CMqttMsg::parseShort(int& result, const char*& p, int& length)
{
  // printf("parseShort:");
  // for( int i=0 ; i<length ; ++i )
  // {
  //   printf("%x,",p[i]);
  // }
  // printf("\n");

   if(length < 2)
   {
      UniERROR("expected 2 bytes for parse byte");
      return false;
   }

   //function of this func
   //does uniParseUInt2 change the pointer p

   //forceful type conversion
   //there may have some order problem:big-endien or little-endien
   //we assumed int to be 4-byte
   unsigned int tmp = 0;
   //strncpy((char *)&tmp+2,p,2);
   char *t = (char *)&tmp+2;
 	for( int i=0 ; i<2 ; ++i )
 	{
 		t[i]=p[i];
 	}
   tmp=ntohl(tmp);
   result = tmp;
  //  uniParseUInt2(p, length);
  p+=2;
  length-=2;
   return true;
}

bool CMqttMsg::parseString(CStr& result, const char*& p, int& length)
{
  //  printf("parseString:");
  //  for( int i=0 ; i<length ; ++i )
  //  {
  //    printf("%x,",p[i]);
  //  }
  //  printf("\n");

   if(length < 2)
   {
      UniERROR("expected 2 bytes for string length");
      return false;
   }

   //
   unsigned int tmp = 0;
   //strncpy((char *)&tmp+2,p,2);
   char *t = (char *)&tmp+2;
 	for( int i=0 ; i<2 ; ++i )
 	{
 		t[i]=p[i];
 	}
   tmp=ntohl(tmp);

  //  printf("Parsing: ");
  //  for( int i=0 ; i<2 ; ++i )
  //  {
  // 		printf("%x,",p[i]);
  //  }
   //something received from web:msg
   //two bytes of this msg must be parsed as a unsigned int
   //how to do that
   int stringLength = tmp;
   cout << "stringlength: " << stringLength << endl;
   //functionality of following method
   //try to rebuild a new one
  //  uniParseUInt2(p, length);
   if(length < stringLength)
   {
      UniERROR("invalid string length %d", stringLength);
      return false;
   }
   p+=2;
   length-=2;
   result.set(p, stringLength);

   cout << "result: " << result.c_str() << endl;
   p += stringLength;
   length -= stringLength;
   return true;
}

void CMqttMsg::parseError(const char* msgName, const char* paraName)
{
   UniERROR("error deteced when parse %s message, parameter: %s", msgName, paraName);
}
