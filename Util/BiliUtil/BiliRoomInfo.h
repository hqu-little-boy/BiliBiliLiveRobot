//
// Created by zpf on 25-3-12.
//

#ifndef ROOMINFO_H
#define ROOMINFO_H
#include "../../Entity/Net/Url.h"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <cstdint>



class BiliRoomInfo
{
public:
    BiliRoomInfo(uint64_t roomId);
    ~BiliRoomInfo() = default;

    bool Init();
    bool GetRoomInfo();
    // [[nodiscard]] bool     IsSuccess() const;
    [[nodiscard]] uint64_t GetRoomId() const;
    [[nodiscard]] uint64_t GetAttentionCount() const;
    [[nodiscard]] uint64_t GetOnlineCount() const;
    [[nodiscard]] bool     IsLiving() const;

private:
    Url url;
    // bool     isSuccess;
    uint64_t roomId;
    uint64_t attentionCount;
    uint64_t onlineCount;
    bool     isLiving;

private:
    boost::asio::io_context                            ioc;
    boost::asio::ssl::context                          ctx;
    boost::asio::ip::tcp::resolver                     resolver;   // DNS解析器
    // boost::asio::ssl::stream<boost::beast::tcp_stream> stream;
};



#endif   // ROOMINFO_H
