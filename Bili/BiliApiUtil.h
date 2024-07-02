//
// Created by zeng on 24-6-25.
//

#ifndef BILIAPIUTIL_H
#define BILIAPIUTIL_H
#include "../Base/noncopyable.h"

#include <boost/asio/buffer.hpp>
#include <boost/endian/buffers.hpp>
#include <list>

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
    static std::vector<uint8_t> MakePack(const std::string_view& body, Operation eOperation);

    /// @brief 解包，可能有多个包一起发，需要分包
    static std::list<std::string> Unpack(
        const std::vector<uint8_t>& buffer);

private:
    /// @brief 解包头部
    static HeaderTuple UnpackHeader(const std::vector<uint8_t>& buffer, unsigned front);
    /// @brief 解析包体
    static std::string UnpackBody(const std::vector<uint8_t>& buffer, unsigned front, unsigned end,
                                  ZipOperation zipOperation);
};



#endif   // BILIAPIUTIL_H
