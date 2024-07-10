//
// Created by zeng on 24-7-10.
//

#ifndef BILILIVECOMMANDPKSTART_H
#define BILILIVECOMMANDPKSTART_H
#include "BiliLiveCommandBase.h"

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/context.hpp>

class BiliLiveCommandPKStart : public BiliLiveCommandBase
{
public:
    explicit BiliLiveCommandPKStart(const nlohmann::json& message);
    ~BiliLiveCommandPKStart() = default;

    [[nodiscard]] virtual std::string ToString() const override;
    virtual bool                      LoadMessage(const nlohmann::json& message) override;
    virtual void                      Run() const override;

private:
    bool     GetRoomInit();
    bool     GetUserInfo();
    bool     GetTopListInfo();
    bool     GetRankListInfo();
    uint64_t oppositeRoomID;
    User     oppositeAnchor;
    unsigned totalAudienceNum;
    unsigned totalRankcount;

    boost::asio::io_context        ioc;        // IO上下文
    boost::asio::ssl::context      ctx;        // SSL上下文
    boost::asio::ip::tcp::resolver resolver;   // DNS解析器
};



#endif   // BILILIVECOMMANDPKSTART_H
