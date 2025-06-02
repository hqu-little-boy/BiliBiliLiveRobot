#ifndef TTSMANAGER_H
#define TTSMANAGER_H

#include <QObject>
#include <QQmlEngine>
#include <QQueue>
#include <QTimer>
#include <QtQmlIntegration>

struct TTSMessage
{
    QString text;
    QString voice;
    int     volume;
    int     speed;
};

class TTSManager : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    // Q_PROPERTY(bool isEnabled READ isEnabled WRITE setIsEnabled NOTIFY isEnabledChanged)
    // Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY isPlayingChanged)
    // Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
    // Q_PROPERTY(int speed READ speed WRITE setSpeed NOTIFY speedChanged)
    // Q_PROPERTY(QString voice READ voice WRITE setVoice NOTIFY voiceChanged)
    // Q_PROPERTY(QStringList availableVoices READ availableVoices CONSTANT)

private:
    explicit TTSManager(QObject* parent = nullptr);

public:
    static TTSManager* instance();
    static TTSManager* create(QQmlEngine* qmlEngine, QJSEngine* jsEngine);

    Q_INVOKABLE bool isEnabled() const
    {
        return m_isEnabled;
    }
    Q_INVOKABLE void setIsEnabled(bool enabled);

    Q_INVOKABLE bool isPlaying() const
    {
        return m_isPlaying;
    }

    Q_INVOKABLE int volume() const
    {
        return m_volume;
    }
    Q_INVOKABLE void setVolume(int volume);

    Q_INVOKABLE int speed() const
    {
        return m_speed;
    }
    Q_INVOKABLE void setSpeed(int speed);

    Q_INVOKABLE QString voice() const
    {
        return m_voice;
    }
    Q_INVOKABLE void setVoice(const QString& voice);

    Q_INVOKABLE QStringList availableVoices() const
    {
        return m_availableVoices;
    }

public slots:
    void speak(const QString& text);

    void speakWithSettings(const QString& text, const QString& voice, int volume, int speed);

    void pause();

    void resume();

    void stop();

    void clearQueue();

    // 测试功能
    void testSpeak();

signals:
    void isEnabledChanged();

    void isPlayingChanged();

    void volumeChanged();

    void speedChanged();

    void voiceChanged();

    void speechStarted();

    void speechFinished();

    void speechError(const QString& error);

private slots:
    void processQueue();

    void onSpeechFinished();

private:
    static TTSManager* m_instance;

    bool        m_isEnabled;
    bool        m_isPlaying;
    bool        m_isPaused;
    int         m_volume;
    int         m_speed;
    QString     m_voice;
    QStringList m_availableVoices;

    QQueue<TTSMessage> m_messageQueue;
    QTimer*            m_processTimer;

    void initializeVoices();

    void startSpeaking(const TTSMessage& message);
};

#endif   // TTSMANAGER_H
