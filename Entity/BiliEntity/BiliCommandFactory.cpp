//
// Created by zeng on 24-7-6.
//

#include "BiliCommandFactory.h"

#include "../Global/MessageDeque.h"
#include "Command/BiliLiveCommandAnchorLotEnd.h"
#include "Command/BiliLiveCommandAnchorLotStart.h"
#include "Command/BiliLiveCommandDanmu.h"
#include "Command/BiliLiveCommandEntryEffect.h"
#include "Command/BiliLiveCommandGuardBuy.h"
#include "Command/BiliLiveCommandInteractWord.h"
#include "Command/BiliLiveCommandLive.h"
#include "Command/BiliLiveCommandPreparing.h"
#include "Command/BiliLiveCommandRedPocketEnd.h"
#include "Command/BiliLiveCommandRedPocketStart.h"
#include "Command/BiliLiveCommandSendGift.h"
#include "Command/BiliLiveCommandSuperChat.h"

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
            return std::unique_ptr<BiliLiveCommandBase>(pCommand);
        }
        return this->commandMap[eCommand](message);
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
    this->commandMap[BiliApiUtil::LiveCommand::GUARD_BUY] = CreateCommand<BiliLiveCommandGuardBuy>;
    this->commandMap[BiliApiUtil::LiveCommand::SUPER_CHAT_MESSAGE] =
        CreateCommand<BiliLiveCommandSuperChat>;
    this->commandMap[BiliApiUtil::LiveCommand::ANCHOR_LOT_START] =
        CreateCommand<BiliLiveCommandAnchorLotStart>;
    this->commandMap[BiliApiUtil::LiveCommand::ANCHOR_LOT_END] =
        CreateCommand<BiliLiveCommandAnchorLotEnd>;
    this->commandMap[BiliApiUtil::LiveCommand::LIVE]      = CreateCommand<BiliLiveCommandLive>;
    this->commandMap[BiliApiUtil::LiveCommand::PREPARING] = CreateCommand<BiliLiveCommandPreparing>;
    this->commandMap[BiliApiUtil::LiveCommand::POPULARITY_RED_POCKET_NEW] =
        CreateCommand<BiliLiveCommandRedPocketStart>;
    this->commandMap[BiliApiUtil::LiveCommand::POPULARITY_RED_POCKET_WINNER_LIST] =
        CreateCommand<BiliLiveCommandRedPocketEnd>;
}