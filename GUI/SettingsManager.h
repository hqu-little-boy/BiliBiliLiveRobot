#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QQmlEngine>
#include <QSettings>
#include <QtQmlIntegration>

class SettingsManager : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    // // TTS设置
    // Q_PROPERTY(bool ttsEnabled READ ttsEnabled WRITE setTtsEnabled NOTIFY ttsEnabledChanged)
    // Q_PROPERTY(int ttsVolume READ ttsVolume WRITE setTtsVolume NOTIFY ttsVolumeChanged)
    // Q_PROPERTY(int ttsSpeed READ ttsSpeed WRITE setTtsSpeed NOTIFY ttsSpeedChanged)
    // Q_PROPERTY(QString ttsVoice READ ttsVoice WRITE setTtsVoice NOTIFY ttsVoiceChanged)
    //
    // // 欢迎设置
    // Q_PROPERTY(bool welcomeEnabled READ welcomeEnabled WRITE setWelcomeEnabled NOTIFY
    // welcomeEnabledChanged) Q_PROPERTY(
    //     QString normalUserWelcome READ normalUserWelcome WRITE setNormalUserWelcome NOTIFY
    //     normalUserWelcomeChanged)
    // Q_PROPERTY(
    //     QString captainUserWelcome READ captainUserWelcome WRITE setCaptainUserWelcome NOTIFY
    //     captainUserWelcomeChanged)
    //
    // // 关注答谢设置
    // Q_PROPERTY(
    //     bool followThanksEnabled READ followThanksEnabled WRITE setFollowThanksEnabled NOTIFY
    //     followThanksEnabledChanged)
    // Q_PROPERTY(
    //     QString followThanksMessage READ followThanksMessage WRITE setFollowThanksMessage NOTIFY
    //     followThanksMessageChanged)
    //
    // // 分享感谢设置
    // Q_PROPERTY(
    //     bool shareThanksEnabled READ shareThanksEnabled WRITE setShareThanksEnabled NOTIFY
    //     shareThanksEnabledChanged)
    // Q_PROPERTY(
    //     QString shareThanksMessage READ shareThanksMessage WRITE setShareThanksMessage NOTIFY
    //     shareThanksMessageChanged)
    //
    // // 直播间设置
    // Q_PROPERTY(QString roomId READ roomId WRITE setRoomId NOTIFY roomIdChanged)
    // Q_PROPERTY(bool autoConnect READ autoConnect WRITE setAutoConnect NOTIFY autoConnectChanged)
    //
    // // 盲盒设置
    // Q_PROPERTY(
    //     bool blindBoxTipEnabled READ blindBoxTipEnabled WRITE setBlindBoxTipEnabled NOTIFY
    //     blindBoxTipEnabledChanged)
    // Q_PROPERTY(
    //     QString blindBoxTipMessage READ blindBoxTipMessage WRITE setBlindBoxTipMessage NOTIFY
    //     blindBoxTipMessageChanged)
    // Q_PROPERTY(
    //     int blindBoxRankTabsIndex READ blindBoxRankTabsIndex WRITE setBlindBoxRankTabsIndex
    //     NOTIFY blindBoxRankTabsIndexChanged)

