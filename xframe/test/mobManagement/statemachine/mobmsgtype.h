#ifndef MOB_MSG_TYPE_H
#define MOB_MSG_TYPE_H

#include "msgdef_rsc.h"
#include "msgdatadef.h"
#include "msgdef_com.h"
#include <iostream>

using std::cout;
using std::endl;

const int RSC_MOB_CON = 0x10;
const int RSC_MOB_CONACK = 0x20;
const int RSC_MOB_REG = 0x30;
const int RSC_MOB_REGACK = 0x40;
const int RSC_MOB_GET = 0x50;
const int RSC_MOB_GETACK = 0x60;

const int RSC_MOB_SUB = 0x100;
const int RSC_MOB_SUBACK = 0x200;
const int RSC_MOB_NTY = 0x300;
const int RSC_MOB_NTYACK = 0x400;

class MobMsgHelper
{
public:
  static TUniNetMsg *buildRscMsg();
};

#endif
