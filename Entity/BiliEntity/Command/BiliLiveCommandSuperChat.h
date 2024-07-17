//
// Created by zeng on 24-7-8.
//

#ifndef BILILIVECOMMANDSUPERCHAT_H
#define BILILIVECOMMANDSUPERCHAT_H


#include "BiliLiveCommandBase.h"

class BiliLiveCommandSuperChat : public BiliLiveCommandBase
{
public:
    explicit BiliLiveCommandSuperChat();
    ~BiliLiveCommandSuperChat() override = default;

    [[nodiscard]] virtual std::string ToString() const override;
    virtual bool                      LoadMessage(const nlohmann::json& message) override;
    virtual void                      Run() const override;

private:
    User                                      user;
    unsigned                                  price;
    std::string                               message;
    static constexpr BiliApiUtil::LiveCommand commandType =
        BiliApiUtil::LiveCommand::SUPER_CHAT_MESSAGE;
};



#endif   // BILILIVECOMMANDSUPERCHAT_H
