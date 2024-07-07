//
// Created by zeng on 24-7-6.
//

#ifndef LIVEBASECOMMAND_H
#define LIVEBASECOMMAND_H
#include "../../../Util/BiliUtil/BiliApiUtil.h"
#include "../../Global/TimeStamp.h"

#include <nlohmann/json.hpp>


class BiliLiveCommandBase
{
public:
    explicit BiliLiveCommandBase(const nlohmann::json& message);
    virtual ~BiliLiveCommandBase() = default;

    [[nodiscard]] virtual std::string ToString() const                           = 0;
    virtual bool                      LoadMessage(const nlohmann::json& message) = 0;
    virtual void                      Run() const                                = 0;
    /// @brief 判断上次运行结束时间距离现在是否超过了5min
    [[nodiscard]] bool IsTimeOut() const;
    void               SetTimeStamp();

private:
    TimeStamp                                 timestamp;
    static constexpr BiliApiUtil::LiveCommand commandType = BiliApiUtil::LiveCommand::NONE;
};



#endif   // LIVEBASECOMMAND_H
