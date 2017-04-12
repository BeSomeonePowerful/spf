#ifndef _MQTTMSG_ARG_H
#define _MQTTMSG_ARG_H

#include <memory>

//using std::shared_ptr;
using namespace std;

// #define ALIVE_TIMER 2
// #define ACK_TIMER 3

struct TMsgArg
{

};


//we need to try to understand pass-by-name
//and pass-by-reference
struct TConArg:public TMsgArg
{
  CStr name;
  CStr user;
  CStr passwd;
  //should argue whether use pass by ref
  //TConArg(CStr _name,CStr _user,CStr _passwd):name(_name),user(_user),
  //passwd(_passwd) {}
  int keepAlive;
};

struct TPubArg:public TMsgArg
{
  CStr topic;
  CStr content;
  //TPubArg(CStr _topic,CStr _content):topic(_topic),content(_content) {}
};

//init of two methods
struct TSubArg:public TMsgArg
{
  CList<CStr> topics;
  int packetId;
  //TSubArg(CStr _topic):topic(_topic) {}
};

struct TUnsubArg:public TMsgArg
{
  CList<CStr> topics;
  int packetId;
  //TUnsubArg(CStr _topic):topic(_topic) {}
};

struct TUnsubAckArg:public TMsgArg
{
  int packetId;
  int code;
  //TUnsubAckArg(int _pid,int _code):packetId(_pid),code(_code) {}
};

struct TSubAckArg:public TMsgArg
{
  int packetId;
  int code;
  //TSubAckArg(int _pid,int _code):packetId(_pid),code(_code) {}
};

struct TConAckArg:public TMsgArg
{
  int code;
};

//how to manage the memory here
//we may have to manage memory by our own
//auto_ptr or shared_ptr doesn't fit our needs

//two type of error may occur
//1. memory leak
//2. wrong memory deallocation
struct TestMessage
{
  int msgType;
  std::shared_ptr<TMsgArg> arg;
};

#endif
