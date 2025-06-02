#include "TTSManager.h"

#include <QDebug>

TTSManager* TTSManager::m_instance = nullptr;

TTSManager* TTSManager::instance()
{
    if (!m_instance)
    {
        m_instance = new TTSManager();
    }
    return m_instance;
}

TTSManager* TTSManager::create(QQmlEngine* qmlEngine, QJSEngine* jsEngine)
{
    Q_UNUSED(qmlEngine)
    Q_UNUSED(jsEngine)
    return instance();
}

TTSManager::TTSManager(QObject* parent)
    : QObject(parent)
    , m_isEnabled(true)
    , m_isPlaying(false)
    , m_isPaused(false)
    , m_volume(80)
    , m_speed(5)
    , m_voice("温柔女声")
    , m_processTimer(new QTimer(this))
{
    initializeVoices();

    connect(m_processTimer, &QTimer::timeout, this, &TTSManager::processQueue);
    m_processTimer->setSingleShot(true);
}

void TTSManager::initializeVoices()
{
    m_availableVoices << "温柔女声" << "活泼女声" << "成熟男声" << "童声" << "机器音";
}

void TTSManager::setIsEnabled(bool enabled)
{
    if (m_isEnabled != enabled)
    {
        m_isEnabled = enabled;
        if (!enabled)
        {
            stop();
        }
        emit isEnabledChanged();
    }
}

void TTSManager::setVolume(int volume)
{
    if (m_volume != volume)
    {
        m_volume = qBound(0, volume, 100);
        emit volumeChanged();
    }
}

void TTSManager::setSpeed(int speed)
{
    if (m_speed != speed)
    {
        m_speed = qBound(1, speed, 10);
        emit speedChanged();
    }
}

void TTSManager::setVoice(const QString& voice)
{
    if (m_voice != voice && m_availableVoices.contains(voice))
    {
        m_voice = voice;
        emit voiceChanged();
    }
}

void TTSManager::speak(const QString& text)
{
    if (!m_isEnabled || text.isEmpty())
    {
        return;
    }

    TTSMessage message;
    message.text   = text;
    message.voice  = m_voice;
    message.volume = m_volume;
    message.speed  = m_speed;

    m_messageQueue.enqueue(message);

    if (!m_isPlaying && !m_isPaused)
    {
        processQueue();
    }
}

void TTSManager::speakWithSettings(const QString& text, const QString& voice, int volume, int speed)
{
    if (!m_isEnabled || text.isEmpty())
    {
        return;
    }

    TTSMessage message;
    message.text   = text;
    message.voice  = voice;
    message.volume = qBound(0, volume, 100);
    message.speed  = qBound(1, speed, 10);

    m_messageQueue.enqueue(message);

    if (!m_isPlaying && !m_isPaused)
    {
        processQueue();
    }
}

void TTSManager::pause()
{
    if (m_isPlaying)
    {
        m_isPaused = true;
        // TODO: 实现真实的TTS暂停逻辑
        qDebug() << "TTS paused";
    }
}

void TTSManager::resume()
{
    if (m_isPaused)
    {
        m_isPaused = false;
        // TODO: 实现真实的TTS恢复逻辑
        if (!m_messageQueue.isEmpty())
        {
            processQueue();
        }
        qDebug() << "TTS resumed";
    }
}

void TTSManager::stop()
{
    m_isPlaying = false;
    m_isPaused  = false;
    m_processTimer->stop();
    clearQueue();

    // TODO: 实现真实的TTS停止逻辑
    qDebug() << "TTS stopped";

    emit isPlayingChanged();
    emit speechFinished();
}

void TTSManager::clearQueue()
{
    m_messageQueue.clear();
}

void TTSManager::testSpeak()
{
    speak("这是一个语音测试，您好！");
}

void TTSManager::processQueue()
{
    if (m_isPaused || m_messageQueue.isEmpty())
    {
        return;
    }

    TTSMessage message = m_messageQueue.dequeue();
    startSpeaking(message);
}

void TTSManager::startSpeaking(const TTSMessage& message)
{
    m_isPlaying = true;
    emit isPlayingChanged();
    emit speechStarted();

    // TODO: 实现真实的TTS播放逻辑
    // 这里只是模拟播放过程
    qDebug() << "Speaking:" << message.text << "Voice:" << message.voice
             << "Volume:" << message.volume << "Speed:" << message.speed;

    // 模拟播放时间（根据文本长度和语速）
    int duration = (message.text.length() * 100) / message.speed;
    m_processTimer->start(qMax(500, duration));
}

void TTSManager::onSpeechFinished()
{
    m_isPlaying = false;
    emit isPlayingChanged();
    emit speechFinished();

    // 继续处理队列中的下一条消息
    if (!m_messageQueue.isEmpty() && !m_isPaused)
    {
        m_processTimer->start(100);   // 短暂延迟后处理下一条
    }
}
