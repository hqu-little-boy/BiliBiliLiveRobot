//
// Created by zeng on 24-7-13.
//

#ifndef BILILIVECOMMANDGUARDBLINDBOX_H
#define BILILIVECOMMANDGUARDBLINDBOX_H
#include "BiliLiveCommandBase.h"


class BiliLiveCommandGuardBlindBox : public BiliLiveCommandBase
{
public:
    explicit BiliLiveCommandGuardBlindBox(const nlohmann::json& message);
    ~BiliLiveCommandGuardBlindBox() override = default;

    [[nodiscard]] std::string ToString() const override;
    void                      Run() const override;
    bool                      LoadMessage(const nlohmann::json& message) override;
    // [[nodiscard]] BiliApiUtil::LiveCommand GetCommandType() const;

private:
    User                                      user;
    std::string                               content;
    static constexpr BiliApiUtil::LiveCommand commandType =
        BiliApiUtil::LiveCommand::COMMON_NOTICE_DANMAKU;
};



#endif   // BILILIVECOMMANDGUARDBLINDBOX_H
