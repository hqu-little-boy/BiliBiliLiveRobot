//
// Created by zeng on 24-7-7.
//

#ifndef BILILIVECOMMANDGUARDBUY_H
#define BILILIVECOMMANDGUARDBUY_H
#include "BiliLiveCommandBase.h"


class BiliLiveCommandGuardBuy : public BiliLiveCommandBase
{
public:
    explicit BiliLiveCommandGuardBuy();
    ~BiliLiveCommandGuardBuy() override = default;

    [[nodiscard]] std::string              ToString() const override;
    void                                   Run() const override;
    bool                                   LoadMessage(const nlohmann::json& message) override;
    // [[nodiscard]] BiliApiUtil::LiveCommand GetCommandType() const;

private:
    User                                      user;
    unsigned                                  guardLevel;
    unsigned                                  num;
    unsigned                                  price;
    static constexpr BiliApiUtil::LiveCommand commandType = BiliApiUtil::LiveCommand::GUARD_BUY;
};



#endif   // BILILIVECOMMANDGUARDBUY_H
