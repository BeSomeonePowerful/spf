
#include "httpserver.h"
HttpServer::HttpServer(char *spec)
{
    _spec = strdup(spec);
}

HttpServer::~HttpServer()
{
    if (_spec) {
        free(_spec);
    }
}
void HttpServer::setServerAdapter(IServerAdapter *serverAdapter)
{
    _serverAdapter = serverAdapter;
}
void HttpServer::start()
{
    _streamer = new HttpPacketStreamer(&factory);
    //_serverAdapter = new HttpServerAdapter(&factory);
    
    IOComponent *ioc = _transport.listen(_spec, _streamer, _serverAdapter);
    if (ioc == NULL) {
        TBSYS_LOG(ERROR, "listen error.");
        return;
    }
    eventThread = new HttpEventThread(_transport);
    timeThread = new HttpTimeThread(_transport);
    
    _transport.start();
    _transport.wait();
    eventThread->run();
    timeThread->run();
}

void HttpServer::stop()
{
    _transport.stop();
}

