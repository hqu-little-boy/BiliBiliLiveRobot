#include "SettingsManager.h"

SettingsManager* SettingsManager::m_instance = nullptr;

SettingsManager* SettingsManager::instance()
{
    if (!m_instance)
    {
        m_instance = new SettingsManager();
    }
    return m_instance;
}

SettingsManager* SettingsManager::create(QQmlEngine* qmlEngine, QJSEngine* jsEngine)
{
    Q_UNUSED(qmlEngine)
    Q_UNUSED(jsEngine)
    return instance();
}

SettingsManager::SettingsManager(QObject* parent)
    : QObject(parent)
    , m_settings(new QSettings(this))
    , m_ttsEnabled(true)
    , m_ttsVolume(80)
    , m_ttsSpeed(5)
    , m_ttsVoice("温柔女声")
    , m_welcomeEnabled(true)
    , m_normalUserWelcome("欢迎 {用户名} 进入直播间~")
    , m_captainUserWelcome("热烈欢迎舰长 {用户名} 进入直播间！")
    , m_followThanksEnabled(true)
    , m_followThanksMessage("感谢 {用户名} 的关注，欢迎加入我们！")
    , m_shareThanksEnabled(true)
    , m_shareThanksMessage("感谢 {用户名} 分享直播间！")
    , m_roomId("")
    , m_autoConnect(false)
{
    loadSettings();
}

void SettingsManager::setTtsEnabled(bool enabled)
{
    if (m_ttsEnabled != enabled)
    {
        m_ttsEnabled = enabled;
        emit ttsEnabledChanged();
    }
}

void SettingsManager::setTtsVolume(int volume)
{
    if (m_ttsVolume != volume)
    {
        m_ttsVolume = volume;
        emit ttsVolumeChanged();
    }
}

void SettingsManager::setTtsSpeed(int speed)
{
    if (m_ttsSpeed != speed)
    {
        m_ttsSpeed = speed;
        emit ttsSpeedChanged();
    }
}

void SettingsManager::setTtsVoice(const QString& voice)
{
    if (m_ttsVoice != voice)
    {
        m_ttsVoice = voice;
        emit ttsVoiceChanged();
    }
}

void SettingsManager::setWelcomeEnabled(bool enabled)
{
    if (m_welcomeEnabled != enabled)
    {
        m_welcomeEnabled = enabled;
        emit welcomeEnabledChanged();
    }
}

void SettingsManager::setNormalUserWelcome(const QString& welcome)
{
    if (m_normalUserWelcome != welcome)
    {
        m_normalUserWelcome = welcome;
        emit normalUserWelcomeChanged();
    }
}

void SettingsManager::setCaptainUserWelcome(const QString& welcome)
{
    if (m_captainUserWelcome != welcome)
    {
        m_captainUserWelcome = welcome;
        emit captainUserWelcomeChanged();
    }
}

void SettingsManager::setFollowThanksEnabled(bool enabled)
{
    if (m_followThanksEnabled != enabled)
    {
        m_followThanksEnabled = enabled;
        emit followThanksEnabledChanged();
    }
}

void SettingsManager::setFollowThanksMessage(const QString& message)
{
    if (m_followThanksMessage != message)
    {
        m_followThanksMessage = message;
        emit followThanksMessageChanged();
    }
}

void SettingsManager::setShareThanksEnabled(bool enabled)
{
    if (m_shareThanksEnabled != enabled)
    {
        m_shareThanksEnabled = enabled;
        emit shareThanksEnabledChanged();
    }
}

void SettingsManager::setShareThanksMessage(const QString& message)
{
    if (m_shareThanksMessage != message)
    {
        m_shareThanksMessage = message;
        emit shareThanksMessageChanged();
    }
}

void SettingsManager::setRoomId(const QString& roomId)
{
    if (m_roomId != roomId)
    {
        m_roomId = roomId;
        emit roomIdChanged();
    }
}

void SettingsManager::setAutoConnect(bool autoConnect)
{
    if (m_autoConnect != autoConnect)
    {
        m_autoConnect = autoConnect;
        emit autoConnectChanged();
    }
}

