#include "mobutility.h"
#include <stdlib.h>
#include <time.h>

RidGenerator::RidGenerator(int _maxRid)
{
  maxRid = _maxRid;
  srand((unsigned int)time(0));

  //the range of rand function
  curRid = rand();
  if( curRid>=maxRid )
    curRid = curRid % maxRid;
}

int RidGenerator::getNextRid()
{
  int tmp = curRid;
  curRid++;
  if( curRid>=maxRid )
    curRid = curRid % maxRid;
  return tmp;
}
