//
// Created by zeng on 24-6-25.
//

#ifndef BILIAPIUTIL_H
#define BILIAPIUTIL_H
#include "../../Base/noncopyable.h"

#include <boost/asio/buffer.hpp>
#include <boost/endian/buffers.hpp>
#include <brotli/decode.h>
#include <list>
#include <span>
#include <unordered_map>

class BiliApiUtil : public noncopyable
{
public:
    enum class Operation
    {
        HANDSHAKE         = 0,   // 握手
        HANDSHAKE_REPLY   = 1,
        HEARTBEAT         = 2,   // 心跳
        HEARTBEAT_REPLY   = 3,   // 心跳回复
        SEND_MSG          = 4,
        SEND_MSG_REPLY    = 5,   // 普通包（命令）
        DISCONNECT_REPLY  = 6,
        AUTH              = 7,   // 认证包
        AUTH_REPLY        = 8,   // 认证回复
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

    enum class ZipOperation
    {
        NormalNone    = 0,
        HeartBeatNone = 1,
        NormalZlib    = 2,
        NormalBrotli  = 3,
    };

    enum class LiveCommand
    {
        NONE,            // 无
        OTHER,           // 其他
        DANMU_MSG,       // 弹幕消息
        INTERACT_WORD,   // 进入直播间或关注消息
        GUARD_BUY,       // 用户购买舰长 / 提督 / 总督
        // USER_TOAST_MSG,   // 用户购买舰长 / 提督 / 总督后的庆祝消息，内容包含用户陪伴天数
        SUPER_CHAT_MESSAGE,   // 用户发送醒目留言
        SEND_GIFT,            // 投喂礼物等
        // COMBO_SEND,                          // 礼物连击
        ENTRY_EFFECT,   // 用户进入直播间
        LIVE,           // 开播
        PREPARING,      // 下播
        // POPULARITY_RED_POCKET_NEW,           // 红包
        POPULARITY_RED_POCKET_START,         // 红包开始
        POPULARITY_RED_POCKET_WINNER_LIST,   // 红包中奖名单
        ANCHOR_LOT_START,                    // 开始抽奖
        ANCHOR_LOT_END,                      // 结束抽奖
        PK_BATTLE_START,                     // PK开始
        PK_BATTLE_START_NEW,                 // PK开始
        COMMON_NOTICE_DANMAKU,               // 大航海盲盒
    };
    static const std::unordered_map<std::string, LiveCommand> liveCommandMap;
    // 自定义Header结构体，用于存储包头信息
    struct HeaderTuple
    {
        uint32_t    totalSize;       // 封包总大小
        uint16_t    headerSize;      // 头部大小
        uint16_t    version;         // 协议版本
        uint32_t    operationCode;   // 操作码
        uint32_t    sequenceId;      // sequence
        std::string ToString() const;
    };

    /// @brief 构建包
    static bool MakePack(const std::string_view& body, Operation eOperation,
                         std::vector<uint8_t>& res);

    /// @brief 解包，可能有多个包一起发，需要分包
    static std::list<std::tuple<LiveCommand, std::string>> Unpack(
        const std::span<const uint8_t>& buffer);

    static LiveCommand GetLiveCommand(const std::string_view& cmd);
    static std::string GetLiveCommandStr(const std::string_view& cmd);

private:
    /// @brief 解包头部
    static HeaderTuple UnpackHeader(const std::span<const uint8_t>& buffer, unsigned front);
    // /// @brief 解析普通无压缩包体
    static std::tuple<BiliApiUtil::LiveCommand, std::string> UnpackBodyNoneCompress(
        const std::span<const uint8_t>& buffer, unsigned front, unsigned end);
    // /// @brief 解析Zlib
    static std::list<std::tuple<LiveCommand, std::string>> UnpackBodyZlib(
        const std::span<const uint8_t>& buffer, unsigned front, unsigned end);
    // /// @brief 解析Brotli
    static std::list<std::tuple<LiveCommand, std::string>> UnpackBodyBrotli(
        const std::span<const uint8_t>& buffer, unsigned front, unsigned end);
    // static BrotliDecoderState* brotliState;
};



#endif   // BILIAPIUTIL_H
