
#include "httpclient.h"
HttpClient::HttpClient(IPacketHandler *packetHandler)
{
    _packetHandler=packetHandler;
    _queueLimit = 256;
    _queueTimeout = 5000;
}

HttpClient::~HttpClient()
{
    
}
void HttpClient::start()
{
    _streamer = new HttpPacketStreamer(&factory);
    eventThread = new HttpEventThread(_transport);
    timeThread = new HttpTimeThread(_transport);
    
    _transport.start();
    _transport.wait();
    eventThread->run();
    timeThread->run();
}

void HttpClient::stop()
{
    _transport.stop();
}

/**
 * 连接服务器
 */
Connection *HttpClient::getConnection(uint64_t serverId) {
    if (serverId == 0) {
        return NULL;
    }

    tbsys::CThreadGuard guard(&_mutex);

    TBNET_CONN_MAP::iterator it ;
    it = _connectMap.find(serverId);
    if (it != _connectMap.end()) {
        return it->second;
    }

    // 从transport中获取
    char spec[64];
    sprintf(spec, "tcp:%s", tbsys::CNetUtil::addrToString(serverId).c_str());
    Connection *conn = _transport.connect(spec, _streamer, true);
    if (!conn) {
        TBSYS_LOG(WARN, "连接到服务器失败: %s", spec);
        return NULL;
    } else {
        conn->setDefaultPacketHandler(_packetHandler);
        conn->setQueueLimit(_queueLimit);
        conn->setQueueTimeout(_queueTimeout);
        _connectMap[serverId] = conn;
    }
    return conn;
}

/**
 * 发送数据包
 */
bool HttpClient::sendPacket(uint64_t serverId, Packet *packet, void *args, bool noblocking) {
    Connection *conn = getConnection(serverId);
    if (conn) {
        return conn->postPacket(packet, _packetHandler, args, noblocking);
    }
    return false;
}
bool HttpClient::sendPacket(const char* ip,int port, Packet *packet, void *args, bool noblocking)
{
    return sendPacket(tbsys::CNetUtil::strToAddr(ip,port),packet,args,noblocking);
}