//
// Created by zeng on 24-7-6.
//

#include "BiliCommandFactory.h"

#include "Command/BiliLiveCommandDanmu.h"
template<typename T>
std::unique_ptr<BiliLiveCommandBase> CreateCommand(const nlohmann::json& message)
{
    return std::make_unique<T>(message);
}

BiliCommandFactory* BiliCommandFactory::pInstance{new BiliCommandFactory()};

BiliCommandFactory* BiliCommandFactory::GetInstance()
{
    if (pInstance == nullptr)
    {
        assert(false);
        pInstance = new BiliCommandFactory();
    }
    return pInstance;
}

std::unique_ptr<BiliLiveCommandBase> BiliCommandFactory::ProduceCommand(
    BiliApiUtil::LiveCommand eCommand, const nlohmann::json& message)
{
    if (eCommand == BiliApiUtil::LiveCommand::NONE || eCommand == BiliApiUtil::LiveCommand::OTHER)
    {
        return nullptr;
    }
    if (this->commandMap.contains(eCommand))
    {
        return std::move(this->commandMap[eCommand](message));
    }
    return nullptr;
}

BiliCommandFactory::BiliCommandFactory()
{
    this->commandMap[BiliApiUtil::LiveCommand::DANMU_MSG] = CreateCommand<BiliLiveCommandDanmu>;
}