void SettingsManager::saveSettings()
{
    // TTS设置
    m_settings->setValue("tts/enabled", m_ttsEnabled);
    m_settings->setValue("tts/volume", m_ttsVolume);
    m_settings->setValue("tts/speed", m_ttsSpeed);
    m_settings->setValue("tts/voice", m_ttsVoice);

    // 欢迎设置
    m_settings->setValue("welcome/enabled", m_welcomeEnabled);
    m_settings->setValue("welcome/normalUser", m_normalUserWelcome);
    m_settings->setValue("welcome/captainUser", m_captainUserWelcome);

    // 关注答谢设置
    m_settings->setValue("follow/enabled", m_followThanksEnabled);
    m_settings->setValue("follow/message", m_followThanksMessage);

    // 分享感谢设置
    m_settings->setValue("share/enabled", m_shareThanksEnabled);
    m_settings->setValue("share/message", m_shareThanksMessage);

    // 直播间设置
    m_settings->setValue("room/id", m_roomId);
    m_settings->setValue("room/autoConnect", m_autoConnect);

    m_settings->sync();
}

void SettingsManager::saveFollowSettings()
{
    m_settings->setValue("follow/enabled", m_followThanksEnabled);
    m_settings->setValue("follow/message", m_followThanksMessage);
    m_settings->sync();
}

void SettingsManager::saveShareSettings()
{
    m_settings->setValue("share/enabled", m_shareThanksEnabled);
    m_settings->setValue("share/message", m_shareThanksMessage);
    m_settings->sync();
}

void SettingsManager::loadSettings()
{
    // TTS设置
    m_ttsEnabled = m_settings->value("tts/enabled", true).toBool();
    m_ttsVolume  = m_settings->value("tts/volume", 80).toInt();
    m_ttsSpeed   = m_settings->value("tts/speed", 5).toInt();
    m_ttsVoice   = m_settings->value("tts/voice", "温柔女声").toString();

    // 欢迎设置
    m_welcomeEnabled = m_settings->value("welcome/enabled", true).toBool();
    m_normalUserWelcome =
        m_settings->value("welcome/normalUser", "欢迎 {用户名} 进入直播间~").toString();
    m_captainUserWelcome =
        m_settings->value("welcome/captainUser", "热烈欢迎舰长 {用户名} 进入直播间！").toString();

    // 关注答谢设置
    m_followThanksEnabled = m_settings->value("follow/enabled", true).toBool();
    m_followThanksMessage =
        m_settings->value("follow/message", "感谢 {用户名} 的关注，欢迎加入我们！").toString();

    // 分享感谢设置
    m_shareThanksEnabled = m_settings->value("share/enabled", true).toBool();
    m_shareThanksMessage =
        m_settings->value("share/message", "感谢 {用户名} 分享直播间！").toString();

    // 直播间设置
    m_roomId      = m_settings->value("room/id", "").toString();
    m_autoConnect = m_settings->value("room/autoConnect", false).toBool();
}

void SettingsManager::resetToDefaults()
{
    m_ttsEnabled          = true;
    m_ttsVolume           = 80;
    m_ttsSpeed            = 5;
    m_ttsVoice            = "温柔女声";
    m_welcomeEnabled      = true;
    m_normalUserWelcome   = "欢迎 {用户名} 进入直播间~";
    m_captainUserWelcome  = "热烈欢迎舰长 {用户名} 进入直播间！";
    m_followThanksEnabled = true;
    m_followThanksMessage = "感谢 {用户名} 的关注，欢迎加入我们！";
    m_shareThanksEnabled  = true;
    m_shareThanksMessage  = "感谢 {用户名} 分享直播间！";
    m_roomId              = "";
    m_autoConnect         = false;

    // 发出所有变更信号
    emit ttsEnabledChanged();
    emit ttsVolumeChanged();
    emit ttsSpeedChanged();
    emit ttsVoiceChanged();
    emit welcomeEnabledChanged();
    emit normalUserWelcomeChanged();
    emit captainUserWelcomeChanged();
    emit followThanksEnabledChanged();
    emit followThanksMessageChanged();
    emit shareThanksEnabledChanged();
    emit shareThanksMessageChanged();
    emit roomIdChanged();
    emit autoConnectChanged();
}
