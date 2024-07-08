//
// Created by zeng on 24-7-7.
//

#include "BiliLiveCommandSendGift.h"

BiliLiveCommandSendGift::BiliLiveCommandSendGift(const nlohmann::json& message)
    : BiliLiveCommandBase(message)
    , user(0, "", 0)
    , giftCount(0)
    , giftPrice(0)
{
    LoadMessage(message);
}

std::string BiliLiveCommandSendGift::ToString() const
{
    return std::format(
        "User: {} 赠送, GiftCount: {}, GiftPrice: {}, GiftName: {}, OriginalGiftName: {}",
        user.GetUname(),
        giftCount,
        giftPrice,
        giftName,
        OriginalGiftName);
}

bool BiliLiveCommandSendGift::LoadMessage(const nlohmann::json& message)
{
    try
    {
        const auto& data{message["data"]};
        auto uname{data["uname"].get<std::string>()};
        auto uid{data["uid"].get<uint64_t>()};
        auto guardLevel{data["guard_level"].get<unsigned>()};
        this->user             = User(uid, uname, guardLevel);
        this->giftCount        = message["data"]["num"];
        this->giftPrice        = message["data"]["price"];
        this->giftName         = message["data"]["giftName"];
        this->OriginalGiftName = message["data"]["original_gift_name"];
    }
    catch (const nlohmann::json::exception& e)
    {
        LOG_MESSAGE(LogLevel::ERROR, e.what());
        return false;
    }
    return true;
}

void BiliLiveCommandSendGift::Run() const
{
    LOG_MESSAGE(LogLevel::INFO, this->ToString());
}

// BiliApiUtil::LiveCommand BiliLiveCommandSendGift::GetCommandType() const
// {
//     return commandType;
// }