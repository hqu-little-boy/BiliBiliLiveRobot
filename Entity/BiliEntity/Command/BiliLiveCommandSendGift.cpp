//
// Created by zeng on 24-7-7.
//

#include "BiliLiveCommandSendGift.h"

BiliLiveCommandSendGift::BiliLiveCommandSendGift(const nlohmann::json& message)
    : BiliLiveCommandBase(message)
    , user(0, "", 0)
    , giftCount(0)
    , giftPrice(0)
    , giftName("")
    , originalGiftPrice(0)
    , originalGiftName("")
{
    LoadMessage(message);
}

std::string BiliLiveCommandSendGift::ToString() const
{
    return std::format("User: {} 赠送, GiftCount: {}, GiftPrice: {}, GiftName: {}, "
                       "OriginalGiftPrice: {}, OriginalGiftName: {}",
                       user.GetUname(),
                       giftCount,
                       giftPrice,
                       giftName,
                       originalGiftPrice,
                       originalGiftName);
}

bool BiliLiveCommandSendGift::LoadMessage(const nlohmann::json& message)
{
    try
    {
        const auto& data{message["data"]};
        auto        uname{data["uname"].get<std::string>()};
        auto        uid{data["uid"].get<uint64_t>()};
        auto        guardLevel{data["guard_level"].get<unsigned>()};
        this->user      = User(uid, uname, guardLevel);
        this->giftCount = message["data"]["num"];
        this->giftPrice = message["data"]["price"];
        this->giftName  = message["data"]["giftName"];
        if (message["data"]["blind_gift"].is_object())
        {
            this->originalGiftPrice = message["data"]["blind_gift"]["original_gift_price"];
            this->originalGiftName  = message["data"]["blind_gift"]["original_gift_name"];
        }
    }
    catch (const nlohmann::json::exception& e)
    {
        LOG_MESSAGE(LogLevel::Error, e.what());
        return false;
    }
    return true;
}

void BiliLiveCommandSendGift::Run() const
{
    LOG_MESSAGE(LogLevel::Info, this->ToString());
}

// BiliApiUtil::LiveCommand BiliLiveCommandSendGift::GetCommandType() const
// {
//     return commandType;
// }