//
// Created by zeng on 24-7-8.
//

#ifndef BILILIVECOMMANDANCHORLOTEND_H
#define BILILIVECOMMANDANCHORLOTEND_H


#include "BiliLiveCommandBase.h"

class BiliLiveCommandAnchorLotEnd : public BiliLiveCommandBase{
public:
    explicit BiliLiveCommandAnchorLotEnd(const nlohmann::json& message);
    ~BiliLiveCommandAnchorLotEnd() override = default;

    [[nodiscard]] virtual std::string ToString() const override;
    virtual bool                      LoadMessage(const nlohmann::json& message) override;
    virtual void                      Run() const override;
private:
    static constexpr BiliApiUtil::LiveCommand commandType = BiliApiUtil::LiveCommand::ANCHOR_LOT_END;

};



#endif //BILILIVECOMMANDANCHORLOTEND_H
