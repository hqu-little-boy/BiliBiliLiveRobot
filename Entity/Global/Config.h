//
// Created by zeng on 24-6-26.
//

#ifndef CONFIG_H
#define CONFIG_H
#include "../Net/Url.h"
#include "Logger.h"

#include <vector>

/// @brief 配置类，单例模式
class Config : public noncopyable
{
public:
    static Config* GetInstance();
    static void    DestroyInstance();

    bool                             LoadFromJson(const std::string& jsonPath);
    [[nodiscard]] uint64_t           GetRoomId() const;
    [[nodiscard]] const Url&         GetDanmuSeverConfUrl() const;
    [[nodiscard]] LogLevel           GetLogLevel() const;
    [[nodiscard]] const std::string& GetLogPath() const;
    [[nodiscard]] uint8_t            GetDanmuLength() const;
    [[nodiscard]] bool               CanPKNotice() const;
    [[nodiscard]] bool               CanGuardNotice() const;
    [[nodiscard]] bool               CanThanksGift() const;
    [[nodiscard]] bool               CanSuperChatNotice() const;
    [[nodiscard]] uint8_t            GetThanksGiftTimeout() const;
    [[nodiscard]] bool               CanDrawByLot() const;
    [[nodiscard]] const std::string& GetDrawByLotWord() const;
    [[nodiscard]] bool               CanThanksFocus() const;
    void                             SetCanThanksFocus(bool canThanksFocus);
    [[nodiscard]] bool               CanThanksShare() const;
    void                             SetCanThanksShare(bool canThanksShare);
    [[nodiscard]] bool               CanEntryNotice() const;
    void                             SetCanEntryNotice(bool canEntryNotice);
    [[nodiscard]] const std::string& GetNormalEntryNoticeWord() const;
    [[nodiscard]] const std::string& GetGuardEntryNoticeWord() const;
    [[nodiscard]] std::string        ToString() const;

private:
    Config();
    ~Config()                        = default;
    Config(const Config&)            = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&)                 = delete;
    Config& operator=(Config&&)      = delete;

private:
    static Config* pInstance;

    uint64_t                 roomId;                  // 房间号
    Url                      danmuSeverConfUrl;       // 弹幕服务器配置URL
    LogLevel                 logLevel;                // 日志等级
    std::string              logPath;                 // 日志路径
    uint8_t                  danmuLength;             // 弹幕长度
    bool                     canPKNotice;             // 是否开启PK通知
    bool                     canGuardNotice;          // 是否开启舰长感谢
    bool                     canThanksGift;           // 是否开启礼物感谢
    bool                     canSuperChatNotice;      // 是否开启醒目留言感谢
    uint8_t                  thanksGiftTimeout;       // 感谢礼物超时时间
    bool                     canDrawByLot;            // 是否开启抽奖
    std::vector<std::string> drawByLotList;           // 抽奖列表
    bool                     canEntryNotice;          // 欢迎弹幕
    std::vector<std::string> normalEntryNoticeList;   // 欢迎普通弹幕列表内容
    std::vector<std::string> guardEntryNoticeList;    // 欢迎普通弹幕列表内容
    bool                     canThanksFocus;          // 是否开启关注感谢
    bool                     canThanksShare;          // 是否开启分享感谢
};

#endif   // CONFIG_H
