//
// Created by zeng on 24-7-8.
//

#ifndef BILILIVECOMMANDPREPARING_H
#define BILILIVECOMMANDPREPARING_H


#include "BiliLiveCommandBase.h"

class BiliLiveCommandPreparing : public BiliLiveCommandBase
{
public:
    explicit BiliLiveCommandPreparing(const nlohmann::json& message);
    ~BiliLiveCommandPreparing() override = default;

    [[nodiscard]] virtual std::string ToString() const override;
    virtual bool                      LoadMessage(const nlohmann::json& message) override;
    virtual void                      Run() const override;

private:
    static constexpr BiliApiUtil::LiveCommand commandType =
        BiliApiUtil::LiveCommand::PREPARING;
};



#endif   // BILILIVECOMMANDPREPARING_H
