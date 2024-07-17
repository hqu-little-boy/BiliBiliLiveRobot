//
// Created by zeng on 24-7-6.
//

#ifndef BILICOMMANDFACTORY_H
#define BILICOMMANDFACTORY_H
#include "../../Util/BiliUtil/BiliApiUtil.h"
#include "Command/BiliLiveCommandBase.h"

#include <nlohmann/json_fwd.hpp>


// 单例模式，用于创建BiliCommand
class BiliCommandFactory
{
public:
    static BiliCommandFactory*           GetInstance();
    std::unique_ptr<BiliLiveCommandBase> GetCommand(BiliApiUtil::LiveCommand eCommand,
                                                       const nlohmann::json&    message);

private:
    BiliCommandFactory();
    ~BiliCommandFactory()                                    = default;
    BiliCommandFactory(const BiliCommandFactory&)            = delete;
    BiliCommandFactory& operator=(const BiliCommandFactory&) = delete;

    static BiliCommandFactory* pInstance;
    std::unordered_map<BiliApiUtil::LiveCommand,
                       std::function<std::unique_ptr<BiliLiveCommandBase>()>>
        commandMap;
};



#endif   // BILICOMMANDFACTORY_H
