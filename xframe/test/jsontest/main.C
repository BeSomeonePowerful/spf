#include "json.h"
#include <iostream>

using std::cout;
using std::endl;


int main()
{
  const char *data = "\
    \"termid:\":13,
    \"uaid:\":17
   ";
  JSONValue *value = JSON::Parse(data);
  if( value!=NULL )
  {
    JSONObject root;
    if( value->IsObject()==false )
    {
      cout << "error: no root node" << endl;
    }
    root = value->AsObject();
    if( root.find("termid")!=root.end() && root["termid"]->IsNumber()==true )
    {
      cout << "termid: " << root["termid"]->AsNumber() << endl;
    }
    if( root.find("uaid")!=root.end() && root["uaid"]->IsNumber()==true )
    {
      cout << "uaid: " << root["uaid"]->AsNumber() << endl;
    }
  }
  else
  {
    cout << "error while parsing json" << endl;
  }
  delete value;

  //stringify something
  JSONObject root1;
  root1["uaid"] = new JSONValue((double)10);
  root1["uaip"] = new JSONValue("127.0.0.1");
  JSONValue *value1 = new JSONValue(root);
  cout << "after stringify: " << value1->Stringify().c_str() << endl;
  delete value1;
  return 0;
}
