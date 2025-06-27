//
// Created by zpf on 25-6-26.
//

#ifndef DATABASE_H
#define DATABASE_H
#include "../BiliEntity/User.h"
#include "SQLiteCpp/Database.h"

#include <string>
#include <vector>

struct GiftData
{
    int64_t     timeStamp;
    User        user;
    std::string giftName;
    int         giftPrice;
    int         giftCount;
};
struct BlindBoxData
{
    int64_t     timeStamp;
    User        user;
    std::string giftName;
    int         giftPrice;
    int         giftCount;
    std::string originalGiftName;
    int         originalGiftPrice;
};
struct DanmakuData
{
    int64_t     timeStamp;
    User        user;
    std::string content;
};
class DataBase
{
public:
    static DataBase* GetInstance();

public:
    bool               Init();
    [[nodiscard]] bool AddGift(const std::string& giftName, int giftPrice, int giftCount,
                               const User& user) const;
    [[nodiscard]] std::vector<GiftData> GetGifts(int64_t startTimeStamp,
                                                 int64_t endTimeStamp) const;

    [[nodiscard]] bool AddBlindBox(const std::string& giftName, int giftPrice, int giftCount,
                                   const std::string& originalGiftName, int originalGiftPrice,
                                   const User& user);
    [[nodiscard]] std::vector<BlindBoxData> GetBlindBoxes(int64_t startTimeStamp,
                                                          int64_t endTimeStamp) const;

    [[nodiscard]] bool                     AddDanmaku(const std::string& content, const User& user);
    [[nodiscard]] std::vector<DanmakuData> GetDanmakus(int64_t startTimeStamp,
                                                       int64_t endTimeStamp) const;
    [[nodiscard]] bool                     AddEnterRoomUser(const User& user);
    [[nodiscard]] std::vector<User>        GetEnterRoomUsers(int64_t startTimeStamp,
                                                             int64_t endTimeStamp) const;

private:
    DataBase();
    ~DataBase()                          = default;
    DataBase(const DataBase&)            = delete;
    DataBase& operator=(const DataBase&) = delete;

private:
    std::string      dbPath;
    SQLite::Database db;

private:
    static DataBase* pInstance;
};



#endif   // DATABASE_H
