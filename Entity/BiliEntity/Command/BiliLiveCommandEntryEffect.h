//
// Created by zeng on 24-7-7.
//

#ifndef BILILIVECOMMANDENTRYEFFECT_H
#define BILILIVECOMMANDENTRYEFFECT_H
#include "../User.h"
#include "BiliLiveCommandBase.h"


class BiliLiveCommandEntryEffect : public BiliLiveCommandBase
{
public:
    explicit BiliLiveCommandEntryEffect();
    ~BiliLiveCommandEntryEffect() override = default;

    [[nodiscard]] virtual std::string ToString() const override;
    virtual bool                      LoadMessage(const nlohmann::json& message) override;
    virtual void                      Run() const override;
    // [[nodiscard]] virtual BiliApiUtil::LiveCommand GetCommandType() const;

private:
    User                                      user;
    static constexpr BiliApiUtil::LiveCommand commandType = BiliApiUtil::LiveCommand::ENTRY_EFFECT;
};



#endif   // BILILIVECOMMANDENTRYEFFECT_H
