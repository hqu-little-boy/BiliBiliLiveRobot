//
// Created by zeng on 24-6-26.
//

#ifndef CONFIG_H
#define CONFIG_H
#include "../Net/Url.h"
#include "Logger.h"

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/context.hpp>
#include <shared_mutex>
#include <vector>

/// @brief 配置类，单例模式
class Config : public noncopyable
{
public:
    static Config* GetInstance();
    static void    DestroyInstance();

    static std::string GetConfigPath();
    static std::string GetCookiePath();
    bool               LoadFromJson(std::string_view jsonPath);
    bool               SaveToJson();
    bool               LoadUID();

    [[nodiscard]] uint64_t           GetRoomId() const;
    void                             SetRoomId(uint64_t roomId);
    [[nodiscard]] const Url&         GetDanmuSeverConfUrl() const;
    [[nodiscard]] LogLevel           GetLogLevel() const;
    [[nodiscard]] const std::string& GetLogPath() const;
    [[nodiscard]] uint8_t            GetDanmuLength() const;
    [[nodiscard]] bool               CanPKNotice() const;
    void                             SetCanPKNotice(bool canPK);
    [[nodiscard]] bool               CanComeAroundNotice() const;
    void                             SetCanComeAroundNotice(bool canComeAroundNotice);
    [[nodiscard]] bool               CanGuardNotice() const;
    void                             SetCanGuardNotice(bool canGuard);
    [[nodiscard]] bool               CanThanksGift() const;
    void                             SetCanThanksGift(bool canThanksGift);
    [[nodiscard]] bool               CanSuperChatNotice() const;
    void                             SetCanSuperChatNotice(bool canSuperChat);
    [[nodiscard]] uint8_t            GetThanksGiftTimeout() const;
    void                             SetThanksGiftTimeout(uint8_t timeout);
    // [[nodiscard]] const std::string&        GetGiftThanksWord() const;
    // void                                    SetGiftThanksWord(const std::string& word);
    [[nodiscard]] bool                      CanThanksBlindBox() const;
    void                                    SetCanThanksBlindBox(bool canThanksBlindBox);
    [[nodiscard]] bool                      CanDrawByLot() const;
    void                                    SetCanDrawByLot(bool canDrawByLot);
    [[nodiscard]] const std::string&        GetDrawByLotWord() const;
    [[nodiscard]] std::vector<std::string>& GetDrawByLotWordList();
    [[nodiscard]] bool                      CanThanksFocus() const;
    void                                    SetCanThanksFocus(bool canThanksFocus);
    [[nodiscard]] const std::string&        GetThanksFocusWord() const;
    void                                    SetThanksFocusWord(std::string_view thanksFocusWord);
    [[nodiscard]] bool                      CanThanksShare() const;
    void                                    SetCanThanksShare(bool canThanksShare);
    [[nodiscard]] const std::string&        GetThanksShareWord() const;
    void                                    SetThanksShareWord(std::string_view thanksShareWord);
    [[nodiscard]] bool                      CanEntryNotice() const;
    void                                    SetCanEntryNotice(bool canEntryNotice);
    [[nodiscard]] const std::string&        GetNormalEntryNoticeWord() const;
    [[nodiscard]] const std::string&        GetGuardEntryNoticeWord() const;
    [[nodiscard]] std::vector<std::string>& GetNormalEntryNoticeList();
    [[nodiscard]] std::vector<std::string>& GetGuardEntryNoticeList();
    [[nodiscard]] std::string               ToString() const;

public:
    [[nodiscard]] const std::string& GetWbiMixKey() const;
    [[nodiscard]] uint64_t           GetRobotUID() const;
    [[nodiscard]] const std::string& GetRobotUname() const;
    [[nodiscard]] const std::string& GetRobotFace() const;
    [[nodiscard]] bool               IsLogined() const;

public:
    [[nodiscard]] std::shared_lock<std::shared_mutex> GetSharedLock();
    [[nodiscard]] std::unique_lock<std::shared_mutex> GetUniqueLock();
#if defined(TEST)
    constexpr inline bool IsTest()
    {
        return true;
    }
#endif

private:
    Config();
    ~Config()                        = default;
    Config(const Config&)            = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&)                 = delete;
    Config& operator=(Config&&)      = delete;

private:
    static Config* pInstance;

    uint64_t    roomId;                // 房间号
    Url         danmuSeverConfUrl;     // 弹幕服务器配置URL
    LogLevel    logLevel;              // 日志等级
    std::string logPath;               // 日志路径
    uint8_t     danmuLength;           // 弹幕长度
    bool        canPKNotice;           // 是否开启PK通知
    bool        canComeAroundNotice;   // 串门通知
    bool        canGuardNotice;        // 是否开启舰长感谢
    bool        canThanksGift;         // 是否开启礼物感谢
    // std::string              giftThanksWord;          // 礼物感谢词
    bool                     canThanksBlindBox;       // 是否开启盲盒感谢
    bool                     canSuperChatNotice;      // 是否开启醒目留言感谢
    uint8_t                  thanksGiftTimeout;       // 感谢礼物超时时间
    bool                     canDrawByLot;            // 是否开启抽奖
    std::vector<std::string> drawByLotList;           // 抽奖列表
    bool                     canEntryNotice;          // 欢迎弹幕
    std::vector<std::string> normalEntryNoticeList;   // 欢迎普通弹幕列表内容
    std::vector<std::string> guardEntryNoticeList;    // 欢迎普通弹幕列表内容
    bool                     canThanksFocus;          // 是否开启关注感谢
    std::string              thanksFocusNotice;       // 关注感谢弹幕内容
    bool                     canThanksShare;          // 是否开启分享感谢
    std::string              thanksShareNotice;       // 分享感谢弹幕内容

    std::string wbiMixKey;   // wbi混淆密钥
    Url         uidUrl;      // 弹幕服务器URL
private:
    uint64_t    robotUID;     // 机器人UID
    std::string robotUname;   // 机器人昵称
    bool        isLogined;    // 是否已登录
    std::string robotFace;    // 机器人头像URL
private:
    boost::asio::io_context        ioc;
    boost::asio::ssl::context      ctx;
    boost::asio::ip::tcp::resolver resolver;   // DNS解析器
private:
    std::string configPath;   // 配置文件路径
private:
    mutable std::shared_mutex configMutex;
};

#endif   // CONFIG_H
