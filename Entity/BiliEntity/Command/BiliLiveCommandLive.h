//
// Created by zeng on 24-7-8.
//

#ifndef BILILIVECOMMANDLIVE_H
#define BILILIVECOMMANDLIVE_H


#include "BiliLiveCommandBase.h"

class BiliLiveCommandLive : public BiliLiveCommandBase
{
public:
    explicit BiliLiveCommandLive();
    ~BiliLiveCommandLive() override = default;

    [[nodiscard]] virtual std::string ToString() const override;
    virtual bool                      LoadMessage(const nlohmann::json& message) override;
    virtual void                      Run() const override;

private:
    static constexpr BiliApiUtil::LiveCommand commandType = BiliApiUtil::LiveCommand::LIVE;
};



#endif   // BILILIVECOMMANDLIVE_H
