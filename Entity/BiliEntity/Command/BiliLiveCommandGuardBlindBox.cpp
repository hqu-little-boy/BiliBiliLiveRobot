//
// Created by zeng on 24-7-13.
//

#include "BiliLiveCommandGuardBlindBox.h"

BiliLiveCommandGuardBlindBox::BiliLiveCommandGuardBlindBox(const nlohmann::json& message)
    : BiliLiveCommandBase(message)
    , user(0, "", 0)
    , content("")
{
}

std::string BiliLiveCommandGuardBlindBox::ToString() const
{
    return std::format("BiliLiveCommandGuardBlindBox: user: {}, content: {}",
                       this->user.GetUname(),
                       this->content);
}

void BiliLiveCommandGuardBlindBox::Run() const
{
    LOG_MESSAGE(LogLevel::Info, this->ToString());
}

bool BiliLiveCommandGuardBlindBox::LoadMessage(const nlohmann::json& message)
{
    LOG_VAR(LogLevel::Debug, message.dump(-1));
    try
    {
        const auto& data{message["data"]};
        if (data["content_segments"][1]["text"].get<std::string>() != "投喂" ||
            data["content_segments"][2]["text"].get<std::string>() != "大航海盲盒")
        {
            LOG_VAR(LogLevel::Warn, message.dump(-1));
            return false;
        }
        this->user.SetUname(data["content_segments"][0]["text"].get<std::string>());
        this->user.SetUname(data["content_segments"][4]["text"].get<std::string>());
    }
    catch (const nlohmann::json::exception& e)
    {
        LOG_MESSAGE(LogLevel::Error, e.what());
        return false;
    }
    return true;
}