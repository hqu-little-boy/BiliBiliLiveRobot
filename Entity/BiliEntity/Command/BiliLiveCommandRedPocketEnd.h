//
// Created by zeng on 24-7-8.
//

#ifndef BILILIVECOMMANDREDPOCKETEND_H
#define BILILIVECOMMANDREDPOCKETEND_H
#include "BiliLiveCommandBase.h"


class BiliLiveCommandRedPocketEnd : public BiliLiveCommandBase
{
public:
    explicit BiliLiveCommandRedPocketEnd();
    ~BiliLiveCommandRedPocketEnd() override = default;

    [[nodiscard]] virtual std::string ToString() const override;
    virtual bool                      LoadMessage(const nlohmann::json& message) override;
    virtual void                      Run() const override;

private:
    const static BiliApiUtil::LiveCommand commandType =
        BiliApiUtil::LiveCommand::POPULARITY_RED_POCKET_WINNER_LIST;
};



#endif   // BILILIVECOMMANDREDPOCKETEND_H
