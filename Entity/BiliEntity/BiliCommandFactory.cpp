//
// Created by zeng on 24-7-6.
//

#include "BiliCommandFactory.h"

#include "../Global/MessageDeque.h"
#include "Command/BiliLiveCommandDanmu.h"
#include "Command/BiliLiveCommandEntryEffect.h"
#include "Command/BiliLiveCommandInteractWord.h"
#include "Command/BiliLiveCommandSendGift.h"

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

std::unique_ptr<BiliLiveCommandBase> BiliCommandFactory::GetCommand(
    BiliApiUtil::LiveCommand eCommand, const nlohmann::json& message)
{
    if (eCommand == BiliApiUtil::LiveCommand::NONE || eCommand == BiliApiUtil::LiveCommand::OTHER)
    {
        return nullptr;
    }
    if (this->commandMap.contains(eCommand))
    {
        auto pCommand{MessageDeque::GetInstance()->PopCommandPool(eCommand)};
        if (pCommand != nullptr && pCommand->LoadMessage(message))
        {
            return std::move(std::unique_ptr<BiliLiveCommandBase>(pCommand));
        }
        return std::move(this->commandMap[eCommand](message));
    }
    return nullptr;
}

BiliCommandFactory::BiliCommandFactory()
{
    this->commandMap[BiliApiUtil::LiveCommand::DANMU_MSG] = CreateCommand<BiliLiveCommandDanmu>;
    // this->commandMap[BiliApiUtil::LiveCommand::ENTRY_EFFECT] =
    //     CreateCommand<BiliLiveCommandEntryEffect>;
    this->commandMap[BiliApiUtil::LiveCommand::INTERACT_WORD] =
        CreateCommand<BiliLiveCommandInteractWord>;
    this->commandMap[BiliApiUtil::LiveCommand::SEND_GIFT] = CreateCommand<BiliLiveCommandSendGift>;
}