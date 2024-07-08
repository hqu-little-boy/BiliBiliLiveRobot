//
// Created by zeng on 24-7-7.
//

#ifndef BILILIVECOMMANDINTERACTWORD_H
#define BILILIVECOMMANDINTERACTWORD_H
#include "../User.h"
#include "BiliLiveCommandBase.h"


class BiliLiveCommandInteractWord : public BiliLiveCommandBase
{
public:
    explicit BiliLiveCommandInteractWord(const nlohmann::json& message);
    ~BiliLiveCommandInteractWord() override = default;

    [[nodiscard]] virtual std::string ToString() const override;
    virtual bool                      LoadMessage(const nlohmann::json& message) override;
    virtual void                      Run() const override;
    // [[nodiscard]] virtual BiliApiUtil::LiveCommand GetCommandType() const;

private:
    unsigned int                              megType;
    User                                      user;
    static constexpr BiliApiUtil::LiveCommand commandType = BiliApiUtil::LiveCommand::INTERACT_WORD;
};



#endif   // BILILIVECOMMANDINTERACTWORD_H
