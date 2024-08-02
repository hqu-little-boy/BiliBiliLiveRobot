//
// Created by zeng on 24-7-6.
//

#include "BiliLiveCommandDanmu.h"

#include "../../../Util/BiliUtil/BiliRequestHeader.h"
#include "../../Global/Config.h"
#include "../../Global/Logger.h"
#include "../../MessageDeque/MessageDeque.h"
// {"cmd":"DANMU_MSG","dm_v2":"","info":[[0,1,25,5566168,1720264015935,-522008777,0,"cc596878",0,0,0,"",0,"{}","{}",{"extra":"{\"send_from_me\":false,\"mode\":0,\"color\":5566168,\"dm_type\":0,\"font_size\":25,\"player_mode\":1,\"show_player_type\":0,\"content\":\"我没卡\",\"user_hash\":\"3428411512\",\"emoticon_unique\":\"\",\"bulge_display\":0,\"recommend_score\":6,\"main_state_dm_color\":\"\",\"objective_state_dm_color\":\"\",\"direction\":0,\"pk_direction\":0,\"quartet_direction\":0,\"anniversary_crowd\":0,\"yeah_space_type\":\"\",\"yeah_space_url\":\"\",\"jump_to_url\":\"\",\"space_type\":\"\",\"space_url\":\"\",\"animation\":{},\"emots\":null,\"is_audited\":false,\"id_str\":\"51db89362755b0d8531a911a2366892589\",\"icon\":null,\"show_reply\":true,\"reply_mid\":0,\"reply_uname\":\"\",\"reply_uname_color\":\"\",\"reply_is_mystery\":false,\"hit_combo\":0}","mode":0,"show_player_type":0,"user":{"base":{"face":"https://i1.hdslb.com/bfs/face/a4eeaac2b5648372ecfed8bce1b0aa6c058bfb64.webp","is_mystery":false,"name":"VonSchwa2eL1zleT","name_color":0,"name_color_str":"","official_info":{"desc":"","role":0,"title":"","type":-1},"origin_info":{"face":"https://i1.hdslb.com/bfs/face/a4eeaac2b5648372ecfed8bce1b0aa6c058bfb64.webp","name":"VonSchwa2eL1zleT"},"risk_ctrl_info":null},"guard":null,"guard_leader":{"is_guard_leader":false},"medal":{"color":9272486,"color_border":9272486,"color_end":9272486,"color_start":9272486,"guard_icon":"","guard_level":0,"honor_icon":"","id":1109359,"is_light":1,"level":11,"name":"爱D撒V","ruid":3493118999529865,"score":21652,"typ":0,"user_receive_count":0,"v2_medal_color_border":"#596FE099","v2_medal_color_end":"#596FE099","v2_medal_color_level":"#000B7099","v2_medal_color_start":"#596FE099","v2_medal_color_text":"#FFFFFFFF"},"title":{"old_title_css_id":"","title_css_id":""},"uhead_frame":null,"uid":99405769,"wealth":null}},{"activity_identity":"","activity_source":0,"not_show":0},0],"我没卡",[99405769,"VonSchwa2eL1zleT",0,0,0,10000,1,""],[11,"爱D撒V","脑虎7月17周年啦",27919461,9272486,"",0,9272486,9272486,9272486,0,1,3493118999529865],[10,0,9868950,">50000",0],["",""],0,0,null,{"ct":"8E96A2BD","ts":1720264015},0,0,null,null,0,286,[17],null]}
BiliLiveCommandDanmu::BiliLiveCommandDanmu()
    : BiliLiveCommandBase()
    , user{0, "", 0}
    , content{""}
{
}

std::string BiliLiveCommandDanmu::ToString() const
{
    return FORMAT("{}: {}", this->user.GetUname(), this->content);
}

bool BiliLiveCommandDanmu::LoadMessage(const nlohmann::json& message)
{
    try
    {
        const auto& info     = message["info"];
        const auto& userInfo = info[2];
        this->user           = User{
            userInfo[0].get<uint64_t>(), userInfo[1].get<std::string>(), info[7].get<unsigned>()};
        this->content = info[1].get<std::string>();
    }
    catch (const nlohmann::json::exception& e)
    {
        LOG_MESSAGE(LogLevel::Error, e.what());
    }
    return true;
}

void BiliLiveCommandDanmu::Run() const
{
    LOG_MESSAGE(LogLevel::Info, this->ToString());
    if (user.GetUid() == 0 ||
        user.GetUid() == BiliRequestHeader::GetInstance()->GetBiliCookie().GetDedeUserID())
    {
        return;
    }

    if (content == "抽签" && Config::GetInstance()->CanDrawByLot())
    {
        MessageDeque::GetInstance()->PushWaitedMessage(
            FORMAT("{} 你的抽奖结果是", this->user.GetUname()));
        MessageDeque::GetInstance()->PushWaitedMessage(Config::GetInstance()->GetDrawByLotWord());
    }
}

// BiliApiUtil::LiveCommand BiliLiveCommandDanmu::GetCommandType() const
// {
//     return commandType;
// }
