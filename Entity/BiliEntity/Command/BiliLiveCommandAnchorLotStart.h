//
// Created by zeng on 24-7-8.
//

#ifndef BILILIVECOMMANDANCHORLOTSTART_H
#define BILILIVECOMMANDANCHORLOTSTART_H


#include "BiliLiveCommandBase.h"

class BiliLiveCommandAnchorLotStart : public BiliLiveCommandBase
{
public:
    explicit BiliLiveCommandAnchorLotStart();
    ~BiliLiveCommandAnchorLotStart() override = default;

    [[nodiscard]] virtual std::string ToString() const override;
    virtual bool                      LoadMessage(const nlohmann::json& message) override;
    virtual void                      Run() const override;

private:
    static constexpr BiliApiUtil::LiveCommand commandType =
        BiliApiUtil::LiveCommand::ANCHOR_LOT_START;
};



#endif   // BILILIVECOMMANDANCHORLOTSTART_H
