//
// Created by zeng on 24-7-6.
//

#ifndef LIVECOMMANDDANMU_H
#define LIVECOMMANDDANMU_H
#include "../User.h"
#include "BiliLiveCommandBase.h"

class BiliLiveCommandDanmu : public BiliLiveCommandBase
{
public:
    explicit BiliLiveCommandDanmu();
    ~BiliLiveCommandDanmu() override = default;

    [[nodiscard]] virtual std::string ToString() const override;
    virtual bool                      LoadMessage(const nlohmann::json& message) override;
    virtual void                      Run() const override;
    // [[nodiscard]] virtual BiliApiUtil::LiveCommand GetCommandType() const;

private:
    User                                      user;
    std::string                               content;
    static constexpr BiliApiUtil::LiveCommand commandType = BiliApiUtil::LiveCommand::DANMU_MSG;
};

#endif   // LIVECOMMANDDANMU_H
