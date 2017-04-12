#ifndef StatelessHandlerThread_Include_Guard
#define StatelessHandlerThread_Include_Guard

#include "rutil/ThreadIf.hxx"
#include "rutil/Logger.hxx"

#include "resip/stack/StatelessHandler.hxx"

#define RESIPROCATE_SUBSYSTEM Subsystem::SIP


namespace resip
{
class StatelessHandlerThread : public ThreadIf
{
   public:
      explicit StatelessHandlerThread(StatelessHandler& handler):
         mHandler(handler)
      {

      }
      virtual ~StatelessHandlerThread(){}


      virtual void thread()
      {
         while(!isShutdown())
         {
            try
            {
            	mHandler.process();
         	}
            catch(BaseException& e)
            {
               ErrLog(<< "Unhandled exception: " << e);
            }
         }
      }

   protected:
      StatelessHandler& mHandler;

}; // class TransactionControllerThread

} // namespace resip

#endif // include guard


