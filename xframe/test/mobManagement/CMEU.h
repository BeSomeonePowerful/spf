#ifndef CMEU_H
#define CMEU_H

#include "MobMsgHandler.h"
class CMEU:public MobMsgHandler
{
public:
  virtual TUniNetMsg *procMsg(TUniNetMsg *msg);

  //call uareg to send a con msg to backboneSat
  virtual void onInit();
private:
  
};

#endif
