#ifndef _HTTPCLIENT_H
#define _HTTPCLIENT_H
#include "tbnet.h"
#include "httpthread.h"

using namespace tbnet;

/*
 *   */
class HttpClient {
public:
    HttpClient(IPacketHandler *packetHandler);
    ~HttpClient();
    void start();
    void stop();
    /**
     * 得到一连接
     */
    Connection *getConnection(uint64_t serverId);
    /**
     * 发送数据包
     */
    bool sendPacket(uint64_t serverId, Packet *packet, void *args = NULL, bool noblocking = true);
    bool sendPacket(const char* ip,int port, Packet *packet, void *args = NULL, bool noblocking = true);
private:
    HttpEventThread *eventThread;
    HttpTimeThread *timeThread;
    Transport _transport;
    HttpPacketStreamer *_streamer;
    DefaultHttpPacketFactory factory;
    IPacketHandler *_packetHandler;
    //char *_spec;
    int _queueLimit;
    int _queueTimeout;
    TBNET_CONN_MAP _connectMap;
    tbsys::CThreadMutex _mutex;
};
#endif