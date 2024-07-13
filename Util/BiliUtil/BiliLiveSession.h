//
// Created by zeng on 24-6-25.
//

#ifndef BILILIVESESSION_H
#define BILILIVESESSION_H
#include "../../Base/noncopyable.h"
#include "../../Entity/Net/Url.h"
#include "concurrencpp/concurrencpp.h"

#include <boost/asio/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/core/noncopyable.hpp>
#include <cstdlib>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <thread>

enum class Operation
{
    HANDSHAKE         = 0,
    HANDSHAKE_REPLY   = 1,
    HEARTBEAT         = 2,
    HEARTBEAT_REPLY   = 3,
    SEND_MSG          = 4,
    SEND_MSG_REPLY    = 5,
    DISCONNECT_REPLY  = 6,
    AUTH              = 7,
    AUTH_REPLY        = 8,
    RAW               = 9,
    PROTO_READY       = 10,
    PROTO_FINISH      = 11,
    CHANGE_ROOM       = 12,
    CHANGE_ROOM_REPLY = 13,
    REGISTER          = 14,
    REGISTER_REPLY    = 15,
    UNREGISTER        = 16,
    UNREGISTER_REPLY  = 17,
};

class BiliLiveSession : public std::enable_shared_from_this<BiliLiveSession>, public noncopyable
{
public:
    struct LiveHostUrl
    {
        LiveHostUrl() = default;
        LiveHostUrl(const std::string& host, unsigned port, unsigned ws_port, unsigned wss_port)
            : host(host)
            , port(port)
            , ws_port(ws_port)
            , wss_port(wss_port)
        {
        }
        std::string host;
        unsigned    port;
        unsigned    ws_port;
        unsigned    wss_port;
    };
    ///@brief 构造函数，设置解析器和WebSocket流
    explicit BiliLiveSession(boost::asio::io_context& ioc);
    bool InitSSLCert();
    bool InitRoomInfo();
    // Start the asynchronous operation
    void run();

    ///@brief 解析完成后的回调函数
    void on_resolve(boost::beast::error_code                            ec,
                    const boost::asio::ip::tcp::resolver::results_type& results);
    ///@brief 连接完成后的回调函数
    void on_connect(boost::beast::error_code                                    ec,
                    boost::asio::ip::tcp::resolver::results_type::endpoint_type ep);
    ///@brief SSL握手完成后的回调函数
    void on_ssl_handshake(boost::beast::error_code ec);
    ///@brief WebSocket握手完成后的回调函数，发送认证包
    void on_handshake(boost::beast::error_code ec);
    ///@brief 写操作完成后的回调函数
    void on_write(boost::beast::error_code ec, std::size_t bytes_transferred);
    ///@brief 认证操作完成后的回调函数
    void on_auth(boost::beast::error_code ec, std::size_t bytes_transferred);
    ///@brief 读操作完成后的回调函数
    void on_read(boost::beast::error_code ec, std::size_t bytes_transferred);
    ///@brief 关闭操作完成后的回调函数
    void on_close(boost::beast::error_code ec);
    ///@brief 发送心跳包
    void do_ping();

private:
    boost::asio::io_context&       ioc;        // IO上下文
    std::list<LiveHostUrl>         liveUrls;   // 直播地址列表
    boost::asio::ssl::context      ctx;        // SSL上下文
    boost::asio::ip::tcp::resolver resolver;   // DNS解析器
    boost::beast::websocket::stream<boost::asio::ssl::stream<boost::beast::tcp_stream>>
                              ws;       // WebSocket流
    boost::beast::flat_buffer buffer;   // 缓冲区
    std::string               text;     // 发送的文本消息
    std::string               token;    // 认证token
    std::string               host;     // 主机名
    std::string               target;   // 目标

    // std::jthread pingThread;   // 心跳线程
    concurrencpp::runtime runtime;
    concurrencpp::timer   pingtimer;   // 心跳定时器
};


#endif   // BILILIVESESSION_H