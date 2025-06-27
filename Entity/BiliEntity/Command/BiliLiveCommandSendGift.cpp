//
// Created by zeng on 24-7-7.
//

#include "BiliLiveCommandSendGift.h"

#include "../../DataBase/DataBase.h"
#include "../../Global/Config.h"
#include "../../MessageDeque/MessageDeque.h"
BiliLiveCommandSendGift::BiliLiveCommandSendGift()
    : BiliLiveCommandBase()
    , user(0, "", 0)
    , giftCount(0)
    , giftPrice(0)
    , giftName("")
    , originalGiftPrice(0)
    , originalGiftName("")
{
}

std::string BiliLiveCommandSendGift::ToString() const
{
    return fmt::format("User: {} 赠送, GiftCount: {}, GiftPrice: {}, GiftName: {}, "
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
    if (Config::GetInstance()->CanThanksBlindBox() && !this->originalGiftName.empty())
    {
        MessageDeque::GetInstance()->AddBlindBoxMessage(
            this->user.GetUname(),
            this->originalGiftName,
            this->giftCount,
            static_cast<int>(this->giftPrice * this->giftCount -
                             this->originalGiftPrice * this->giftCount));
    }
    else if (Config::GetInstance()->CanThanksGift())
    {

        // std::string message{fmt::vformat(
        //     Config::GetInstance()->GetGiftThanksWord(),
        //     fmt::make_format_args(this->user.GetUname(), this->giftCount, this->giftName))};
        MessageDeque::GetInstance()->AddGiftMessage(
            this->user.GetUname(), this->giftName, this->giftCount);
    }
    if (const auto res = DataBase::GetInstance()->AddGift(
            this->giftName, this->giftPrice, this->giftCount, this->user);
        !res)
    {
        LOG_MESSAGE(LogLevel::Error, fmt::format("Failed to add Gift to {}", this->ToString()));
    }
    if (!this->originalGiftName.empty())
    {
        if (const auto res = DataBase::GetInstance()->AddBlindBox(this->originalGiftName,
                                                                  this->originalGiftPrice,
                                                                  this->giftCount,
                                                                  this->giftName,
                                                                  this->giftPrice,
                                                                  this->user);
            !res)
        {
            LOG_MESSAGE(LogLevel::Error,
                        fmt::format("Failed to add Blind Box to {}", this->ToString()));
        }
    }
}

// BiliApiUtil::LiveCommand BiliLiveCommandSendGift::GetCommandType() const
// {
//     return commandType;
// }