public:
    static SettingsManager* instance();
    static SettingsManager* create(QQmlEngine* qmlEngine, QJSEngine* jsEngine);

    // TTS设置
    Q_INVOKABLE bool ttsEnabled() const
    {
        return m_ttsEnabled;
    }
    Q_INVOKABLE void setTtsEnabled(bool enabled);

    Q_INVOKABLE int ttsVolume() const
    {
        return m_ttsVolume;
    }
    Q_INVOKABLE void setTtsVolume(int volume);

    Q_INVOKABLE int ttsSpeed() const
    {
        return m_ttsSpeed;
    }
    Q_INVOKABLE void setTtsSpeed(int speed);

    Q_INVOKABLE QString ttsVoice() const
    {
        return m_ttsVoice;
    }
    Q_INVOKABLE void setTtsVoice(const QString& voice);

    // 欢迎设置
    Q_INVOKABLE bool welcomeEnabled() const
    {
        return m_welcomeEnabled;
    }
    Q_INVOKABLE void setWelcomeEnabled(bool enabled);

    Q_INVOKABLE QString normalUserWelcome() const
    {
        return m_normalUserWelcome;
    }
    Q_INVOKABLE void setNormalUserWelcome(const QString& welcome);

    Q_INVOKABLE QString captainUserWelcome() const
    {
        return m_captainUserWelcome;
    }
    Q_INVOKABLE void setCaptainUserWelcome(const QString& welcome);

    // 礼物设置
    Q_INVOKABLE bool giftEnabled() const
    {
        return m_giftEnabled;
    }
    Q_INVOKABLE void setGiftEnabled(bool enabled)
    {
        m_giftEnabled = enabled;
    }
    Q_INVOKABLE QString giftMessage() const
    {
        return m_giftMessage;
    }
    Q_INVOKABLE void setGiftMessage(const QString& message)
    {
        m_giftMessage = message;
    }
    Q_INVOKABLE uint32_t giftThanksDelay() const
    {
        return m_giftThanksDelay;
    }
    Q_INVOKABLE void setGiftThanksDelay(uint32_t delay)
    {
        m_giftThanksDelay = delay;
    }
    // 关注答谢设置
    Q_INVOKABLE bool followThanksEnabled() const
    {
        return m_followThanksEnabled;
    }
    Q_INVOKABLE void setFollowThanksEnabled(bool enabled);

    Q_INVOKABLE QString followThanksMessage() const
    {
        return m_followThanksMessage;
    }
    Q_INVOKABLE void setFollowThanksMessage(const QString& message);

    // 分享感谢设置
    Q_INVOKABLE bool shareThanksEnabled() const
    {
        return m_shareThanksEnabled;
    }
    Q_INVOKABLE void setShareThanksEnabled(bool enabled);

    Q_INVOKABLE QString shareThanksMessage() const
    {
        return m_shareThanksMessage;
    }
    Q_INVOKABLE void setShareThanksMessage(const QString& message);

    // 直播间设置
    Q_INVOKABLE QString roomId() const
    {
        return m_roomId;
    }
    Q_INVOKABLE void setRoomId(const QString& roomId);

    Q_INVOKABLE bool autoConnect() const
    {
        return m_autoConnect;
    }
    Q_INVOKABLE void setAutoConnect(bool autoConnect);

    // 盲盒设置
    Q_INVOKABLE bool blindBoxTipEnabled() const
    {
        return m_blindBoxTipEnabled;
    }
    Q_INVOKABLE void setBlindBoxTipEnabled(bool enabled)
    {
        m_blindBoxTipEnabled = enabled;
    }
    Q_INVOKABLE QString blindBoxTipMessage() const
    {
        return m_blindBoxTipMessage;
    }
    Q_INVOKABLE void setBlindBoxTipMessage(const QString& message)
    {
        m_blindBoxTipMessage = message;
    }
    Q_INVOKABLE int blindBoxRankTabsIndex() const
    {
        return m_blindBoxRankTabsIndex;
    }
    Q_INVOKABLE void setBlindBoxRankTabsIndex(int index)
    {
        m_blindBoxRankTabsIndex = index;
    }

public slots:
    void saveSettings();
    void saveFollowSettings();
    void saveShareSettings();

    void loadSettings();

    void resetToDefaults();

signals:
    void ttsEnabledChanged();

    void ttsVolumeChanged();

    void ttsSpeedChanged();

    void ttsVoiceChanged();

    void welcomeEnabledChanged();

    void normalUserWelcomeChanged();

    void captainUserWelcomeChanged();

    void followThanksEnabledChanged();

    void followThanksMessageChanged();

    void shareThanksEnabledChanged();

    void shareThanksMessageChanged();

    void roomIdChanged();

    void autoConnectChanged();

    void blindBoxTipEnabledChanged();
    void blindBoxTipMessageChanged();
    void blindBoxRankTabsIndexChanged();

private:
    explicit SettingsManager(QObject* parent = nullptr);

private:
    static SettingsManager* m_instance;

    QSettings* m_settings;

    // TTS设置
    bool    m_ttsEnabled;
    int     m_ttsVolume;
    int     m_ttsSpeed;
    QString m_ttsVoice;

    // 欢迎设置
    bool    m_welcomeEnabled;
    QString m_normalUserWelcome;
    QString m_captainUserWelcome;

    // 礼物设置
    bool     m_giftEnabled;
    QString  m_giftMessage;
    uint32_t m_giftThanksDelay;

    // 关注答谢设置
    bool    m_followThanksEnabled;
    QString m_followThanksMessage;

    // 分享感谢设置
    bool    m_shareThanksEnabled;
    QString m_shareThanksMessage;

    // 直播间设置
    QString m_roomId;
    bool    m_autoConnect;

    // 盲盒设置
    bool    m_blindBoxTipEnabled;
    QString m_blindBoxTipMessage;
    int     m_blindBoxRankTabsIndex;
};

#endif   // SETTINGSMANAGER_H
