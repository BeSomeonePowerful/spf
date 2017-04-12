#include "timer.h"

TTimer::TTimer(UINT timerMark, UINT timerKey, UINT timerDelay, UINT timerTID, UINT timerIID, void* timerPara) :
   mWhen(timerDelay + getTimeMs()),
   mTimerKey(timerKey),
   mTimerMark(timerMark),
   mTimerDelay(timerDelay),
   mTimerTID(timerTID),
   mTimerIID(timerIID),
   mPara(timerPara)   
{}

std::ostream& 
TTimer::encode(std::ostream& str) const
{
   UINT64 now(getTimeMs());
   str << "TTimer[ when=" << mWhen << " rel=";
   if (mWhen < now)
   {
      str << "past";
   }
   else
   {
      str << (mWhen - now);
   }
   str << "]";
   return str;
}
