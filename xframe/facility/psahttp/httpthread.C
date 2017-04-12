#include <iostream>
#include "httpthread.h"
using namespace std;
HttpEventThread::HttpEventThread(Transport &server):mServer(server)
{
}

HttpEventThread::~HttpEventThread()
{

}
void HttpEventThread::onstart()
{

}
void HttpEventThread::process()
{
		while(!isShutdown())
		{
                       mServer.eventProcess();
                       //tosleep(100);
		}
}
HttpTimeThread::HttpTimeThread(Transport &server):mServer(server)
{
}

HttpTimeThread::~HttpTimeThread()
{

}
void HttpTimeThread::onstart()
{

}
void HttpTimeThread::process()
{
		while(!isShutdown())
		{
                       mServer.timeProcess();
                       //usleep(500);
		}
}
