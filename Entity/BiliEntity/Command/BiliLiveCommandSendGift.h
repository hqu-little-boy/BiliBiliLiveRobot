//
// Created by zeng on 24-7-7.
//

#ifndef BILILIVECOMMANDSENDGIFT_H
#define BILILIVECOMMANDSENDGIFT_H
#include "BiliLiveCommandBase.h"

#include <nlohmann/json.hpp>


class BiliLiveCommandSendGift : public BiliLiveCommandBase
{
public:
    explicit BiliLiveCommandSendGift(const nlohmann::json& message);
    ~BiliLiveCommandSendGift() override = default;

    [[nodiscard]] virtual std::string ToString() const override;
    virtual bool                      LoadMessage(const nlohmann::json& message) override;
    virtual void                      Run() const override;
    // [[nodiscard]] virtual BiliApiUtil::LiveCommand GetCommandType() const;

private:
    User                                      user;
    unsigned                                  giftCount;
    unsigned                                  giftPrice;
    std::string                               giftName;
    std::string                               OriginalGiftName;
    static constexpr BiliApiUtil::LiveCommand commandType = BiliApiUtil::LiveCommand::SEND_GIFT;
};



#endif   // BILILIVECOMMANDSENDGIFT_H
