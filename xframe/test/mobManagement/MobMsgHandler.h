#ifndef MOB_MSG_HANDLER_H
#define MOB_MSG_HANDLER_H

class MobMsgHandler
{
public:
  virtual TUniNetMsg *procMsg(TUniNetMsg *msg)=0;
};

#endif
