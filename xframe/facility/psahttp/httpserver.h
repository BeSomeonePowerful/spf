/* httpserver.h, by ztwu, 2014-7-16
*/

#ifndef _HTTPSERVER_H
#define _HTTPSERVER_H
#include "tbnet.h"
#include "httpthread.h"

using namespace tbnet;

/**
 *   */
class HttpServerAdapter : public IServerAdapter
{
public:
    HttpServerAdapter(IPacketFactory *factory) {
        _factory = factory;
    }
    IPacketHandler::HPRetCode handlePacket(Connection *connection, Packet *packet)
    {
        HttpRequestPacket *request = (HttpRequestPacket*) packet;
        HttpResponsePacket *reply = (HttpResponsePacket*)_factory->createPacket(0);
        reply->setStatus(true);
        reply->setKeepAlive(request->isKeepAlive());
       // if (!request->isKeepAlive()) {
       //     connection->setWriteFinishClose(true);
       // }
        //char *query = request->getQuery();
        char *query = "text";
        if (query) {
            reply->setBody(query, strlen(query));
        }
        request->free();
        connection->postPacket(reply);
        return IPacketHandler::FREE_CHANNEL;
    }    
private:
    IPacketFactory *_factory;
};

/*
 *   */
class HttpServer {
public:
    HttpServer(char *spec);
    ~HttpServer();
    void start();
    void stop();
    void setServerAdapter(IServerAdapter *iServerAdapter);
private:
    HttpEventThread *eventThread;
    HttpTimeThread *timeThread;
    Transport _transport;
    HttpPacketStreamer *_streamer;
    DefaultHttpPacketFactory factory;
    IServerAdapter *_serverAdapter;
    char *_spec;
};
#endif


