#ifndef MOB_UTILITY_H
#define MOB_UTILITY_H

class RidGenerator
{
public:
  RidGenerator(int _maxRid=65536);
  virtual int getNextRid();
private:
  int maxRid;
  int curRid;
};


#endif
