//
// Created by zeng on 24-7-8.
//

#ifndef BILILIVECOMMANDREDPOCKETSTART_H
#define BILILIVECOMMANDREDPOCKETSTART_H


#include "BiliLiveCommandBase.h"
class BiliLiveCommandRedPocketStart : public BiliLiveCommandBase
{
public:
    explicit BiliLiveCommandRedPocketStart(const nlohmann::json& message);
    ~BiliLiveCommandRedPocketStart() override = default;

    [[nodiscard]] virtual std::string ToString() const override;
    virtual bool                      LoadMessage(const nlohmann::json& message) override;
    virtual void                      Run() const override;

private:
    const static BiliApiUtil::LiveCommand commandType =
        BiliApiUtil::LiveCommand::POPULARITY_RED_POCKET_START;
};



#endif   // BILILIVECOMMANDREDPOCKETSTART_H
