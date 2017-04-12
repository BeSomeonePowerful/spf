#ifndef _HTTPTHREAD_H
#define _HTTPTHREAD_H
#include "threadif.h"
#include "tbnet.h"
using namespace tbnet;
class HttpEventThread : public ThreadIf
{
    public:
        HttpEventThread(Transport& server);
        virtual ~HttpEventThread();
        virtual void process();
     //   virtual void buildFdSet(CFdSet & fdset);
     //   virtual unsigned int getTimeTillNextProcessMS() const;
      //  virtual void beforeProcess() {};
        virtual void onstart();
       // virtual void afterProcess() {};
    private:
        Transport &mServer;

};
class HttpTimeThread : public ThreadIf
{
    public:
        HttpTimeThread(Transport& server);
        virtual ~HttpTimeThread();
        virtual void process();
     //   virtual void buildFdSet(CFdSet & fdset);
     //   virtual unsigned int getTimeTillNextProcessMS() const;
      //  virtual void beforeProcess() {};
        virtual void onstart();
       // virtual void afterProcess() {};
    private:
        Transport &mServer;

};

#endif
