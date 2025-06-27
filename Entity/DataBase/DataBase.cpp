//
// Created by zpf on 25-6-26.
//

#include "DataBase.h"

#include "../Global/Logger.h"
DataBase* DataBase::pInstance = nullptr;
DataBase* DataBase::GetInstance()
{
    if (pInstance == nullptr)
    {
        pInstance = new DataBase();
    }
    return pInstance;
}
bool DataBase::Init()
{
    try
    {
        db.exec("CREATE TABLE IF NOT EXISTS gifts (timeStamp INTEGER PRIMARY KEY, "
                "giftName TEXT, giftPrice INTEGER, giftCount INTEGER,"
                "sendUserName TEXT, sendUserUID INTEGER);");
        db.exec("CREATE TABLE IF NOT EXISTS blindBox (timeStamp INTEGER PRIMARY KEY, "
                "giftName TEXT, giftPrice INTEGER,  giftCount INTEGER,"
                "originalGiftName TEXT, originalGiftPrice INTEGER, "
                "sendUserName TEXT, sendUserUID INTEGER);");
        db.exec("CREATE TABLE IF NOT EXISTS danmaku (timeStamp INTEGER PRIMARY KEY, "
                "content TEXT, "
                "sendUserName TEXT, sendUserUID INTEGER);");
        db.exec("CREATE TABLE IF NOT EXISTS enterRoomUser (timeStamp INTEGER PRIMARY KEY, "
                "enterUserName TEXT, enterUserUID INTEGER);");

        return true;
    }
    catch (const SQLite::Exception& e)
    {
        LOG_MESSAGE(LogLevel::Error, fmt::format("Database initialization failed: {}", e.what()));
        return false;
    }
    return true;
}
bool DataBase::AddGift(const std::string& giftName, int giftPrice, int giftCount,
                       const User& user) const
{
    try
    {
        SQLite::Statement query(db,
                                "INSERT INTO gifts (timeStamp, giftName, giftPrice, giftCount, "
                                "sendUserName, sendUserUID) "
                                "VALUES (?, ?, ?, ?, ?, ?);");
        // 获取当前时间点
        auto now = std::chrono::system_clock::now();
        // 计算当前时间点距离 epoch 的毫秒数
        auto milliseconds =
            std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        query.bind(1, milliseconds);
        query.bind(2, giftName);
        query.bind(3, giftPrice);
        query.bind(4, giftCount);
        query.bind(5, user.GetUname());
        query.bind(6, static_cast<int64_t>(user.GetUid()));
        auto sqliteResult = query.exec();
        if (sqliteResult != 1)
        {
            LOG_MESSAGE(LogLevel::Error,
                        fmt::format("Failed to add gift: SQLite returned {}", sqliteResult));
            return false;
        }
        LOG_MESSAGE(LogLevel::Debug,
                    fmt::format("Added gift: {} x {}, sent by {} (UID: {})",
                                giftName,
                                giftCount,
                                user.GetUname(),
                                user.GetUid()));
        return true;
    }
    catch (const SQLite::Exception& e)
    {
        LOG_MESSAGE(LogLevel::Error, fmt::format("Failed to add gift: {}", e.what()));
        return false;
    }
}
std::vector<GiftData> DataBase::GetGifts(int64_t startTimeStamp, int64_t endTimeStamp) const
{
    std::vector<GiftData> gifts;
    try
    {
        SQLite::Statement query(
            db,
            "SELECT timeStamp, giftName, giftPrice, giftCount, sendUserName, sendUserUID "
            "FROM gifts WHERE timeStamp >= ? AND timeStamp <= ?;");
        query.bind(1, startTimeStamp);
        query.bind(2, endTimeStamp);
        while (query.executeStep())
        {
            GiftData gift;
            gift.timeStamp = query.getColumn(0).getInt64();
            gift.giftName  = query.getColumn(1).getText();
            gift.giftPrice = query.getColumn(2).getInt();
            gift.giftCount = query.getColumn(3).getInt();
            gift.user.SetUname(query.getColumn(4).getText());
            gift.user.SetUid(query.getColumn(5).getInt64());
            gifts.push_back(gift);
        }
    }
    catch (const SQLite::Exception& e)
    {
        LOG_MESSAGE(LogLevel::Error, fmt::format("Failed to get gifts: {}", e.what()));
        return {};
    }
    return gifts;
}
bool DataBase::AddBlindBox(const std::string& giftName, int giftPrice, int giftCount,
                           const std::string& originalGiftName, int originalGiftPrice,
                           const User& user)
{
    try
    {
        SQLite::Statement query(db,
                                "INSERT INTO blindBox (timeStamp, giftName, giftPrice, giftCount, "
                                "originalGiftName, originalGiftPrice, sendUserName, sendUserUID) "
                                "VALUES (?, ?, ?, ?, ?, ?, ?, ?);");
        // 获取当前时间点
        auto now = std::chrono::system_clock::now();
        // 计算当前时间点距离 epoch 的毫秒数
        auto milliseconds =
            std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        query.bind(1, milliseconds);
        query.bind(2, giftName);
        query.bind(3, giftPrice);
        query.bind(4, giftCount);
        query.bind(5, originalGiftName);
        query.bind(6, originalGiftPrice);
        query.bind(7, user.GetUname());
        query.bind(8, static_cast<int64_t>(user.GetUid()));
        auto sqliteResult = query.exec();
        if (sqliteResult != 1)
        {
            LOG_MESSAGE(LogLevel::Error,
                        fmt::format("Failed to add blind box: SQLite returned {}", sqliteResult));
            return false;
        }
        LOG_MESSAGE(LogLevel::Debug,
                    fmt::format("Added blind box: {} x {}, sent by {} (UID: {})",
                                giftName,
                                giftCount,
                                user.GetUname(),
                                user.GetUid()));
        return true;
    }
    catch (const SQLite::Exception& e)
    {
        LOG_MESSAGE(LogLevel::Error, fmt::format("Failed to add blind box: {}", e.what()));
        return false;
    }
    return true;
}
std::vector<BlindBoxData> DataBase::GetBlindBoxes(int64_t startTimeStamp,
                                                  int64_t endTimeStamp) const
{
    std::vector<BlindBoxData> blindBoxes;
    try
    {
        SQLite::Statement query(
            db,
            "SELECT timeStamp, giftName, giftPrice, giftCount, originalGiftName, "
            "originalGiftPrice, "
            "sendUserName, sendUserUID FROM blindBox WHERE timeStamp >= ? AND timeStamp <= ?;");
        query.bind(1, startTimeStamp);
        query.bind(2, endTimeStamp);
        while (query.executeStep())
        {
            BlindBoxData box;
            box.timeStamp         = query.getColumn(0).getInt64();
            box.giftName          = query.getColumn(1).getText();
            box.giftPrice         = query.getColumn(2).getInt();
            box.giftCount         = query.getColumn(3).getInt();
            box.originalGiftName  = query.getColumn(4).getText();
            box.originalGiftPrice = query.getColumn(5).getInt();
            box.user.SetUname(query.getColumn(6).getText());
            box.user.SetUid(query.getColumn(7).getInt64());
            blindBoxes.push_back(box);
        }
    }
    catch (const SQLite::Exception& e)
    {
        LOG_MESSAGE(LogLevel::Error, fmt::format("Failed to get blind boxes: {}", e.what()));
        return {};
    }
    return blindBoxes;
}
bool DataBase::AddDanmaku(const std::string& content, const User& user)
{
    try
    {
        SQLite::Statement query(
            db,
            "INSERT INTO danmaku (timeStamp, content, sendUserName, sendUserUID) "
            "VALUES (?, ?, ?, ?);");
        // 获取当前时间点
        auto now = std::chrono::system_clock::now();
        // 计算当前时间点距离 epoch 的毫秒数
        auto milliseconds =
            std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        query.bind(1, milliseconds);
        query.bind(2, content);
        query.bind(3, user.GetUname());
        query.bind(4, static_cast<int64_t>(user.GetUid()));
        auto sqliteResult = query.exec();
        if (sqliteResult != 1)
        {
            LOG_MESSAGE(LogLevel::Error,
                        fmt::format("Failed to add danmaku: SQLite returned {}", sqliteResult));
            return false;
        }
        LOG_MESSAGE(LogLevel::Debug,
                    fmt::format("Added danmaku: {}, sent by {} (UID: {})",
                                content,
                                user.GetUname(),
                                user.GetUid()));
        return true;
    }
    catch (const SQLite::Exception& e)
    {
        LOG_MESSAGE(LogLevel::Error, fmt::format("Failed to add danmaku: {}", e.what()));
        return false;
    }
    return true;
}
std::vector<DanmakuData> DataBase::GetDanmakus(int64_t startTimeStamp, int64_t endTimeStamp) const
{
    std::vector<DanmakuData> danmakus;
    try
    {
        SQLite::Statement query(db,
                                "SELECT timeStamp, content, sendUserName, sendUserUID FROM danmaku "
                                "WHERE timeStamp >= ? AND timeStamp <= ?;");
        query.bind(1, startTimeStamp);
        query.bind(2, endTimeStamp);
        while (query.executeStep())
        {
            DanmakuData danmaku;
            danmaku.timeStamp = query.getColumn(0).getInt64();
            danmaku.content   = query.getColumn(1).getText();
            danmaku.user.SetUname(query.getColumn(2).getText());
            danmaku.user.SetUid(query.getColumn(3).getInt64());
            danmakus.push_back(danmaku);
        }
    }
    catch (const SQLite::Exception& e)
    {
        LOG_MESSAGE(LogLevel::Error, fmt::format("Failed to get danmakus: {}", e.what()));
        return {};
    }
    return danmakus;
}
bool DataBase::AddEnterRoomUser(const User& user)
{
    try
    {
        SQLite::Statement query(
            db,
            "INSERT INTO enterRoomUser (timeStamp, enterUserName, enterUserUID) "
            "VALUES (?, ?, ?);");
        // 获取当前时间点
        auto now = std::chrono::system_clock::now();
        // 计算当前时间点距离 epoch 的毫秒数
        auto milliseconds =
            std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        query.bind(1, milliseconds);
        query.bind(2, user.GetUname());
        query.bind(3, static_cast<int64_t>(user.GetUid()));
        auto sqliteResult = query.exec();
        if (sqliteResult != 1)
        {
            LOG_MESSAGE(
                LogLevel::Error,
                fmt::format("Failed to add enter room user: SQLite returned {}", sqliteResult));
            return false;
        }
        LOG_MESSAGE(
            LogLevel::Debug,
            fmt::format("Added enter room user: {}, UID: {}", user.GetUname(), user.GetUid()));
        return true;
    }
    catch (const SQLite::Exception& e)
    {
        LOG_MESSAGE(LogLevel::Error, fmt::format("Failed to add enter room user: {}", e.what()));
        return false;
    }
}
std::vector<User> DataBase::GetEnterRoomUsers(int64_t startTimeStamp, int64_t endTimeStamp) const
{
    std::vector<User> enterRoomUsers;
    try
    {
        SQLite::Statement query(db,
                                "SELECT timeStamp, enterUserName, enterUserUID FROM enterRoomUser "
                                "WHERE timeStamp >= ? AND timeStamp <= ?;");
        query.bind(1, startTimeStamp);
        query.bind(2, endTimeStamp);
        while (query.executeStep())
        {
            User user;
            user.SetUname(query.getColumn(1).getText());
            user.SetUid(query.getColumn(2).getInt64());
            enterRoomUsers.push_back(user);
        }
    }
    catch (const SQLite::Exception& e)
    {
        LOG_MESSAGE(LogLevel::Error, fmt::format("Failed to get enter room users: {}", e.what()));
        return {};
    }
    return enterRoomUsers;
}

DataBase::DataBase()
    : dbPath("database.sqlite")
    , db(dbPath, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE)
{
}
